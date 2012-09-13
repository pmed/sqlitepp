#include <tut.h>

#include <sqlitepp/exception.hpp>

#include "statement_data.hpp"

using namespace sqlitepp;

namespace tut {

struct exception_data : statement_data
{
	exception ex;
	exception_data() : ex(2343, utf("exception message")) {}
};

typedef test_group<exception_data> ex_test_group;
typedef ex_test_group::object object;

ex_test_group ex_g("2. exception");

template<>template<>
void object::test<1>()
{
	ensure_equals("code", ex.code(), 2343);
	ensure_equals("what", ex.what(), utf8("exception message"));
}

template<>template<>
void object::test<2>()
{
	exception ex_copy(ex);
	ensure_equals("code copy", ex_copy.code(), 2343);
	ensure_equals("what copy", ex_copy.what(), utf8("exception message"));
}

template<>template<>
void object::test<3>()
{
	try
	{
		try
		{
			throw ex;
		}
		catch (std::runtime_error const& err)
		{
			ensure_equals("catched what", err.what(), utf8("exception message"));
			throw;
		}
	}
	catch (sqlitepp::exception const& e)
	{
		ensure_equals("catched code", e.code(), 2343);
		ensure_equals("catched what", e.what(), utf8("exception message"));
	}
}

template<>template<>
void object::test<4>()
{
	try
	{
		se << utf("qaz");
		fail( "exception expected");
	}
	catch (sqlitepp::exception const& ex)
	{
		ensure( "error code", ex.code() != 0 );
		ensure( "what", ex.what() != 0 );
	}
}

int throw_ex()
{
	throw std::runtime_error("!");
}

} // namespace tut {
