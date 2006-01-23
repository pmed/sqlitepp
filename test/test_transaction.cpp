#include <tut.h>

#include <sqlitepp/transaction.hpp>
#include <sqlitepp/exception.hpp>
#include <sqlitepp/binders.hpp>

#include "statement_data.hpp"

namespace
{


struct transaction_data : statement_data
{
};

typedef tut::test_group<transaction_data> test_group;
typedef test_group::object object;

test_group g("transaction");

using namespace sqlitepp;

// implicit rollback
template<>template<>
void object::test<1>()
{
	int rows;
	ensure( "no active txn", !se.active_txn() );
	{
		transaction t(se);
		ensure( "active txn", se.active_txn() );
		ensure( "this active txn", se.active_txn() == &t );
		
		record r1(1, "Евгения", 566.24);
		r1.insert(se);

		se << "select count(*) from some_table", into(rows);
		ensure_equals("row inserted", rows, 1);
	}	
	ensure( "no active txn", !se.active_txn() );
	se << "select count(*) from some_table", into(rows);
	ensure_equals("rollback", rows, 0);
}

// explicit commit
template<>template<>
void object::test<2>()
{
	{
		transaction t(se);
		record r1(1, "Евгения", 566.24);
		r1.insert(se);
		t.commit();
	}
	ensure( "no active txn", !se.active_txn() );

	int rows;
	se << "select count(*) from some_table", into(rows);
	ensure_equals("commit", rows, 1);
}

// nested transactions
template<>template<>
void object::test<3>()
{
	transaction t1(se);
	try
	{
		transaction t2(se);
		fail("exception expected");
	}
	catch(sqlitepp::exception const& ex)
	{
		ensure_equals("exception", strcmp(ex.what(), "nested transactions are not supported"), 0);
	}
}

} // namespace
