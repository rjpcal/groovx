///////////////////////////////////////////////////////////////////////
//
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Mon Oct 30 11:40:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "position.h"

#include "tcl/listpkg.h"

#define NO_TRACE
#include "util/trace.h"

//---------------------------------------------------------------------
//
// PoslistTcl::Poslist_Init --
//
//---------------------------------------------------------------------

extern "C"
int Poslist_Init(Tcl_Interp* interp) {
DOTRACE("Poslist_Init");

  Tcl::TclPkg* pkg = 
	 new Tcl::PtrListPkg<Position>(interp, "PosList", "$Revision$");

  return pkg->initStatus();
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
