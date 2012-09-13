#include <tut.h>

#include <sqlitepp/string.hpp>

using namespace sqlitepp;

namespace tut {

struct string_data
{
	string_t str_;
};

typedef tut::test_group<string_data> string_test_group;
typedef string_test_group::object object;

string_test_group str_g("1. string");


template<>template<>
void object::test<1>()
{
	ensure("empty string", str_.empty());
}

template<>template<>
void object::test<2>()
{
	sqlitepp::string_t s;
	ensure("empty string", s.empty());
	ensure_equals("strings", str_, s);
}

template<>template<>
void object::test<3>()
{
	str_ = utf("1");
	sqlitepp::string_t s(str_);
	ensure_equals("strings", str_, s);

	str_ = utf("2");
	s = str_;
	ensure_equals("strings", str_, s);
	
	s.clear();
	ensure("empty", s.empty());
}

} // namespace tut {
