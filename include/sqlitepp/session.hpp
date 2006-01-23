//////////////////////////////////////////////////////////////////////////////
// session.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_SESSION_HPP_INCLUDED
#define SQLITEPP_SESSION_HPP_INCLUDED

#include "string.hpp"
#include "query.hpp"

struct sqlite3;

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class transaction;

// Database session. Noncopyable.
class session
{
	friend class transaction; // for access to active_txn_ and impl_
//	friend class statement;   // access to check_error()
public:
	// Create an empty session.
	session() : impl_(0), active_txn_(0) {}
	
	// Create and open session.
	session(string_t const& file_name) : impl_(0), active_txn_(0)
	{
		open(file_name);
	}
	
	// Close session on destroy.
	~session()
	{
		try
		{
			close();
		}
		catch(...)
		{
		}
	}

	// Open database session. Previous one will be closed.
	void open(string_t const& file_name);
	
	// Close database session.
	void close();

	// Is session opened?
	bool is_open() const // throw()
	{
		return impl_ != 0;
	}
	
	// Last sqlite error code.
	// If session is not opended returns SQLITE_OK.
	// For more inforamtion see SQLite error codes.
	int last_error() const; // throw()
	
	// Last sqlite error message.
	// If session is not opended returns null.
	string_t last_error_msg() const; // throw()

	// Is there an active transaction?
	// Currently SQLite 3 doesn't support nested transactions.
	// So we can test, is there any transaction in session.
	// If we have the transaction, we get it or null otherwise.
	transaction* active_txn() const // throw()
	{
		return active_txn_; 
	}

	// Bool-like type for implicit casting.
	typedef bool (session::*unspecified_bool_type)() const;

	// Implicit cast into bool-like type. Return !last_error()
	operator unspecified_bool_type() const // throw()
	{
		return last_error() ? 0 : &session::is_open;
	}

	/// Check error code. If code is not ok, throws exception.
	void check_error(int code) const;

	// Execute SQL query immediately.
	// It might be useful for resultless statements like INSERT, UPDATE etc.
	// T is any output-stream-shiftable type.
	template<typename T>
	once_query operator<<(T const& t)
	{
		once_query q(this);
		q << t;
		return q;
	}

	// Get SQLite handle.
	sqlite3* impl() const // throw()
	{
		return impl_;
	}
private:
	// Noncopyable.
	session(session const&);
	// Nonassignable.
	session& operator=(session const&);

	sqlite3* impl_;
	transaction* active_txn_;
};

//////////////////////////////////////////////////////////////////////////////

} // end of namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_SESSION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
