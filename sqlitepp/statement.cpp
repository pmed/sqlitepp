//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <numeric>
#include <type_traits>

#include "../sqlite/sqlite3.h"

#include "statement.hpp"
#include "exception.hpp"
#include "binders.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

statement::statement(session& s)
	: s_(s)
	, impl_(nullptr)
{
}
//----------------------------------------------------------------------------

statement::statement(session& s, string_t const& sql)
	: s_(s)
	, q_(sql)
	, impl_(nullptr)
{
}
//----------------------------------------------------------------------------

statement::~statement()
{
	finalize(false);
}
//----------------------------------------------------------------------------

void statement::prepare()
{
	try
	{
		typedef std::conditional<sizeof(char_t) == sizeof(utf8_char), char const*, void const*>::type tail_type;
		char_t const* tail = nullptr;
		string_t const sql = q_.sql();
		s_.check_error(
			aux::select(::sqlite3_prepare, ::sqlite3_prepare16)(s_.impl(), sql.c_str(),
				static_cast<int>(sql.size() * sizeof(char_t)),
				&impl_, reinterpret_cast<tail_type*>(&tail)) );
		if ( tail && *tail )
		{
			throw multi_stmt_not_supported();
		}

		// bind into binders
		std::accumulate(q_.intos_.begin(), q_.intos_.end(), 0,
			[this](int pos, into_binder_ptr& i) { return i->bind(*this, pos); });

		// bind use binders
		std::accumulate(q_.uses_.begin(), q_.uses_.end(), 1,
			[this](int pos, use_binder_ptr& u) { return u->bind(*this, pos); });
	}
	catch (std::exception const&)
	{
		// statement stays not prepared
		finalize(false);
		throw;
	}
}
//----------------------------------------------------------------------------

bool statement::exec()
{
	if ( !is_prepared() )
	{
		prepare();
	}
	try
	{
		int const r = ::sqlite3_step(impl_);
		switch ( r )
		{
		case SQLITE_ROW:
			// statement has result (select for ex.) - update into holders
			std::for_each(q_.intos_.begin(), q_.intos_.end(),
				[this](into_binder_ptr& i) { i->update(*this); });
			return s_.last_exec_ = true;
		case SQLITE_DONE:
			// reset statement to be ready for the next exec
			s_.check_error( ::sqlite3_reset(impl_) );
			return s_.last_exec_ = false;
		default:
			s_.check_error(r);
			// should never return this
			return false;
		}
	}
	catch (std::exception const&)
	{
		finalize(false);
		throw;
	}
}
//----------------------------------------------------------------------------

void statement::reset(bool rebind)
{
	if ( is_prepared() )
	{
		s_.check_error( ::sqlite3_reset(impl_) );
		if ( rebind )
		{
			std::accumulate(q_.uses_.begin(), q_.uses_.end(), 1,
				[this](int pos, use_binder_ptr& u) { return u->bind(*this, pos); });
		}
	}
}
//----------------------------------------------------------------------------

void statement::finalize(bool check_error) // throw
{
	if ( is_prepared() )
	{
		int const r = ::sqlite3_finalize(impl_);
		impl_ = nullptr;
		if ( check_error )
		{
			s_.check_error(r);
		}
	}
}
//----------------------------------------------------------------------------

int statement::column_count() const
{
	int count = ::sqlite3_column_count(impl_);
	s_.check_last_error();
	return count;
}
//----------------------------------------------------------------------------

string_t statement::column_name(int column) const
{
	char_t const* name = reinterpret_cast<char_t const*>
		(aux::select(::sqlite3_column_name, ::sqlite3_column_name16)(impl_, column));
	s_.check_last_error();
	return name;
}
//----------------------------------------------------------------------------

int statement::column_index(string_t const& name) const
{
	for (int c = 0, cc = column_count(); c < cc; ++c)
	{
		if ( name == column_name(c) )
		{
			return c;
		}
	}
	throw no_such_column(name);
}

statement::col_type statement::column_type(int column) const
{
	int const type = ::sqlite3_column_type(impl_, column);
	s_.check_last_error();
	return static_cast<col_type>(type);
}
//----------------------------------------------------------------------------

void statement::column_value(int column, int& value) const
{
	value = ::sqlite3_column_int(impl_, column);
	s_.check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, long long& value) const
{
	value = ::sqlite3_column_int64(impl_, column);
	s_.check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, double& value) const
{
	value = ::sqlite3_column_double(impl_, column);
	s_.check_last_error();
}
//----------------------------------------------------------------------------

void statement::column_value(int column, string_t& value) const
{
	char_t const* str = reinterpret_cast<char_t const*>(
		aux::select(::sqlite3_column_text, ::sqlite3_column_text16)(impl_, column));
	s_.check_last_error();
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
	value.data = ::sqlite3_column_blob(impl_, column);
	s_.check_last_error();

	value.size = sqlite3_column_bytes(impl_, column);
	s_.check_last_error();
}
//----------------------------------------------------------------------------

int statement::use_pos(string_t const& name) const
{
	int pos = ::sqlite3_bind_parameter_index(impl_, utf8(name).c_str());
	if ( pos <= 0 )
	{
		throw no_such_column(name);
	}
	return pos;
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, int value)
{
	s_.check_error( ::sqlite3_bind_int(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, double value)
{
	s_.check_error( ::sqlite3_bind_double(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, long long value)
{
	s_.check_error( ::sqlite3_bind_int64(impl_, pos, value) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, utf8_char const* value)
{
	s_.check_error( ::sqlite3_bind_text(impl_, pos, value, -1, SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, utf16_char const* value)
{
	s_.check_error( ::sqlite3_bind_text16(impl_, pos, value, -1, SQLITE_STATIC) );
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, string_t const& value, bool make_copy)
{
	s_.check_error( aux::select(::sqlite3_bind_text, ::sqlite3_bind_text16)
		(impl_, pos, value.empty()? nullptr : value.c_str(),
		static_cast<int>(value.size() * sizeof(char_t)), make_copy? SQLITE_TRANSIENT : SQLITE_STATIC)
	);
}
//----------------------------------------------------------------------------

void statement::use_value(int pos, blob const& value, bool make_copy)
{
	s_.check_error( ::sqlite3_bind_blob(impl_, pos, value.data,
		static_cast<int>(value.size), make_copy? SQLITE_TRANSIENT : SQLITE_STATIC) );
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
