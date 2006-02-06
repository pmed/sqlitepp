//////////////////////////////////////////////////////////////////////////////
// query.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/query.hpp"

#include <algorithm>
#include <stdexcept>

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

template<typename T>
inline void delete_object(T* obj)
{
	delete obj;
}
//----------------------------------------------------------------------------

template<typename T>
inline T* clone_object(T* obj)
{
	return obj->clone();
}

//////////////////////////////////////////////////////////////////////////////

} // namespace // implementation details
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// 
// query 
//

void query::assign(query const& src)
{
	clear();
	// clone binders
	std::transform(src.intos_.begin(), src.intos_.end(),
		std::back_inserter(intos_), clone_object<into_binder>);
	std::transform(src.uses_.begin(), src.uses_.end(),
		std::back_inserter(uses_), clone_object<use_binder>);
	// set sql
	set_sql(src.sql());
}

void query::set_sql(string_t const& sql)
{
	sql_.str(sql.c_str());
	sql_.seekp(0, std::ios_base::end).clear();
}

void query::clear() // throw()
{
	// clear binders
	std::for_each(intos_.begin(), intos_.end(), delete_object<into_binder>);
	intos_.clear();
	std::for_each(uses_.begin(), uses_.end(), delete_object<use_binder>);
	uses_.clear();
	// clear sql
	set_sql(string_t());
}
//----------------------------------------------------------------------------

query& query::put(into_binder_ptr i)
{
	if ( !i.get() )
	{
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
			throw session_not_open();
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
