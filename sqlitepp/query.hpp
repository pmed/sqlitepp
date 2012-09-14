//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_PROXIES_HPP_INCLUDED
#define SQLITEPP_PROXIES_HPP_INCLUDED

#include <vector>
#include <sstream>
#include <memory>

#include "string.hpp"
#include "binders.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class session;
class statement;

// SQL query base class. Noncopyable.
class query
{
	friend class statement; // access to intos_ and uses_
public:
	// Create an empty query.
	query() {}
	// Create a query with SQL text.
	explicit query(string_t const& sql) { sql_ << sql; }

	query(query&& src);
	query& operator=(query&& src);

	// Clear query on destroy.
	~query() { clear(); }

	// Current SQL statement.
	string_t sql() const // throw()
	{
		return sql_.str();
	}
	
	// Set new SQL statement.
	void sql(string_t const& text);

	// Clear sql text, into and use bindings.
	void clear(); // throw()
	
	// Is query empty?
	bool empty() const; // throw()

	// Collect SQL text.
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
		return put(std::move(i));
	}
	
	// Add use binder.
	query& put(use_binder_ptr i);
	
	// Add use binder.
	query& operator,(use_binder_ptr u)
	{
		return put(std::move(u));
	}

private:
	query(query const& src); // = delete
	query& operator=(query const& src); // = delete

	std::vector<into_binder_ptr> intos_;
	std::vector<use_binder_ptr> uses_;

	std::basic_ostringstream<char_t> sql_;
};

// Statement preparing proxy.
class prepare_query : public query
{
	friend class statement; // access to ctor
public:
	// Transfer execution responsibility from src to this object.
	prepare_query(prepare_query&& src);

	// Move query to statement on destroy.
	~prepare_query();

private:
	// Create preparing proxy for statement.
	prepare_query(statement& st);

	prepare_query(prepare_query const& src); // = delete
	prepare_query& operator=(prepare_query const& src); // = delete

	statement* st_;
};

// Immediatly executed query proxy.
class once_query : public query
{
	friend class session; // access to ctor
public:
	// Transfer execution responsibiblty from src to this object.
	once_query(once_query&& src);

	// Execute statement on destroy.
	~once_query();

private:
	// Create proxy for session.
	once_query(session& s);

	once_query(once_query const& src); // = delete;
	once_query& operator=(once_query const& src); // = delete;

	session* s_;
};

//////////////////////////////////////////////////////////////////////////////

} //namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_PROXIES_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
