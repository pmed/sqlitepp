// $Id$

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

test_group g("5. transaction");

using namespace sqlitepp;

// implicit rollback
template<>template<>
void object::test<1>()
{
	int rows;
	ensure( "no active txn", !se.active_txn() );
	{
		transaction t(se);
		ensure_equals( "this active txn", se.active_txn(), &t );
		
		record r1(1, utf(L"Евгения"), 566.24);
		r1.insert(se);

		se << utf(L"select count(*) from some_table"), into(rows);
		ensure_equals("row inserted", rows, 1);
	}	
	ensure( "no active txn", !se.active_txn() );
	se << utf(L"select count(*) from some_table"), into(rows);
	ensure_equals("rollback", rows, 0);
}

// explicit commit
template<>template<>
void object::test<2>()
{
	{
		transaction t(se);
		record r1(1, utf(L"Евгения"), 566.24);
		r1.insert(se);
		t.commit();
	}
	ensure( "no active txn", !se.active_txn() );

	int rows;
	se << utf(L"select count(*) from some_table"), into(rows);
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
		fail("nested_txn_exception expected");
	}
	catch(nested_txn_not_supported const&)
	{
	}
	catch(...)
	{
		fail("nested_txn_not_supported expected");
	}
}

} // namespace
