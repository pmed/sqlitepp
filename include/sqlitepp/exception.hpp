//////////////////////////////////////////////////////////////////////////////
// exception.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_EXCEPTION_HPP_INCLUDED
#define SQLITEPP_EXCEPTION_HPP_INCLUDED

#include <exception>

#include "string.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// SQLite exception
class exception : public std::exception
{
public:
	// create exception from error code and description
	explicit exception(int err_code, string_t const& descr = string_t())
		: code_(err_code), descr_(descr)
	{
	}

	~exception() throw()
	{
	}

	// Exception error description
	char const* what() const throw();
	// SQLite library error code.
	int code() const // throw()
	{
		return code_;
	}

	// SQLite error description
	string_t const& descr() const //throw()
	{
		return descr_;
	}
private:	
	int code_;
	string_t descr_;
};

struct nested_txn_not_supported : exception
{
	nested_txn_not_supported();
	char const* what() const throw();
};

struct no_such_column : exception
{
	no_such_column(string_t const& col_name);
	char const* what() const throw();
};

struct session_not_open : exception
{
	session_not_open();
	char const* what() const throw();
};

struct multi_stmt_not_supported : exception
{
	multi_stmt_not_supported();
	char const* what() const throw();
};

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_EXCEPTION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
