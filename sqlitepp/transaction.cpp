//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>

#include "transaction.hpp"
#include "exception.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

transaction::transaction(session& s, type t)
	: s_(s)
	, do_rollback_(false)
{
	if ( s_.active_txn() )
	{
		throw nested_txn_not_supported();
	}

	char const* begin_cmd;
	switch ( t )
	{
	case deferred:
		begin_cmd = "begin deferred";
		break;
	case immediate:
		begin_cmd = "begin immediate";
		break;
	case exclusive:
		begin_cmd = "begin exclusive";
		break;
	default:
		assert(false && "unknown transaction type");
		begin_cmd = "begin";
		break;
	}

	s_ << utf(begin_cmd);
	s_.active_txn_ = this;
	do_rollback_ = true;
}
//----------------------------------------------------------------------------

transaction::~transaction()
{
	if ( do_rollback_ )
	{
		s_ << utf(L"rollback");
	}
	s_.active_txn_ = nullptr;
}
//----------------------------------------------------------------------------

void transaction::commit()
{
	s_ << utf(L"commit");
	do_rollback_ = false;
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
