#include <iostream>
#include <tut.h>

#include <sqlitepp/string.hpp>

namespace 
{

using namespace sqlitepp;

struct string_data
{
	string_t str_;
};
	
typedef tut::test_group<string_data> test_group;
typedef test_group::object object;

test_group g("string");


template<>template<>
void object::test<1>()
{
	ensure("empty string", str_.empty());
	std::cout << "char_t size " << sizeof(sqlitepp::char_t);
}

template<>template<>
void object::test<2>()
{
	sqlitepp::string_t s;
	ensure("empty string", s.empty());
	ensure("strings", str_ == s);
}

template<>template<>
void object::test<3>()
{
	str_ = utf(L"1");
	sqlitepp::string_t s(str_);
	ensure("strings", str_ == s);

	str_ = utf(L"2");
	s = str_;
	ensure("strings", str_ == s);
	
	s.clear();
	ensure("empty", s.empty());
}

} // namespace
