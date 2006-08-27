// $Id$

#include <tut.h>

#include <sqlitepp/exception.hpp>
#include <sqlitepp/binders.hpp>

#include "statement_data.hpp"

namespace
{
	
struct use_data : statement_data
{
	use_data()
	{
	}
};

typedef tut::test_group<use_data> test_group;
typedef test_group::object object;

test_group g("8. use");

using namespace sqlitepp;

// use by pos binding
template<>template<>
void object::test<1>()
{
	
	record recs[2] = { record(1, utf(L"Наталия"), 123.45), record(2, utf(L"Анастасия"), 678.90) };

	// insert records
	for (size_t i = 0; i < dimof(recs); ++i)
	{
		st << utf(L"insert into some_table values( :id, :name, :salary, NULL)"),
			use(recs[i].id), use(recs[i].name), use(recs[i].salary);
		ensure("row inserted", !st.exec() );
	}

	// count inserted record
	int count;
	se << utf(L"select count(*) from some_table"), into(count);
	ensure_equals("row count", count, static_cast<int>(dimof(recs)) ); 

	// check inserted records
	record r;
	st << utf(L"select id, name, salary from some_table"),
		into(r.id, utf(L"id")), into(r.name, utf(L"name")), into(r.salary, utf(L"salary"));
	for (count = 0; st.exec(); ++count)
	{
		ensure_equals(r, recs[count]);
	}
	ensure_equals("row count", count, static_cast<int>(dimof(recs))); 
}

// use by name binding
template<>template<>
void object::test<2>()
{
	record recs[2] = { record(1, utf(L"Маша"), 123.45), record(2, utf(L"Люба"), 678.90) };

	// insert records
	for (size_t i = 0; i < dimof(recs); ++i)
	{
		st << utf(L"insert into some_table(id, name, salary) values( :id, :name, :salary)"),
			use(recs[i].id, utf(L":id")),
			use(recs[i].name, utf(L":name")),
			use(recs[i].salary, utf(L":salary"));
		ensure("row inserted", !st.exec() );
	}

	// count inserted record
	int count;
	se << utf(L"select count(*) from some_table"), into(count);
	ensure_equals("row count", count, static_cast<int>(dimof(recs)) ); 

	// check inserted records
	record r;
	st << utf(L"select id, name, salary from some_table"),
		into(r.id, utf(L"id")), into(r.name, utf(L"name")), into(r.salary, utf(L"salary"));
	for (count = 0; st.exec(); ++count)
	{
		ensure_equals(r, recs[count]);
	}
	ensure_equals(count, dimof(recs)); 
}

// use by invalid name binding
template<>template<>
void object::test<3>()
{
	try
	{
		int id;
		st << utf(L"insert into some_table(id, name, salary) values( :id, '', 0)"),
			use(id, utf(L"id_ZZZ"));
		st.exec();
		fail( "exception expected" );
	}
	catch(sqlitepp::no_such_column const&)
	{
		ensure( "statement not prepared", !st.is_prepared() );
	}
}

// use BLOB
template<>template<>
void object::test<4>()
{
	const char BLOB_data[] = { "very big data" };

	record r1(1, utf(L"Вера"), 545.6);
	r1.data.assign(BLOB_data, BLOB_data + sizeof(BLOB_data));
	
	st << utf(L"insert into some_table(id, name, salary, data) values( :id, :name, :salary, :data)"),
		use(r1.data, utf(L":data")), use(r1.id, utf(L":id")),
		use(r1.name, utf(L":name")), use(r1.salary, utf(L":salary"));
	ensure("row inserted", !st.exec() );

	record r2;
	st << utf(L"select * from some_table where id = 1"),
		into(r2.id), into(r2.name), into(r2.salary), into(r2.data);

	ensure("select completed", st.exec());
	ensure_equals(r2, r1);
	ensure( "single row", !st.exec() );
}

} // namespace
