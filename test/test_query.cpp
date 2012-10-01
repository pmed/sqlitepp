#include <tut.h>

#include <locale>

#include <sqlitepp/query.hpp>
#include <sqlitepp/binders.hpp>
#include <sqlitepp/use.hpp>

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

	q.sql(utf(L"qaz"));
	ensure_equals("q.sql() == qaz", q.sql(), utf(L"qaz") );
}

template<>template<>
void object::test<2>()
{
	int z = 100;
	q << utf(L"qaz") << z;
	ensure_equals("q.sql() == qaz100", q.sql(), utf(L"qaz100"));
}

template<>template<>
void object::test<3>()
{
	try
	{
		into_binder_ptr i;
		q.put(i);
		fail("exception expected");
	}
	catch (std::invalid_argument const&)
	{
	}
	ensure("intos empty", q.intos().empty());
	try
	{
		use_binder_ptr u;
		q.put(u);
		fail("exception expected");
	}
	catch (std::invalid_argument const&)
	{
	}
	ensure("uses empty", q.uses().empty());
}

template<>template<>
void object::test<4>()
{
	query q1, q2;

	q1 << "some text " << 7 << " with numbers " << 23.44, use("zz");

	ensure_equals("q1 sql", q1.sql(), utf("some text 7 with numbers 23.44"));
	ensure_equals("q1 uses", q1.uses().size(), 1u);

	ensure_equals("q2 sql", q2.sql(), utf(""));
	ensure_equals("q2 uses", q2.uses().size(), 0u);

	swap(q1, q2);

	ensure_equals("swapped q1 sql", q1.sql(), utf(""));
	ensure_equals("swapped q1 uses", q1.uses().size(), 0u);

	ensure_equals("swapped q2 sql", q2.sql(), utf("some text 7 with numbers 23.44"));
	ensure_equals("swapped q2 uses", q2.uses().size(), 1u);

	q1 << "some string " << 33 ;
	q2 << " added";
	ensure_equals("changed q1 sql", q1.sql(), utf("some string 33"));
	ensure_equals("changed q2 sql", q2.sql(), utf("some text 7 with numbers 23.44 added"));

	q1.clear();
	q2.clear();

	ensure("cleared q1 sql", q1.sql().empty());
	ensure("cleared q2 sql", q2.sql().empty());

	ensure("cleared q1 intos", q1.intos().empty());
	ensure("cleared q2 intos", q2.intos().empty());

	ensure("cleared q1 uses", q1.uses().empty());
	ensure("cleared q2 uses", q2.uses().empty());
}

} // namespace tut {
