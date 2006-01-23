//////////////////////////////////////////////////////////////////////////////
// session.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/session.hpp"

#include <sqlite3.h>

#include "sqlitepp/exception.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

void session::open(string_t const& file_name)
{
	// close session
	close();

	int const r = sqlite3_open(file_name.c_str(), &impl_);
	if ( r != SQLITE_OK )
	{
		string_t msg( last_error_msg() );
		close(); // session should be closed anyway
		throw exception(r, msg);
	}
}
//----------------------------------------------------------------------------

void session::close()
{
	if ( is_open() )
	{
		int const r = sqlite3_close(impl_);
		impl_ = 0;
		check_error(r);
	}
}
//----------------------------------------------------------------------------

int session::last_error() const // throw()
{
	return is_open() ? sqlite3_errcode(impl_) : SQLITE_OK;
}
//----------------------------------------------------------------------------

string_t session::last_error_msg() const // throw()
{
	string_t result;
	if ( is_open() )
	{
		result = select(::sqlite3_errmsg, ::sqlite3_errmsg16)(impl_);
	}
	return result;
}
//----------------------------------------------------------------------------

void session::check_error(int code) const
{
	if ( code != SQLITE_OK && code != SQLITE_ROW && code != SQLITE_DONE )
	{
		throw exception(code, last_error_msg());
	}
}
	 
//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
