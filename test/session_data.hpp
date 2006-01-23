#ifndef SQLITEPP_TEST_SESSION_DATA_HPP_INCLUDED
#define SQLITEPP_TEST_SESSION_DATA_HPP_INCLUDED

#include <string>
#include <sqlitepp/session.hpp>

struct session_data
{
	session_data(std::string const& name = "test.db");
	~session_data();

	std::string name_;
	sqlitepp::session se;
};

#endif // SQLITEPP_TEST_SESSION_DATA_HPP_INCLUDED
