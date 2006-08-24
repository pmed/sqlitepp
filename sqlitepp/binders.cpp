//////////////////////////////////////////////////////////////////////////////
// binders.cpp
//
// Copyright (c) 2004 Pavel Medvedev
// Use, modification and distribution is subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>

#include "binders.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////
//
// into_binder
//

into_binder::into_binder()
{
}
//----------------------------------------------------------------------------

into_binder::into_binder(into_binder const&)
{
}
//----------------------------------------------------------------------------

into_binder::~into_binder() 
{
}
//----------------------------------------------------------------------------

into_binder* into_binder::clone() const
{
	into_binder* copy = do_clone();
	assert(typeid(*this)==typeid(*copy) && !"do_clone not overriden correctly");
	return copy;
}
//----------------------------------------------------------------------------

int into_binder::bind(statement& st, int pos)
{
	do_bind(st, pos);
	return pos + 1;
}
//----------------------------------------------------------------------------

void into_binder::update(statement& st)
{
	do_update(st);
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
//
// use_binder
//

use_binder::use_binder()
{
}
//----------------------------------------------------------------------------

use_binder::use_binder(use_binder const&)
{
}
//----------------------------------------------------------------------------

use_binder::~use_binder() 
{
}
//----------------------------------------------------------------------------

use_binder* use_binder::clone() const
{
	use_binder* copy = do_clone();
	assert(typeid(*this)==typeid(*copy) && !"do_clone not overriden correctly");
	return copy;
}
//----------------------------------------------------------------------------

int use_binder::bind(statement& st, int pos)
{
	do_bind(st, pos);
	return pos + 1;
}
    
//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////
