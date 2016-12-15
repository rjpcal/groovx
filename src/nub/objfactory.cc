/** @file nub/objfactory.cc generic factory for creating objects given
    a type name */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun 30 15:01:02 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "objfactory.h"

namespace
{
  nub::obj_factory* s_instance = nullptr;
}

nub::obj_factory::obj_factory() :
  rutz::factory<soft_ref<nub::object> >() {}

nub::obj_factory::~obj_factory() noexcept {}

nub::obj_factory& nub::obj_factory::instance()
{
  if (s_instance == nullptr)
    {
      s_instance = new obj_factory;
    }
  return *s_instance;
}
