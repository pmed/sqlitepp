//////////////////////////////////////////////////////////////////////////////
// statement.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STATEMENT_HPP_INCLUDED
#define SQLITEPP_STATEMENT_HPP_INCLUDED

#include <vector>

#include "string.hpp"
#include "query.hpp" 
#include "binders.hpp"

struct sqlite3_stmt;

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

class session;

typedef std::vector<unsigned char> blob;

// Database statement, noncopyable
class statement
{
public:
	// Create empty statement
	explicit statement(session& s)
		: s_(s)
		, impl_(0)
	{
	}
	
	// Create statement with SQL query.
	explicit statement(session& s, string_t const& sql)
		: s_(s)
		, q_(sql)
		, impl_(0)
	{
	}

	// Destroy statement.
	~statement()
	{
		finalize();
	}
	
	// Bool-like type for implicit casting.
	typedef bool (statement::*unspecified_bool_type)() const;

	// Implicit cast into bool-like type. Return prepared()
	operator unspecified_bool_type() const // throw()
	{
		return prepared() ? &statement::prepared : 0;
	}

	// Start statement preparing.
	template<typename T>
	prepare_query operator<<(T const& t)
	{
		prepare_query pq(this);
		pq << t;
		return pq;
	}

	// Statement query.
	query const& q() const // throw()
	{
		return q_;
	}

	// Statement query.
	query& q() // throw()
	{
		return q_;
	}

	// Set new query
	void set_q(query const& q)
	{
		q_ = q;
	}

	// Execute statement. Return true if result exists.
	bool exec();

	// Prepare statement.
	void prepare();
	// Finalize statement.
	void finalize(); // throw()
	// Is statement prepared.
	bool prepared() const throw() { return impl_ != 0; }
	// Reset statement. Return to prepared state.
	void reset();

	// Number of columns in result set of prepared statement.
	int column_count() const;
	// Column name in result set.
	string_t column_name(int column) const;
	// Column index in result set.
	int column_index(string_t const& name) const;

	// Colmn type of result set in prepared statement.
	enum col_type { COL_INT = 1, COL_FLOAT = 2, COL_TEXT = 3, COL_BLOB = 4, COL_NULL = 5 };
	// Column type in result set.
	col_type column_type(int column) const;

	// Column value as int.
	void column_value(int column, int& value) const;
	// Column value as 64-bit int.
	void column_value(int column, long long& value) const;
	// Column value as double.
	void column_value(int column, double& value) const;
	// Column value as string.
	void column_value(int column, string_t& value) const;
	// Column value as BLOB.
	void column_value(int column, blob& value) const;

	template<typename T>
	T column_as(int column) const
	{
		T t;
		column_value(column, t);
		return t;
	}
	
	// Use int value in query.
	void use_value(int pos, int value);
	// Use 64-bit int value in query.
	void use_value(int pos, long long value);
	// Use double value in query.
	void use_value(int pos, double value);
	// Use string value in query.
	void use_value(int pos, string_t const& value);
	// Use BLOB value in query.
	void use_value(int pos, blob const& value);

	// Get use position by name in query.
	int use_pos(string_t const& name) const;

	sqlite3_stmt* impl() const // throw()
	{
		return impl_;
	}
private:
	// Copy not allowed.
	statement(statement const&);
	// Assignment not allowed.
	statement& operator=(statement const&);

	// Check error code. If code is not ok, throws exception.
	void check_error(int code) const;
	void check_last_error() const;

	session& s_;
	query q_;
	sqlite3_stmt* impl_;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STATEMENT_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
