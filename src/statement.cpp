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

// Update into_binder functor
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

// Do bindings to statement functor
struct bind
{
	bind(statement& st) : st_(st) {}
	bind& operator=(bind const&);

	template<typename T>
	int operator()(int pos, T* binder) const
	{
		return binder->bind(st_, pos);
	}

	statement& st_;
};
//----------------------------------------------------------------------------

// Need to overload bind_text functions because of char const* UTF-8 version :(
int do_bind_text(sqlite3_stmt* st, int pos, utf8_string const& str)
{
	return ::sqlite3_bind_text(st, pos, reinterpret_cast<char const*>(str.empty()? 0 : str.data()),
		static_cast<int>(str.size() * sizeof(utf8_char)), SQLITE_STATIC);
}
//----------------------------------------------------------------------------

int do_bind_text(sqlite3_stmt* st, int pos, utf16_string const& str)
{
	return ::sqlite3_bind_text16(st, pos, str.empty()? 0 : str.data(),
		static_cast<int>(str.size() * sizeof(utf16_char)), SQLITE_STATIC);
}
//----------------------------------------------------------------------------

// sqlite3_bind_parameter_index exists only for UTF-8 - do overloading again :(
int bind_index(sqlite3_stmt* st, utf8_string const& name)
{
	return sqlite3_bind_parameter_index(st, reinterpret_cast<char const*>(name.c_str()));
}
//----------------------------------------------------------------------------

int bind_index(sqlite3_stmt* st, utf16_string const& name)
{
	return bind_index(st, utf8(name));
}

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

void statement::do_prepare(utf8_string const& sql)
{
	char const* tail;
		
	check_error( ::sqlite3_prepare(s_.impl(),
		reinterpret_cast<char const*>(sql.c_str()),
		static_cast<int>(sql.size() * sizeof(utf8_char)), 
		&impl_, &tail) );
	if ( tail && *tail )
	{
		throw multi_stmt_not_supported();
	}
}
//----------------------------------------------------------------------------

void statement::do_prepare(utf16_string const& sql)
{
	utf16_char const* tail;
		
	check_error( ::sqlite3_prepare16(s_.impl(), sql.c_str(),
		static_cast<int>(sql.size() * sizeof(utf16_char)), 
		&impl_, reinterpret_cast<void const**>(&tail)) );
	if ( tail && *tail )
	{
		throw multi_stmt_not_supported();
	}
}
//----------------------------------------------------------------------------

void statement::prepare()
{
	try
	{
		do_prepare(q_.sql());
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
	char_t const* name = reinterpret_cast<char_t const*>
		(aux::select(::sqlite3_column_name, ::sqlite3_column_name16)(impl_, column));
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
	throw no_such_column(name);
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
	char_t const* str = reinterpret_cast<char_t const*>(
		aux::select(::sqlite3_column_text, ::sqlite3_column_text16)(impl_, column));
	check_last_error();
	if ( str )
	{
		value = str;
	}
	else
	{
		value.clear();
	}
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
	int pos = bind_index(impl_, name);
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
	// call do_* helper - select for utf8_char not applicable for sqlite3_bind_text :(
	check_error( do_bind_text(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, blob const& value)
{
	check_error( sqlite3_bind_blob(impl_, pos, 
		value.empty()? 0 : &value[0], 
		static_cast<int>(value.size()), SQLITE_STATIC) );
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
