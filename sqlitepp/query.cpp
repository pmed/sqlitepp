//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "query.hpp"
#include "exception.hpp"
#include "statement.hpp"
#include "session.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////
//
// query
//

query::query(query&& src)
{
	*this = std::move(src);
}

query& query::operator=(query&& src)
{
	if ( this != &src )
	{
		intos_ = std::move(src.intos_);
		uses_ = std::move(src.uses_);
		swap(sql_, src.sql_);
		sql_.clear();
		src.sql_.clear();
	}
	return *this;
}

void query::sql(string_t const& text)
{
	sql_.str(text);
	sql_.seekp(0, std::ios_base::end).clear();
}
//----------------------------------------------------------------------------

void query::clear() // throw()
{
	intos_.clear();
	uses_.clear();
	sql(string_t());
}
//----------------------------------------------------------------------------

bool query::empty() const // throw()
{
	return sql_.str().empty() && intos_.empty() && uses_.empty();
}
//----------------------------------------------------------------------------

query& query::put(into_binder_ptr i)
{
	if ( !i )
	{
		throw std::invalid_argument("null into binder");
	}
	intos_.emplace_back(std::move(i));
	return *this;
}
//----------------------------------------------------------------------------

query& query::put(use_binder_ptr u)
{
	if ( !u )
	{
		throw std::invalid_argument("null use binder");
	}
	uses_.emplace_back(std::move(u));
	return *this;
}
//----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
//
// prepare_query
//
prepare_query::prepare_query(statement& st)
	: st_(&st)
{
}

prepare_query::prepare_query(prepare_query&& src)
	: query(std::move(src))
	, st_(src.st_)
{
	src.st_ = nullptr;
}

prepare_query::~prepare_query()
{
	if ( st_ )
	{
		st_->q() = std::move(*this);
		st_->finalize();
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// once_query
//

once_query::once_query(session& s)
	: s_(&s)
{
}
//----------------------------------------------------------------------------

once_query::once_query(once_query&& src)
	: query(std::move(src))
	, s_(src.s_)
{
	src.s_ = nullptr;
}
//----------------------------------------------------------------------------

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
		st.q() = std::move(*this);
		st.exec();
	}
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
