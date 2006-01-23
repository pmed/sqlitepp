//////////////////////////////////////////////////////////////////////////////
// statement.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/statement.hpp"

#include <algorithm>
#include <numeric>

#include <sqlite3.h>

#include "sqlitepp/binders.hpp"
#include "sqlitepp/session.hpp"
#include "sqlitepp/exception.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

namespace { // implementation details

//////////////////////////////////////////////////////////////////////////////

struct update
{
	update(statement& st) : st_(st) {}
	update& operator=(update const&);

	void operator() (into_binder* b) const
	{
		b->update(st_);
	}

	statement& st_;
};
//----------------------------------------------------------------------------

struct bind
{
	bind(statement& st) : st_(st) {}
	bind& operator=(bind const&);

	int operator()(int pos, binder* b) const
	{
		return b->bind(st_, pos);
	}

	statement& st_;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace { // implenetation details

//////////////////////////////////////////////////////////////////////////////

void statement::check_error(int code) const
{
	return s_.check_error(code);
}
//----------------------------------------------------------------------------

void statement::check_last_error() const
{
	s_.check_error(s_.last_error());
}
//----------------------------------------------------------------------------

void statement::prepare()
{
	try
	{
		string_t const& sql = q_.sql();
		char_t const* tail;
		check_error( select(::sqlite3_prepare, ::sqlite3_prepare16)
			(s_.impl(), sql.c_str(), sql.size(), &impl_, &tail) );
		if ( tail && *tail )
		{
			throw exception(SQLITE_ERROR, "only one statement is supported");
		}

		// bind into and use binders
		std::accumulate(q_.intos().begin(), q_.intos().end(), 0, bind(*this));
		std::accumulate(q_.uses().begin(), q_.uses().end(), 1, bind(*this));
	}
	catch(std::exception const&)
	{
		// statement should stay not prepared
		finalize();
		throw;
	}
}
//----------------------------------------------------------------------------

bool statement::exec()
{
	if ( !prepared() )
	{
		prepare();
	}
	// If statement has result (select for ex.)
	// update into holders.
	// If statement done (insert, create table for ex.)
	// finalize it becase transaction object doesn't end itself
	// if there statements exist in sqlite database.	
	int r = sqlite3_step(impl_);
	switch ( r )
	{
	case SQLITE_ROW:
		std::for_each(q_.intos().begin(), q_.intos().end(), update(*this));
		return true;
	case SQLITE_DONE:
		finalize(); 
		return false;
	default:
		check_error(r);
		return false;
	}
}
//----------------------------------------------------------------------------

void statement::reset()
{
	if ( prepared() )
	{
		check_error( sqlite3_reset(impl_) );
	}
}
//----------------------------------------------------------------------------

void statement::finalize() // throw
{
	if ( prepared() )
	{
		sqlite3_finalize(impl_);
		impl_ = 0;
	}
}
//----------------------------------------------------------------------------

int statement::column_count() const
{
	int count = sqlite3_column_count(impl_);
	check_last_error();
	return count;
}
//----------------------------------------------------------------------------

string_t statement::column_name(int column) const
{
	char_t const* name = sqlite3_column_name(impl_, column);
	check_last_error();
	return name;
}
//----------------------------------------------------------------------------

int statement::column_index(string_t const& name) const
{
	for (int c = 0, cc = column_count(); c < cc; ++c)
	{
		if ( name == column_name(c) )
			return c;
	}
	throw exception(SQLITE_RANGE, string_t("no column ") + name);
}

statement::col_type statement::column_type(int column) const
{
	int type = sqlite3_column_type(impl_, column);
	check_last_error();
	return static_cast<col_type>(type);
}
//----------------------------------------------------------------------------

void statement::column_value(int column, int& value) const
{
	value = sqlite3_column_int(impl_, column);
	check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, long long& value) const
{
	value = sqlite3_column_int64(impl_, column);
	check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, double& value) const
{
	value = sqlite3_column_double(impl_, column);
	check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, string_t& value) const
{
	char_t const* str = reinterpret_cast<char const*>(sqlite3_column_text(impl_, column));
	check_last_error();
	value = str? str : "";
}
//----------------------------------------------------------------------------

void statement::column_value(int column, blob& value) const
{
	unsigned char const* data = static_cast<unsigned char const*>(sqlite3_column_blob(impl_, column));
	check_last_error();
	size_t size = sqlite3_column_bytes(impl_, column);
	check_last_error();
	value.assign(data, data + size);
}
//----------------------------------------------------------------------------

int statement::use_pos(string_t const& name) const
{
	int pos = sqlite3_bind_parameter_index(impl_, name.c_str());
	if ( pos <= 0 )
	{
		throw exception(SQLITE_RANGE, s_.last_error_msg());
	}
	return pos;
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, int value)
{
	check_error( sqlite3_bind_int(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, double value)
{
	check_error( sqlite3_bind_double(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, long long value)
{
	check_error( sqlite3_bind_int64(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, string_t const& value)
{
	check_error( sqlite3_bind_text(impl_, pos, 
		value.empty()? 0 : value.c_str(), value.size(), SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, blob const& value)
{
	check_error( sqlite3_bind_blob(impl_, pos, 
		value.empty()? 0 : &value[0], value.size(), SQLITE_STATIC) );
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
