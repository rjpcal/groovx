///////////////////////////////////////////////////////////////////////
//
// tclutil.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 11 10:27:35 2000
// written: Wed Oct 11 17:02:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_CC_DEFINED
#define TCLUTIL_CC_DEFINED

#include "tcl/tclutil.h"

#include <tcl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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
// Tcl::SafeInterface member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::SafeInterface::SafeInterface(Tcl_Interp* interp) :
  itsInterp(interp)
{}

Tcl::SafeInterface::~SafeInterface() {}

void Tcl::SafeInterface::handleError(const char* msg) const {
DOTRACE("Tcl::SafeInterface::handleError");

  throw TclError(msg);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::SafeInterp::SafeInterp(Tcl_Interp* interp) :
  Tcl::SafeInterface(interp)
{
DOTRACE("Tcl::SafeInterp::SafeInterp");
  Invariant(itsInterp != 0);
}

Tcl::SafeInterp::~SafeInterp() {}

///////////////////////////////////////////////////////////////////////
//
// Expressions
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::evalBooleanExpr(Tcl_Obj* obj) const {
DOTRACE("Tcl::SafeInterp::evalBooleanExpr");

  int expr_result;

  if (Tcl_ExprBooleanObj(itsInterp, obj, &expr_result) != TCL_OK)
	 {
		handleError("error evaluating boolean expression");
	 }

  return bool(expr_result);
}

///////////////////////////////////////////////////////////////////////
//
// Interpreter
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::interpDeleted() const {
DOTRACE("Tcl::SafeInterp::interpDeleted");

  Invariant(itsInterp != 0); 
  return bool(Tcl_InterpDeleted(itsInterp));
}

///////////////////////////////////////////////////////////////////////
//
// Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::SafeInterp::appendResult(const char* msg) const {
DOTRACE("Tcl::SafeInterp::appendResult");

  Tcl_AppendResult(itsInterp, msg);
}

static const char vcid_tclutil_cc[] = "$Header$";
#endif // !TCLUTIL_CC_DEFINED
