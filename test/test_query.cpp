#include <tut.h>

#include <locale>

#include <sqlitepp/query.hpp>
#include <sqlitepp/binders.hpp>

using namespace sqlitepp;

namespace tut {

struct query_data
{
	query q;
};

typedef tut::test_group<query_data> query_test_group;
typedef query_test_group::object object;

query_test_group qg("6. query");

template<>template<>
void object::test<1>()
{
	ensure("empty query", q.empty() );

	q.sql(utf("qaz"));
	ensure_equals("q.sql() == qaz", q.sql(), utf("qaz") );
}

template<>template<>
void object::test<2>()
{
	int z = 100;
	q << "qaz" << z;
	ensure_equals("q.sql() == qaz100", q.sql(), utf("qaz100"));
}

template<>template<>
void object::test<6>()
{
	try
	{
		q.put(into_binder_ptr());
		fail("exception expected");
	}
	catch (std::invalid_argument const&)
	{
	}
	ensure("q empty", q.empty());
	try
	{
		q.put(use_binder_ptr());
		fail("exception expected");
	}
	catch (std::invalid_argument const&)
	{
	}
	ensure("q empty", q.empty());
}

} // namespace tut {
