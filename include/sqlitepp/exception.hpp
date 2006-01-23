//////////////////////////////////////////////////////////////////////////////
// exception.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_EXCEPTION_HPP_INCLUDED
#define SQLITEPP_EXCEPTION_HPP_INCLUDED

#include "string.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// SQLite exception
class exception
{
public:
	explicit exception(int err_code, string_t const& descr)
		: code_(err_code), descr_(descr)
	{
	}

	~exception() throw() {}

	char_t const* what() const throw() { return descr_.c_str(); }

	// SQLite library error code.
	int code() const // throw()
	{
		return code_;
	}
private:	
	int code_;
	string_t descr_;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_EXCEPTION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
