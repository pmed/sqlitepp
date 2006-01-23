#include <tut.h>
#include <tut_reporter.h>

#include <iostream>

namespace tut
{
	test_runner_singleton runner;
}

int main()
{
    std::cout << "SQLite++ test application." << std::endl;

	tut::reporter callback;
	tut::runner.get().set_callback(&callback);
	try
	{
		tut::runner.get().run_tests();
	}
	catch( const std::exception& ex )
	{
		std::cerr << "tut raised ex: " << ex.what() << std::endl;
	}

	//return callback.all_ok() ? 0 : -1;
	system("pause");
}
