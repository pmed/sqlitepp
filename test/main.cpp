#include <tut.h>
#include <tut_reporter.h>

#include <iostream>

namespace tut
{
	test_runner_singleton runner;
}

int main(int argc, char** argv)
{
	std::cout << "SQLite++ test application." << std::endl;

	tut::reporter callback;
	tut::runner.get().set_callback(&callback);
	try
	{
		switch ( argc )
		{
		case 1:
			// run all tests
			tut::runner.get().run_tests();
			break;
		case 2: 
			// run a test group
			tut::runner.get().run_tests(argv[1]);
			break;
		case 3:
			// run particular test in a group
			tut::runner.get().run_test(argv[1], atoi(argv[2]));
			break;
		default:
			std::cout << "Usage\n" << argv[0] << " [group name] [test number]" << std::endl;
			return -1;
		}
	}
	catch( std::exception const& ex )
	{
		std::cerr << "tut raised ex: " << ex.what() << std::endl;
	}

	std::cin.get();
}
