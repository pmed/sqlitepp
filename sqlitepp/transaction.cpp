//////////////////////////////////////////////////////////////////////////////
// $Id$
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sqlite3.h>

#include "transaction.hpp"
#include "exception.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

transaction::transaction(session& s) : s_(s), do_rollback_(false)
{
	if ( s_.active_txn() )
	{
		throw nested_txn_not_supported();
	}
	s_ << utf(L"begin");
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
		s_ << utf(L"end");
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
		s_ << utf(L"rollback");
	}
	catch(std::exception const&)
	{
		// eat exception!!!
	}
}
//----------------------------------------------------------------------------

void transaction::commit()
{
	s_ << utf(L"commit");
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
