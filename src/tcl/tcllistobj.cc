///////////////////////////////////////////////////////////////////////
//
// tcllistobj.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jul 11 12:32:35 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef TCLLISTOBJ_CC_DEFINED
#define TCLLISTOBJ_CC_DEFINED

#include "tcl/tcllistobj.h"

#include "util/error.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

Tcl::List::List() :
  itsList(Tcl_NewListObj(0,0)),
  itsElements(0),
  itsLength(0)
{
DOTRACE("Tcl::List::List");
  split();
}

Tcl::List::List(const Tcl::ObjPtr& listObj) :
  itsList(listObj),
  itsElements(0),
  itsLength(0)
{
DOTRACE("Tcl::List::List");
  split();
}

void Tcl::List::split() const
{
DOTRACE("Tcl::List::split");

  int count;
  if ( Tcl_ListObjGetElements(0, itsList.obj(), &count, &itsElements)
       != TCL_OK)
    {
      throw rutz::error("couldn't split Tcl list", SRC_POS);
    }

  ASSERT(count >= 0);
  itsLength = static_cast<unsigned int>(count);
}

void Tcl::List::doAppend(const Tcl::ObjPtr& obj, unsigned int times)
{
DOTRACE("Tcl::List::doAppend");

  itsList.ensureUnique();

  while (times--)
    if ( Tcl_ListObjAppendElement(0, itsList.obj(), obj.obj())
         != TCL_OK )
      {
        throw rutz::error("couldn't append to Tcl list", SRC_POS);
      }

  invalidate();
}

Tcl_Obj* Tcl::List::at(unsigned int index) const
{
DOTRACE("Tcl::List::at");

  update();

  if (index >= itsLength)
    throw rutz::error("index was out of range in Tcl list access",
                      SRC_POS);

  dbg_eval(3, index); dbg_eval_nl(3, itsElements[index]);

  return itsElements[index];
}

unsigned int Tcl::List::getLength(Tcl_Obj* obj)
{
DOTRACE("Tcl::List::getLength");

  int len;
  if ( Tcl_ListObjLength(0, obj, &len) != TCL_OK)
    {
      throw rutz::error("couldn't get list length of Tcl object",
                        SRC_POS);
    }

  ASSERT(len >= 0);

  return static_cast<unsigned int>(len);
}

static const char vcid_tcllistobj_cc[] = "$Id$ $URL$";
#endif // !TCLLISTOBJ_CC_DEFINED
