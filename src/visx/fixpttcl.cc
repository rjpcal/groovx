///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
// Rob Peters
// created: Jan-99
// written: Wed Mar 15 11:14:08 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include <tcl.h>

#include "fixpt.h"
#include "objlist.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"


extern "C" Tcl_PackageInitProc Fixpt_Init;

int Fixpt_Init(Tcl_Interp* interp) {
DOTRACE("Fixpt_Init");

  new Tcl::PropertyListItemPkg<FixPt, ObjList>(interp, ObjList::theObjList(),
															  "FixPt", "$Revision$");

  Tcl_Eval(interp,
			  "namespace eval FixPt { namespace export * }\n"
			  "namespace eval Fixpt { namespace import ::FixPt::*; "
			  "    proc Fixpt {} { return [FixPt::FixPt] } }");

  return TCL_OK;
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
