///////////////////////////////////////////////////////////////////////
//
// tcllistobj.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 12:32:35 2001
// written: Wed Jul 11 13:05:54 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLISTOBJ_CC_DEFINED
#define TCLLISTOBJ_CC_DEFINED

#include "tcl/tcllistobj.h"

#include "tcl/tclerror.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

Tcl::List::List(Tcl_Obj* listObj) :
  itsList(listObj),
  itsElements(0),
  itsLength(0)
{
DOTRACE("Tcl::List::List");

  int count;
  if ( Tcl_ListObjGetElements(0, itsList, &count, &itsElements)
       != TCL_OK)
    {
      throw Tcl::TclError("couldn't split Tcl list");
    }

  Assert(count >= 0);
  itsLength = (unsigned int) count;
}

Tcl_Obj* Tcl::List::at(unsigned int index) const
{
DOTRACE("Tcl::List::at");

  if (index > itsLength)
    throw Tcl::TclError("index was out of range in Tcl list access");

  DebugEval(index); DebugEvalNL(itsElements[index]);

  return itsElements[index];
}

static const char vcid_tcllistobj_cc[] = "$Header$";
#endif // !TCLLISTOBJ_CC_DEFINED
