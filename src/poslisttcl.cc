///////////////////////////////////////////////////////////////////////
//
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Wed Dec 15 17:43:40 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include <tcl.h>

#include "poslist.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Poslist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace PoslistTcl {
  class PosListPkg;
}

///////////////////////////////////////////////////////////////////////
//
// PosListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class PoslistTcl::PosListPkg : public Tcl::IoPtrListPkg {
public:
  PosListPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, PosList::thePosList(), "PosList", "3.0") {}
};

//---------------------------------------------------------------------
//
// PoslistTcl::Poslist_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Poslist_Init;

int Poslist_Init(Tcl_Interp* interp) {
DOTRACE("Poslist_Init");

  new PoslistTcl::PosListPkg(interp);

  return TCL_OK;
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
