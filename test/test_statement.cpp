// $Id$

#include <cmath>
#include <iterator>
#include <tut.h>
#include <sqlitepp/exception.hpp>

#include "statement_data.hpp"

using namespace sqlitepp;

std::ostream& operator<<(std::ostream& os, sqlitepp::blob const& b)
{
	os << '[' << b.size() << "] = (";
	std::copy(b.begin(), b.end(), std::ostream_iterator<sqlitepp::blob::value_type>(os, ", "));
	return os;
}

statement_data::statement_data() : st(se)
{
	se << utf(L"create table some_table(id integer, name text, salary real(8), data blob)");
}

statement_data::~statement_data()
{
	se << utf(L"drop table some_table");
}

void statement_data::record::insert(sqlitepp::session& se)
{
	se << utf(L"insert into some_table(id, name, salary) values") << 
		utf(L"(") << id << utf(L", \'") << name << utf(L"\', ") << salary << utf(L")");
}

void ensure_equals(statement_data::record const& r1, 
				   statement_data::record const& r2)
{
	tut::ensure_equals("id", r1.id, r2.id);
	tut::ensure_equals("name", r1.name, r2.name);
	tut::ensure_distance("salary", r1.salary, r2.salary, 0.01);
	tut::ensure_equals("data", r1.data, r2.data);
}

namespace tut {
	
typedef tut::test_group<statement_data> statement_test_group;
typedef statement_test_group::object object;

statement_test_group st_g("4. statement");

// test INSERT
template<>template<>
void object::test<1>()
{
	record r(1, utf(L"Janet"), 345.2);
	r.insert(se);
}

// test query
template<>template<>
void object::test<2>()
{
	ensure("no query", st.q().sql().empty());
	ensure("not prepared", !st.is_prepared());
	st.q().sql(utf(L"zzz"));
	ensure_equals("query == zzz", st.q().sql(), utf(L"zzz"));
}

// test empty query error
template<>template<>
void object::test<3>()
{
	ensure("no query", st.q().empty());
	try
	{
		st.exec();
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure("not prepared", !st.is_prepared());
	}
}

/// Prepare/finalize
template<>template<>
void object::test<4>()
{
	ensure("no query", st.q().empty());
	st.q().sql(utf(L"insert into some_table(id, name, salary) values(1, 'lisa', 23.345)"));
	
	st.prepare();
	ensure("prepared", st.is_prepared());
	
	st.exec();

	st.finalize();
	ensure("not prepared", !st.is_prepared());
}

template<>template<>
void object::test<5>()
{
	record r(1, utf(L"Daria"), 123.45);
	r.insert(se);
	
	st << utf(L"select * from some_table");
	ensure ( "select executed", st.exec() );

	ensure_equals("col count", st.column_count(), 4);

	ensure_equals( "col 0 name", st.column_name(0), utf(L"id") );
	ensure_equals( "col 0 index", st.column_index(utf(L"id")), 0 );
	ensure_equals( "col 0 type", st.column_type(0), statement::COL_INT );
	ensure_equals( "col 0 value", st.get<int>(0), r.id );

	ensure_equals( "col 1 name", st.column_name(1), utf(L"name") );
	ensure_equals( "col 1 index", st.column_index(utf(L"name")), 1 );
	ensure_equals( "col 1 type", st.column_type(1), statement::COL_TEXT );
	ensure_equals( "col 1 value", st.get<sqlitepp::string_t>(1), r.name );

	ensure_equals( "col 2 name", st.column_name(2), utf(L"salary") );
	ensure_equals( "col 2 index", st.column_index(utf(L"salary")), 2 );
	ensure_equals( "col 2 type", st.column_type(2), statement::COL_FLOAT );
	ensure_distance( "col 2 value", st.get<double>(2), r.salary, 0.01 );

	ensure_equals( "col 3 name", st.column_name(3), utf(L"data") );
	ensure_equals( "col 3 index", st.column_index(utf(L"data")), 3 );
	ensure_equals( "col 3 type", st.column_type(3), statement::COL_NULL );

	try
	{
		st.column_index(utf(L"zz"));
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
	}
	try
	{
		st.get<double>(200);
		fail( "bind range exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
	}

	st.reset();
}

} // namespace tut {
