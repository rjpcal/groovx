///////////////////////////////////////////////////////////////////////
// tlisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:38:37 1999
// written: Sat Mar 13 13:23:37 1999
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "tlisttcl.h"

#include <tcl.h>

#include "tlist.h"
#include "objlisttcl.h"
#include "poslisttcl.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Tlist Tcl package
///////////////////////////////////////////////////////////////////////

namespace TlistTcl {
  Tlist *theTlist = NULL;
}

int TlistTcl::Tlist_Init(Tcl_Interp *interp) {
DOTRACE("TlistTcl::Tlist_Init");
  TlistTcl::theTlist = new Tlist(*ObjlistTcl::getObjList(),
											*PoslistTcl::getPosList());
  return TCL_OK;
}

#endif // !TLISTTCL_CC_DEFINED
