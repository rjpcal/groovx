///////////////////////////////////////////////////////////////////////
//
// tclutil.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 11 10:27:35 2000
// written: Wed Oct 11 11:27:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_CC_DEFINED
#define TCLUTIL_CC_DEFINED

#include "tcl/tclutil.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"

// OK if interp is 0
int Tcl::Safe::listLength(Tcl_Interp* interp, Tcl_Obj* tcllist)
  throw(Tcl::TclError) {
DOTRACE("Tcl::Safe::listLength");
  int length;
  if (Tcl_ListObjLength(interp, tcllist, &length) != TCL_OK) {
	 throw TclError("error getting list length");
  }
  return length;
}

// OK if interp is 0
Tcl_Obj* Tcl::Safe::listElement(Tcl_Interp* interp, Tcl_Obj* tcllist,
										  int index) throw(Tcl::TclError) {
DOTRACE("Tcl::Safe::listElement");

  Tcl_Obj* element = NULL;
  if (Tcl_ListObjIndex(interp, tcllist, index, &element) != TCL_OK) {
	 throw TclError("error getting list element");
  }
  return element;
}

// OK if interp is 0
void Tcl::Safe::splitList(Tcl_Interp* interp, Tcl_Obj* tcllist,
				 Tcl_Obj**& elements_out, int& length_out) throw(Tcl::TclError) {
DOTRACE("Tcl::Safe::splitList");

  if ( Tcl_ListObjGetElements(interp, tcllist,
										&length_out, &elements_out) != TCL_OK ) {
	 throw TclError("error splitting list");
  }
}

// OK if interp is 0
int Tcl::Safe::getInt(Tcl_Interp* interp, Tcl_Obj* intObj)
  throw(Tcl::TclError) {
DOTRACE("Tcl::Safe::getInt");
  int return_val=-1;
  if (Tcl_GetIntFromObj(interp, intObj, &return_val) != TCL_OK) {
	 throw TclError("error getting int from Tcl_Obj");
  }
  return return_val;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::SafeInterp::SafeInterp(Tcl_Interp* interp, Tcl::SafeInterp::ErrMode mode) :
  itsInterp(interp),
  itsErrHandler(0),
  itsLastResult(TCL_OK)
{
  if (itsInterp == 0 && mode == BKD_ERROR) { mode = IGNORE; }

  if (mode == IGNORE)
	 itsErrHandler = new Util::NullErrorHandler;
  else if (mode == BKD_ERROR)
	 itsErrHandler = new Tcl::BkdErrorHandler(itsInterp);
  else if (mode == THROW)
	 itsErrHandler = 0;
}

Tcl::SafeInterp::~SafeInterp() {
  delete itsErrHandler;
}

void Tcl::SafeInterp::handleError(const char* msg, int result) {
DOTRACE("Tcl::SafeInterp::handleError");

  itsLastResult = result;

  if (itsErrHandler != 0) {
	 itsErrHandler->handleMsg(msg);
  }
  else {
	 throw TclError(msg);
  }
}

static const char vcid_tclutil_cc[] = "$Header$";
#endif // !TCLUTIL_CC_DEFINED
