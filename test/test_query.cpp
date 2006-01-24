#include <tut.h>

#include <locale>

#include <sqlitepp/query.hpp>
#include <sqlitepp/binders.hpp>

namespace {

using namespace sqlitepp;

struct query_data
{
	query q;
};

typedef tut::test_group<query_data> test_group;
typedef test_group::object object;

test_group g("query");

template<>template<>
void test_group::object::test<1>()
{
	ensure("empty query", q.sql().empty() );

	q.set_sql("qaz");
	ensure("q.sql() == qaz", q.sql() == "qaz" );
}

template<>template<>
void test_group::object::test<2>()
{
	q.set_sql("qaz");
	query q2("qaz");
	ensure("q = q2", q.sql() == q2.sql() );
}

template<>template<>
void test_group::object::test<3>()
{
	query q2("qaz");
	q2.clear();
	ensure("clear", q.sql() == q2.sql() );
}

template<>template<>
void test_group::object::test<4>()
{
	query q2("qaz");

	ensure("q", q.sql().empty() );
	ensure("q2.sql() == qaz", q2.sql() == "qaz" );
	swap(q, q2);
	ensure("q.sql() == qaz", q.sql() == "qaz" );
	ensure("q2 empty", q2.sql().empty() );

	q << "aaaa";
	ensure("q.sql() = qazaaaa", q.sql() == "qazaaaa");

	q2 << 23.34;
	ensure("q2.sql() = 23.34", q2.sql() == "23.34");

}

template<>template<>
void test_group::object::test<5>()
{
	int z = 100;
	q << "qaz" << z;
	ensure("q.sql() == qaz100", q.sql() == "qaz100");

	query q2(q);
	ensure("destructive copy q", q.sql().empty() );
	ensure("destructive copy q2", q2.sql() == "qaz100" );

	q << "aaaa";
	ensure("q.sql() == aaaa", q.sql() == "aaaa");

	q2 << "wert";
	ensure("q2.sql() == qaz100wert", q2.sql() == "qaz100wert");
}

template<>template<>
void test_group::object::test<6>()
{
	try
	{
		into_binder_ptr i;
		q.put(i);
		fail("exception expected");
	}
	catch(std::invalid_argument const&)
	{
	}
	ensure("intos empty", q.intos().empty());
	try
	{
		use_binder_ptr u;
		q.put(u);
		fail("exception expected");
	}
	catch(std::invalid_argument const&)
	{
	}
	ensure("uses empty", q.uses().empty());
}

} // namespace {
