//////////////////////////////////////////////////////////////////////////////
// exception.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "sqlitepp/exception.hpp"

#include <sqlite3.h>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

char const* exception::what() const throw()
{
	switch ( code_ )
	{
		case SQLITE_OK:	        return "not an error";
		case SQLITE_ERROR:      return "SQL logic error or missing database";
		case SQLITE_INTERNAL:   return "internal SQLite implementation flaw";
		case SQLITE_PERM:       return "access permission denied";
		case SQLITE_ABORT:      return "callback requested query abort";
		case SQLITE_BUSY:       return "database is locked";
		case SQLITE_LOCKED:     return "database table is locked";
		case SQLITE_NOMEM:      return "out of memory";
		case SQLITE_READONLY:   return "attempt to write a readonly database";
		case SQLITE_INTERRUPT:  return "interrupted";
		case SQLITE_IOERR:      return "disk I/O error";
		case SQLITE_CORRUPT:    return "database disk image is malformed";
		case SQLITE_NOTFOUND:   return "table or record not found";
		case SQLITE_FULL:       return "database is full";
		case SQLITE_CANTOPEN:   return "unable to open database file";
		case SQLITE_PROTOCOL:   return "database locking protocol failure";
		case SQLITE_EMPTY:      return "table contains no data";
		case SQLITE_SCHEMA:     return "database schema has changed";
		case SQLITE_TOOBIG:     return "too much data for one table row";
		case SQLITE_CONSTRAINT: return "constraint failed";
		case SQLITE_MISMATCH:   return "datatype mismatch";
		case SQLITE_MISUSE:     return "library routine called out of sequence";
		case SQLITE_NOLFS:      return "kernel lacks large file support";
		case SQLITE_AUTH:       return "authorization denied";
		case SQLITE_FORMAT:     return "auxiliary database format error";
		case SQLITE_RANGE:      return "bind index out of range";
		case SQLITE_NOTADB:     return "file is encrypted or is not a database";
		default:                return "unknown error";
	}
}

nested_txn_not_supported::nested_txn_not_supported()
	: exception(-1, ansi_to_utf(what()))
{
}

char const* nested_txn_not_supported::what() const throw()
{
	return "nested transactions are not supported";
}

no_such_column::no_such_column(string_t const& col_name)
	: exception(-2, ansi_to_utf(what()) + col_name)
{
}

char const* no_such_column::what() const throw()
{
	return "no such column";
}

session_not_open::session_not_open()
	: exception(-3, ansi_to_utf(what()))
{
}

char const* session_not_open::what() const throw()
{
	return "session not open";
}

multi_stmt_not_supported::multi_stmt_not_supported()
	: exception(-4, ansi_to_utf(what()))
{
}

char const* multi_stmt_not_supported::what() const throw()
{
	return "only one statement is supported";
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
