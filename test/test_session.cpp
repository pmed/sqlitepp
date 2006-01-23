#include <stdio.h>
#include <tchar.h>
#include <tut.h>

#include "session_data.hpp"
#include <sqlitepp/exception.hpp>

session_data::session_data(std::string const& name) : name_(name)
{
	// ensure remove previously used database
	remove(name_.c_str()); 
}

session_data::~session_data()
{
	se.close();
	// ensure remove database
	remove(name_.c_str()); 
}

namespace 
{
	
typedef tut::test_group<session_data> test_group;
typedef test_group::object object;

test_group g("session");

// test open session
template<>template<>
void object::test<1>()
{
	se.open(name_);
	ensure( "open", se.is_open() );
	ensure( "valid", se);
	ensure( "impl", se.impl());
	ensure( "no active txn", !se.active_txn() );
}

// test close session
template<>template<>
void object::test<2>()
{
	ensure("closed", !se.is_open());
	se.open(name_);
	ensure( "open", se.is_open() );
	ensure( "valid", se);
	ensure( "no active txn", !se.active_txn() );

	se.close();
	ensure("closed", !se.is_open());
}

// test last error
template<>template<>
void object::test<3>()
{
	se.open(name_);
	ensure( "open", se.is_open() );
	try
	{
		se << "select * from undefined_table";
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure( "session error", se.last_error() );
		ensure( "session error msg", se.last_error_msg() );
		ensure( "session not valid", !se );
	}
}

} // namespace
