#include <cmath>
#include <tut.h>
#include <sqlitepp/exception.hpp>

#include "statement_data.hpp"

statement_data::statement_data() : st(se)
{
	se.open(name_);
	se << "create table some_table(id integer, name text, salary real(8), data blob)";
}

statement_data::~statement_data()
{
	st.finalize();
	se << "drop table some_table";
}

void statement_data::record::insert(sqlitepp::session& se)
{
	se << "insert into some_table(id, name, salary) values" << 
		"(" << id << ", \'" << name << "\', " << salary << ")";
}

void ensure_equals(statement_data::record const& r1, 
				   statement_data::record const& r2)
{
	tut::ensure_equals("id", r1.id, r2.id);
	tut::ensure_equals("name", r1.name, r2.name);
	tut::ensure_distance("salary", r1.salary, r2.salary, 0.01);
	tut::ensure("data", r1.data == r2.data);
}

namespace
{
	
typedef tut::test_group<statement_data> test_group;
typedef test_group::object object;

test_group g("statement");

using namespace sqlitepp;

// test INSERT
template<>template<>
void object::test<1>()
{
	record r(1, "Жанна", 345.2);
	r.insert(se);
	ensure("session valid", se);
}

// test query
template<>template<>
void object::test<2>()
{
	ensure("no query", st.sql().empty());
	ensure("not prepared", !st.prepared());
	ensure("not prepared 2", !st);
	st.set_sql("zzz");
	ensure_equals("query", st.sql(), "zzz");
}

// test empty query error
template<>template<>
void object::test<3>()
{
	ensure("no query", st.sql().empty());
	try
	{
		st.exec();
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure("not prepared", !st);
	}
}

/// Prepare/finalize
template<>template<>
void object::test<4>()
{
	ensure("no query", st.sql().empty());
	st.set_sql("insert into some_table(id, name, salary) values(1, 'lisa', 23.345)");
	
	st.prepare();
	ensure("prepared", st);
	
	st.exec();

	st.finalize();
	ensure("not prepared", !st);
}

template<>template<>
void object::test<5>()
{
	record r(1, "Дарья", 123.45);
	r.insert(se);
	ensure("session valid", se);
	
	st << "select * from some_table";
	ensure ( "select executed", st.exec() );

	ensure_equals( "col count", st.column_count(), 4 );

	ensure_equals( "col 0 name", st.column_name(0), std::string("id") );
	ensure_equals( "col 0 index", st.column_index("id"), 0 );
	ensure_equals( "col 0 type", st.column_type(0), statement::COL_INT );
	ensure_equals( "col 0 value", st.column_as<int>(0), r.id );

	ensure_equals( "col 1 name", st.column_name(1), std::string("name") );
	ensure_equals( "col 1 index", st.column_index("name"), 1 );
	ensure_equals( "col 1 type", st.column_type(1), statement::COL_TEXT );
	ensure_equals( "col 1 value", st.column_as<std::string>(1), r.name );

	ensure_equals( "col 2 name", st.column_name(2), std::string("salary") );
	ensure_equals( "col 2 index", st.column_index("salary"), 2 );
	ensure_equals( "col 2 type", st.column_type(2), statement::COL_FLOAT );
	ensure_equals( "col 2 value", st.column_as<double>(2), r.salary );

	ensure_equals( "col 3 name", st.column_name(3), std::string("data") );
	ensure_equals( "col 3 index", st.column_index("data"), 3 );
	ensure_equals( "col 3 type", st.column_type(3), statement::COL_NULL );

	try
	{
		st.column_index("zz");
		fail( "exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure( "session invalid", !se );
	}
	try
	{
		st.column_as<double>(200);
		fail( "bind range exception expected" );
	}
	catch(sqlitepp::exception const&)
	{
		ensure( "session invalid", !se );
	}
}

} // namespace
