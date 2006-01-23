//////////////////////////////////////////////////////////////////////////////
// query.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/query.hpp"

#include <cassert>
#include <algorithm>

#include <sqlite3.h>

#include "sqlitepp/exception.hpp"
#include "sqlitepp/statement.hpp"
#include "sqlitepp/session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

namespace { // implementation details

//////////////////////////////////////////////////////////////////////////////

inline void delete_binder(binder* b)
{
	delete b;
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace // implementation details
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// 
// query 
//

void query::set_sql(string_t const& sql)
{
	sql_.str(sql.c_str());
	sql_.seekp(0, std::ios_base::end).clear();
}

void query::clear() // throw()
{
	// clear binders
	std::for_each(intos_.begin(), intos_.end(), delete_binder);
	intos_.clear();
	std::for_each(uses_.begin(), uses_.end(), delete_binder);
	uses_.clear();
	// clear sql
	sql_.clear();
}
//----------------------------------------------------------------------------

query& query::put(into_binder_ptr i)
{
	if ( !i.get() )
	{
		assert(!"null into binder");
		throw std::invalid_argument("null into binder");
	}
	intos_.push_back(i.release());
	return *this;
}
//----------------------------------------------------------------------------

query& query::put(use_binder_ptr u)
{
	if ( !u.get() )
	{
		assert(!"null use binder");
		throw std::invalid_argument("null use binder");
	}
	uses_.push_back(u.release());
	return *this;
}
//----------------------------------------------------------------------------

void swap(query& q1, query& q2)
{
	// swap binders
	swap(q1.intos_, q2.intos_);
	swap(q1.uses_, q2.uses_);
	// swap sql streams
	string_t s1(q1.sql_.str().c_str());
	string_t s2(q2.sql_.str().c_str());
	q1.sql_.str(s2.c_str());
	q1.sql_.seekp(0, std::ios_base::end).clear();
	q2.sql_.str(s1.c_str());
	q2.sql_.seekp(0, std::ios_base::end).clear();
}
//----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
// 
// prepare_query 
//
prepare_query::~prepare_query()
{
	if ( st_ )
	{
		// move query to statement.
		swap(st_->q(), *this); 
	}
}

//////////////////////////////////////////////////////////////////////////////
// 
// once_query
//
once_query::~once_query()
{
	if ( s_ )
	{
		if ( !s_->is_open() )
		{
			assert(!"session not open");
			throw exception(SQLITE_ERROR, "session not open");
		}
		// execute statement in session.
		statement st(*s_);
		swap(st.q(), *this);
		st.exec();
	}
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
