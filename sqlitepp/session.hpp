//////////////////////////////////////////////////////////////////////////////
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
	friend class transaction; // access to active_txn_
	friend class statement;   // access to last_exec_
public:
	// Create a session.
	session();
	
	// Create and open session.
	// Optional parameter flags for file open operations
	// (see SQLite reference at http://sqlite.org/c3ref/c_open_autoproxy.html)
	explicit session(string_t const& file_name, int flags = 0);
	
	// Close session on destroy.
	~session();

	// Open database session. Previous one will be closed.
	// Optional parameter flags for file open operations
	// (see SQLite reference at http://sqlite.org/c3ref/c_open_autoproxy.html)
	void open(string_t const& file_name, int flags = 0);
	
	// Close database session.
	void close();

	// Is session opened?
	bool is_open() const // throw()
	{
		return impl_ != nullptr;
	}

	// Is there an active transaction?
	// Currently SQLite 3 doesn't support nested transactions.
	// So we can test, is there any transaction in session.
	// If we have the transaction, we get it or null otherwise.
	transaction* active_txn() const // throw()
	{
		return active_txn_;
	}

	/// SQLite implementation for native sqlite3 functions.
	sqlite3* impl() const { return impl_; }

	/// Check error code. If code is not ok, throws exception.
	void check_error(int code) const;
	void check_last_error() const { check_error(last_error()); }

	// Last session error
	int last_error() const;

	// Last statement::exec result
	bool last_exec() const { return last_exec_; }

	// Last insert row ID
	long long last_insert_rowid() const;
	
	// The number of rows that were changed (or inserted or deleted)
	// by the most recent SQL statement
	size_t last_changes() const;

	// The number of rows that were changed (or inserted or deleted)
	// since the database was opened
	size_t total_changes() const;

	// Execute SQL query immediately.
	// It might be useful for resultless statements like INSERT, UPDATE etc.
	// T is any output-stream-shiftable type.
	template<typename T>
	once_query operator<<(T const& t)
	{
		once_query q(*this);
		q << t;
		return q;
	}

	// Swap session instances
	friend void swap(session& lhs, session& rhs)
	{
		std::swap(lhs.impl_, rhs.impl_);
		std::swap(lhs.active_txn_, rhs.active_txn_);
	}
private:
	// Noncopyable.
	session(session const&);
	// Nonassignable.
	session& operator=(session const&);

	sqlite3* impl_;
	transaction* active_txn_;
	bool last_exec_;
};

//////////////////////////////////////////////////////////////////////////////

} // end of namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_SESSION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
