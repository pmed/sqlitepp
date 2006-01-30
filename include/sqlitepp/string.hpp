//////////////////////////////////////////////////////////////////////////////
// string.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STRING_HPP_INCLUDED
#define SQLITEPP_STRING_HPP_INCLUDED

#include <limits>
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

namespace metaprogram {

//////////////////////////////////////////////////////////////////////////////

// Meta if
template<bool C, typename T1, typename T2>
struct if_ { typedef T1 result; };
template<typename T1, typename T2>
struct if_<false, T1, T2> { typedef T2 result; };

// Meta is
template<typename T1, typename T2>
struct is_ { enum { result = false }; };
template<typename T1>
struct is_<T1, T1> { enum { result = true }; };

struct utf16_char
{
	class unknown_utf16_char_type;
	typedef if_<std::numeric_limits<wchar_t>::digits == 16, wchar_t,
		if_<std::numeric_limits<unsigned short>::digits == 16, unsigned short,
		if_<std::numeric_limits<unsigned int>::digits == 16, unsigned int, 
			unknown_utf16_char_type>::result >::result >::result type;
};

struct utf32_char
{
	class unknown_utf16_char_type;
	typedef if_<std::numeric_limits<wchar_t>::digits == 32, wchar_t,
		if_<std::numeric_limits<unsigned short>::digits == 32, unsigned short,
		if_<std::numeric_limits<unsigned int>::digits == 32, unsigned int, 
			unknown_utf32_char_type>::result >::result >::result type;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace metaprogram 

//////////////////////////////////////////////////////////////////////////////

typedef char utf8_char;
typedef metaprogram::utf16_char::type utf16_char;
typedef metaprogram::utf32_char::type utf32_char;
struct utf16_char_traits
{
	typedef utf16_char 		char_type;
	typedef int 		        int_type;
	typedef std::streampos 	pos_type;
	typedef std::streamoff 	off_type;
	typedef std::mbstate_t 	state_type;

	static void assign(char_type& c1, char_type const& c2)
	{ c1 = c2; }

	static bool eq(char_type const& c1, char_type const& c2)
	{ return c1 == c2; }

	static bool lt(char_type const& c1, char_type const& c2)
	{ return c1 < c2; }

	static int compare(char_type const* s1, char_type const* s2, size_t n)
	{ return memcmp(s1, s2, n * sizeof(char_type)); }

	static size_t length(char_type const* s)
	{
		size_t len = 0;
		while ( s && *s )
			++len;
		return len;
	}

	static char_type const* find(char_type const* s, size_t n, char_type const& a)
	{
		while ( s && *s && n--)
			if ( *s == a )
				break;
		return s;
	}
	
	static char_type* move(char_type* s1, char_type const* s2, size_t n)
	{
		return static_cast<char_type*>(memmove(s1, s2, n * sizeof(char_type)));
	}

	static char_type* copy(char_type* s1, char_type const* s2, size_t n)
	{ 
		return static_cast<char_type*>(memcpy(s1, s2, n * sizeof(char_type)));
	}

    static char_type* assign(char_type* s, size_t n, char_type a)
	{
		return static_cast<char_type*>(memset(s, a, n * sizeof(char_type)));
	}

	static char_type to_char_type(int_type const& c)
	{
		return static_cast<char_type>(c);
	}

	static int_type to_int_type(char_type const& c)
	{
		return static_cast<int_type>(c);
	}

	static bool eq_int_type(int_type const& c1, int_type const& c2)
	{
		return c1 == c2;
	}

	static int_type eof()
	{
		return static_cast<int_type>(EOF);
	}

    static int_type not_eof(int_type const& c)
	{
		return (c == eof()) ? 0 : c;
	}
 };

typedef std::basic_string<utf8_char> utf8_string;
typedef std::basic_string<utf16_char, 
	metaprogram::if_<metaprogram::is_<wchar_t, utf16_char>::result,
		std::char_traits<wchar_t>, utf16_char_traits>::result> utf16_string;

#ifdef SQLITEPP_UTF16
	typedef utf16_char char_t;
	typedef utf16_string string_t;
#else
	typedef utf8_char char_t;
	typedef utf8_string string_t;
#endif // SQLITEPP_UTF16

utf8_string utf8(utf16_char const*);

inline utf8_string utf8(utf16_string const& str)
{
	return utf8(str.c_str());
}

inline utf8_string utf8(utf8_char const* str)
{
	return utf8_string(str);
}

inline utf8_string utf8(utf8_string const& str)
{
	return str;
}

utf16_string utf16(utf8_char const*);

inline utf16_string utf16(utf8_string const& str)
{
	return utf16(str.c_str());
}

inline utf16_string utf16(utf16_char const* str)
{
	return utf16_string(str);
}

inline utf16_string utf16(utf16_string const& str)
{
	return str;
}


//////////////////////////////////////////////////////////////////////////////

namespace aux {

//////////////////////////////////////////////////////////////////////////////

template<typename C, typename T1, typename T2>
struct selector;

template<typename T1, typename T2>
struct selector<utf8_char, T1, T2>
{
	selector(T1 t1, T2) : result(t1) {}
	typedef T1 type;
	type result;
};

template<typename T1, typename T2>
struct selector<utf16_char, T1, T2>
{
	selector(T1, T2 t2) : result(t2) {}
	typedef T2 type;
	type result;
};

template<typename T1, typename T2>
inline typename selector<char_t, T1, T2>::type select(T1 t1, T2 t2)
{
	return selector<char_t, T1, T2>(t1, t2).result;
}

template<typename C>
struct converter;

template<>
struct converter<utf8_char>
{
	static utf8_string convert(utf8_char const* str)
	{
		return str;
	}
	static utf8_string convert(utf8_string const& str)
	{
		return str;
	}
	static utf8_string convert(utf16_char const* str)
	{
		return utf8(str);
	}
	static utf8_string convert(utf16_string const& str)
	{
		return utf8(str);
	}
};

template<>
struct converter<utf16_char>
{
	static utf16_string convert(utf8_char const* str)
	{
		return utf16(str);
	}
	static utf16_string convert(utf8_string const& str)
	{
		return utf16(str);
	}
	static utf16_string convert(utf16_char const* str)
	{
		return str;
	}
	static utf16_string convert(utf16_string const& str)
	{
		return str;
	}
};

//////////////////////////////////////////////////////////////////////////////

} // namespace aux

//////////////////////////////////////////////////////////////////////////////

inline string_t utf(utf8_char const* str)
{
	return aux::converter<char_t>::convert(str);
}

inline string_t utf(utf8_string const& str)
{
	return aux::converter<char_t>::convert(str);
}

inline string_t utf(utf16_char const* str)
{
	return aux::converter<char_t>::convert(str);
}

inline string_t utf(utf16_string const& str)
{
	return aux::converter<char_t>::convert(str);
}

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STRING_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
