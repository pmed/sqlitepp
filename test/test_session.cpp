#include <stdio.h>
#include <tchar.h>
#include <tut.h>

#include "session_data.hpp"
#include <sqlitepp/exception.hpp>

using namespace sqlitepp;

session_data::session_data(sqlitepp::string_t const& name) : name_(name)
{
	se.open(name_);
	// ensure remove previously used database
//	remove(name_.c_str()); 
}

session_data::~session_data()
{
	se.close();
	// ensure remove database
//	remove(name_.c_str()); 
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
	ensure( "open", se.is_open() );
	ensure( "valid", se);
	ensure( "impl", se.impl());
	ensure( "no active txn", !se.active_txn() );
}

// test close session
template<>template<>
void object::test<2>()
{
	se.close();
	ensure("closed", !se.is_open());
}

// test last error
template<>template<>
void object::test<3>()
{
	try
	{
		se << utf(L"select * from undefined_table");
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure( "session error", se.last_error() );
		ensure( "session error msg", !se.last_error_msg().empty() );
		ensure( "session not valid", !se );
	}
}

} // namespace
