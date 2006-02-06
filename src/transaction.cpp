//////////////////////////////////////////////////////////////////////////////
// transaction.cpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/transaction.hpp"

#include <sqlite3.h>

#include "sqlitepp/exception.hpp"
#include "sqlitepp/session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

transaction::transaction(session& s) : s_(s), do_rollback_(false)
{
	if ( s_.active_txn() )
	{
		throw nested_txn_not_supported();
	}
	s_ << "begin";
	s_.active_txn_ = this;
	do_rollback_ = true;
}
//----------------------------------------------------------------------------

transaction::~transaction() 
{
	if ( do_rollback_ )
	{
		rollback();
	}
	try
	{
		s_ << "end";
	}
	catch(std::exception const&)
	{
		// eat exception!!!
	}
	s_.active_txn_ = 0;
}
//----------------------------------------------------------------------------

void transaction::rollback() // throw()
{
	try
	{
		s_ << "rollback";
	}
	catch(std::exception const&)
	{
		// eat exception!!!
	}
}
//----------------------------------------------------------------------------

void transaction::commit()
{
	s_ << "commit";
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
