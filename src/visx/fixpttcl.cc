///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
// Rob Peters
// created: Jan-99
// written: Tue Oct 19 10:42:44 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include <tcl.h>

#include "fixpt.h"
#include "iomgr.h"
#include "objlist.h"
#include "propitempkg.h"

#define NO_TRACE
#include "trace.h"


extern "C" Tcl_PackageInitProc Fixpt_Init;

int Fixpt_Init(Tcl_Interp* interp) {
DOTRACE("Fixpt_Init");

  new PropertyListItemPkg<FixPt, ObjList>(interp, ObjList::theObjList(),
 														"Fixpt", "$Revision$");

  FactoryRegistrar<IO, FixPt> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
