///////////////////////////////////////////////////////////////////////
//
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Wed Dec 15 17:52:55 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "poslist.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"

//---------------------------------------------------------------------
//
// PoslistTcl::Poslist_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Poslist_Init;

int Poslist_Init(Tcl_Interp* interp) {
DOTRACE("Poslist_Init");

  new Tcl::IoPtrListPkg(interp, PosList::thePosList(),
								"PosList", "$Revision$");

  return TCL_OK;
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
