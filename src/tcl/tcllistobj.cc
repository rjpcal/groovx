///////////////////////////////////////////////////////////////////////
//
// tcllistobj.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 12:32:35 2001
// written: Sun Nov  3 13:41:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLISTOBJ_CC_DEFINED
#define TCLLISTOBJ_CC_DEFINED

#include "tcl/tcllistobj.h"

#include "tcl/tclerror.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"

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
  if ( Tcl_ListObjGetElements(0, itsList.obj(), &count, &itsElements) != TCL_OK)
    {
      throw Tcl::TclError("couldn't split Tcl list");
    }

  Assert(count >= 0);
  itsLength = (unsigned int) count;
}

void Tcl::List::doAppend(const Tcl::ObjPtr& obj, unsigned int times)
{
DOTRACE("Tcl::List::doAppend");

  itsList.ensureUnique();

  while (times--)
    if ( Tcl_ListObjAppendElement(0, itsList.obj(), obj.obj()) != TCL_OK )
      {
        throw Tcl::TclError("couldn't append to Tcl list");
      }

  invalidate();
}

Tcl_Obj* Tcl::List::at(unsigned int index) const
{
DOTRACE("Tcl::List::at");

  update();

  if (index >= itsLength)
    throw Tcl::TclError("index was out of range in Tcl list access");

  dbgEval(3, index); dbgEvalNL(3, itsElements[index]);

  return itsElements[index];
}

unsigned int Tcl::List::getLength(Tcl_Obj* obj)
{
DOTRACE("Tcl::List::getLength");

  int len;
  if ( Tcl_ListObjLength(0, obj, &len) != TCL_OK)
    {
      throw Tcl::TclError("couldn't get list length of Tcl object");
    }

  Assert(len >= 0);

  return (unsigned int) len;
}

static const char vcid_tcllistobj_cc[] = "$Header$";
#endif // !TCLLISTOBJ_CC_DEFINED
