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
// Tcl::SafeInterface -- Ints
//
///////////////////////////////////////////////////////////////////////

int Tcl::SafeInterface::getInt(Tcl_Obj* intObj) const throw(Tcl::TclError) {
DOTRACE("Tcl::SafeInterface::getInt");

  int return_val=-1;
  // OK if interp is 0
  if (Tcl_GetIntFromObj(itsInterp, intObj, &return_val) != TCL_OK) {
	 handleError("error getting int from Tcl_Obj");
  }
  return return_val;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterface -- Lists
//
///////////////////////////////////////////////////////////////////////

int Tcl::SafeInterface::listLength(Tcl_Obj* tcllist)
  const throw(Tcl::TclError) {
DOTRACE("Tcl::SafeInterface::listLength");

  int length;
  // OK if itsInterp is 0
  if (Tcl_ListObjLength(itsInterp, tcllist, &length) != TCL_OK) {
	 handleError("error getting list length");
  }
  return length;
}

Tcl_Obj* Tcl::SafeInterface::listElement(Tcl_Obj* tcllist,
													  int index) const throw(Tcl::TclError) {
DOTRACE("Tcl::SafeInterface::listElement");

  Tcl_Obj* element = 0;
  // OK if interp is 0
  if (Tcl_ListObjIndex(itsInterp, tcllist, index, &element) != TCL_OK) {
	 handleError("error getting list element");
  }

  Postcondition(element != 0);
  return element;
}

void Tcl::SafeInterface::splitList(Tcl_Obj* tcllist, Tcl_Obj**& elements_out,
											  int& length_out) const throw(Tcl::TclError) {
DOTRACE("Tcl::SafeInterface::splitList");

  // OK if itsInterp is 0
  if ( Tcl_ListObjGetElements(itsInterp, tcllist,
										&length_out, &elements_out) != TCL_OK ) {
	 handleError("error splitting list");
  }
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
// Tcl::SafeInterp -- Expressions
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
// Tcl::SafeInterp -- Interpreter
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::interpDeleted() const {
DOTRACE("Tcl::SafeInterp::interpDeleted");

  Invariant(itsInterp != 0); 
  return bool(Tcl_InterpDeleted(itsInterp));
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp -- Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::SafeInterp::appendResult(const char* msg) const {
DOTRACE("Tcl::SafeInterp::appendResult");

  Tcl_AppendResult(itsInterp, msg);
}

static const char vcid_tclutil_cc[] = "$Header$";
#endif // !TCLUTIL_CC_DEFINED
