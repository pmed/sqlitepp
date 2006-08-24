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
	ensure("empty query", q.empty() );

	q.set_sql(utf(L"qaz"));
	ensure_equals("q.sql() == qaz", q.sql(), utf(L"qaz") );
}

template<>template<>
void test_group::object::test<2>()
{
	q.set_sql(utf(L"qaz"));
	query q2(q);
	ensure_equals("sql q = q2", q.sql(), q2.sql() );
	ensure_equals("intos q = q2", q.intos().size(), q2.intos().size() );
	ensure_equals("uses q = q2", q.uses().size(), q2.uses().size() );
}

template<>template<>
void test_group::object::test<3>()
{
	q.set_sql(utf(L"qaz"));
	ensure("not empty", !q.empty() );
	query q2(q);
	ensure_equals("sql q == q2", q.sql(), q2.sql() );
	
	q2.clear();
	ensure("q !empty", !q.empty() );
	ensure("q2 empty", q2.empty() );
}

template<>template<>
void test_group::object::test<4>()
{
	query q2(utf(L"qaz"));

	ensure("q", q.sql().empty() );
	ensure_equals("q2.sql() == qaz", q2.sql(), utf(L"qaz") );
	swap(q, q2);
	ensure_equals("q.sql() == qaz", q.sql(), utf(L"qaz") );
	ensure("q2 empty", q2.sql().empty() );

	q <<utf(L"aaaa");
	ensure_equals("q.sql() = qazaaaa", q.sql(), utf(L"qazaaaa"));

	q2 << 23.34;
	ensure_equals("q2.sql() = 23.34", q2.sql(), utf(L"23.34"));

}

template<>template<>
void test_group::object::test<5>()
{
	int z = 100;
	q << utf(L"qaz") << z;
	ensure_equals("q.sql() == qaz100", q.sql(), utf(L"qaz100"));

	query q2(q);
	q2 << utf(L"wert");
	ensure_equals("q2.sql() == qaz100wert", q2.sql(), utf(L"qaz100wert"));
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
