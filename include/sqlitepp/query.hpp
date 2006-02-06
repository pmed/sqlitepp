//////////////////////////////////////////////////////////////////////////////
// sql_query.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_PROXIES_HPP_INCLUDED
#define SQLITEPP_PROXIES_HPP_INCLUDED

#include <vector>
#include <sstream>

#include "string.hpp"
#include "binders.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

// SQL sql_query base class.
class query
{
public:
	// Create empty sql_query.
	query()
	{
	}
	
	// Create sql_query with SQL statement.
	explicit query(string_t const& sql)
	{
		sql_ << sql;
	}
	
	query(query const& src)
	{
		assign(src);
	}
	
	query& operator=(query const& src)
	{
		assign(src);
		return *this;
	}

	// Clear sql_query on destroy.
	~query()
	{
		clear();
	}

	void assign(query const& src);

	// Current SQL statement.
	string_t sql() const // throw()
	{
		return sql_.str();
	}
	
	// Set new SQL statement.
	void set_sql(string_t const& sql);

	// Clear sql_query sql, into and use bindings.
	void clear(); // throw()
	
	// Is query empty?
	bool empty() const // throw()
	{
		return sql_.str().empty() && intos_.empty() && uses_.empty();
	}

	// Into binders container type.
	typedef std::vector<into_binder*> into_binders;
	
	// Into binders.
	into_binders const& intos() const // throw()
	{
		return intos_;
	}

	// Use binders container type.
	typedef std::vector<use_binder*> use_binders;

	// Use binders.
	use_binders const& uses() const // throw()
	{
		return uses_;
	}

	query& operator<<(utf8_string const& s)
	{
		sql_ << utf(s);
		return *this;
	}

	query& operator<<(utf16_string const& s)
	{
		sql_ << utf(s);
		return *this;
	}

	// Collect SQL.
	template<typename T>
	query& operator<<(T const& t)
	{
		sql_ << t;
		return *this;
	}

	// Add into binder.
	query& put(into_binder_ptr i);

	// Add into binder.
	query& operator,(into_binder_ptr i)
	{
		return put(i);
	}
	
	// Add use binder.
	query& put(use_binder_ptr i);
	
	// Add use binder.
	query& operator,(use_binder_ptr u)
	{
		return put(u);
	}

	// Swap queries.
	friend void swap(query& lhs, query& rhs);
private:	
	into_binders intos_;
	use_binders  uses_;

	std::basic_ostringstream<char_t> sql_;
};

// Statement preparing proxy.
class prepare_query : public query
{
	friend class statement;
public:
	// Destructive copy.
	prepare_query(prepare_query& src)
	{
		swap(*this, src);
		st_ = src.st_; src.st_ = 0;
	}

	// Set statement sql_query on destroy.
	~prepare_query();
private:
	// Create preparing proxy for statement.
	prepare_query(statement* st)
		: st_(st)
	{
	}
	
	// Assignment not allowed.
	prepare_query& operator=(prepare_query const&);

	statement* st_;
};

// Immediatly executed sql_query.
class once_query : public query
{
	friend class session;
public:
	// Destructive copy.
	once_query(once_query& src)
	{
		swap(*this, src);
		s_ = src.s_; src.s_ = 0;
	}

	// Execute sql_query on destroy.
	~once_query();
private:
	// Create proxy for session.
	once_query(session* s)
		: s_(s)
	{
	}
	
	// Assignment not allowed.
	once_query& operator=(once_query const&); 

	session* s_;
};

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_PROXIES_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
