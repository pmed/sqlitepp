//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_CONFIG_HPP_INCLUDED
#define SQLITEPP_CONFIG_HPP_INCLUDED

// In C++11 default behavior when throwing an exception in destructors has
// changed. Default for all destructors is noexcept, which means the
// program will be immediately terminated when an exception in a destructor
// is thrown. To change this behavior to allow throwing catchable
// exceptions in a destructor we add the noexcept(false) specifier.  
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define NOEXCEPT_FALSE  noexcept(false)
#else
#define NOEXCEPT_FALSE
#endif

#endif // SQLITEPP_CONFIG_HPP_INCLUDED
