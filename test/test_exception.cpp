#include <tut.h>

#include <sqlitepp/exception.hpp>

#include "session_data.hpp"

namespace
{

using namespace sqlitepp;

struct exception_data : session_data
{
};

typedef tut::test_group<exception_data> test_group;
typedef test_group::object object;

test_group g("exception");

template<>template<>
void object::test<1>()
{
	try
	{
		se << utf(L"qaz");
		fail( "exception expected");
	}
	catch(sqlitepp::exception const& ex)
	{
		ensure( "error code", ex.code() );
		ensure( "what", ex.what() );
	}
}

} // namespace
