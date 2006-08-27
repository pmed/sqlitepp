#include <tut.h>
#include <time.h>

#include <sqlitepp/binders.hpp>
#include <sqlitepp/string.hpp>
#include <sqlitepp/transaction.hpp>

#include "statement_data.hpp"

using namespace sqlitepp;

namespace sqlitepp 
{

template<>
struct converter<tm>
{
	typedef time_t base_type;
	static time_t from(tm& src)
	{
		return mktime(&src);
	}
	static tm to(time_t const src)
	{
		return *localtime(&src);
	}
};

}
namespace
{

	struct conv_data : statement_data
{
	conv_data()
	{
		se << utf(L"insert into some_table(id, name, salary) values(100, 'qaqa', 0.1)");
	}

	template<typename T>
	void select_id()
	{
		T id;
		se << utf(L"select id from some_table"), into(id);
		tut::ensure_equals("id", id, T(100));
	}

	template<typename T>
	void select_salary(T const& expected, T const& dist)
	{
		T salary;
		se << utf(L"select salary from some_table"), into(salary);
		tut::ensure_distance("salary", salary, expected, dist);
	}
};

typedef tut::test_group<conv_data> test_group;
typedef test_group::object object;

test_group g("9. conversion");

template<>template<>
void object::test<1>()
{
	select_id<char>();
	select_id<unsigned char>();
	select_id<signed char>();
	select_id<short>();
	select_id<short>();
	select_id<unsigned short>();
	select_id<int>();
	select_id<unsigned int>();
}

template<>template<>
void object::test<2>()
{
	select_salary<float>(0.1f, 0.01f);
	select_salary<double>(0.1, 0.01);
}

template<>template<>
void object::test<3>()
{
	transaction txn(se);

	se << utf(L"create table timer(t integer)");
	
	time_t now = time(0);
	tm t1 = *localtime(&now);
	se << utf(L"insert into timer(t) values(:t)"), use(t1);
	
	tm t2;
	se << utf(L"select t from timer"), into(t2);

	ensure("tm equal", memcmp(&t1, &t2, sizeof tm) == 0);
}

} // namespace
