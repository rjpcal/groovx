///////////////////////////////////////////////////////////////////////
//
// tclutil.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 11 10:27:35 2000
// written: Wed Oct 11 15:49:09 2000
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

Tcl::SafeInterface::SafeInterface(Tcl_Interp* interp, Tcl::SafeInterface::ErrMode mode) :
  itsErrHandler(0),
  itsLastOpSucceeded(true),
  itsInterp(interp)
{
  if (itsInterp == 0 && mode == BKD_ERROR) { mode = IGNORE; }

  if (mode == IGNORE)
	 itsErrHandler = new Util::NullErrorHandler;
  else if (mode == BKD_ERROR)
	 itsErrHandler = new Tcl::BkdErrorHandler(itsInterp);
  else if (mode == THROW)
	 itsErrHandler = 0;
}

Tcl::SafeInterface::~SafeInterface() {
  delete itsErrHandler;
}

void Tcl::SafeInterface::swap(Tcl::SafeInterface& other) {
DOTRACE("Tcl::SafeInterface::swap");

  Tcl_Interp* otherInterp = other.itsInterp; 
  other.itsInterp = this->itsInterp;
  this->itsInterp = otherInterp;

  Util::ErrorHandler* otherErrHandler = other.itsErrHandler; 
  other.itsErrHandler = this->itsErrHandler;
  this->itsErrHandler = otherErrHandler;

  bool otherLastResult = other.itsLastOpSucceeded; 
  other.itsLastOpSucceeded = this->itsLastOpSucceeded;
  this->itsLastOpSucceeded = otherLastResult;
}

void Tcl::SafeInterface::handleError(const char* msg) const {
DOTRACE("Tcl::SafeInterface::handleError");

  itsLastOpSucceeded = false;

  if (itsErrHandler != 0) {
	 itsErrHandler->handleMsg(msg);
  }
  else {
	 throw TclError(msg);
  }
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::SafeInterp member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::SafeInterp::SafeInterp(Tcl_Interp* interp, ErrMode mode) :
  Tcl::SafeInterface(interp, mode)
{
DOTRACE("Tcl::SafeInterp::SafeInterp");
  Invariant(itsInterp != 0);
}

Tcl::SafeInterp::~SafeInterp() {}

void Tcl::SafeInterp::reset(Tcl_Interp* interp,
									 Tcl::SafeInterface::ErrMode mode) {
DOTRACE("Tcl::SafeInterp::reset");

  Precondition(interp != 0);

  SafeInterface new_interp(interp, mode);
  this->swap(new_interp);

  Invariant(itsInterp != 0);
}

///////////////////////////////////////////////////////////////////////
//
// Expressions
//
///////////////////////////////////////////////////////////////////////

bool Tcl::SafeInterp::evalBooleanExpr(Tcl_Obj* obj) const {
DOTRACE("Tcl::SafeInterp::evalBooleanExpr");

  resetSuccessFlag();

  int expr_result;

  if (Tcl_ExprBooleanObj(itsInterp, obj, &expr_result) != TCL_OK)
	 {
		handleError("error evaluating boolean expression");
		return false;
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

  resetSuccessFlag();
  return (itsInterp != 0) && Tcl_InterpDeleted(itsInterp);
}

///////////////////////////////////////////////////////////////////////
//
// Result
//
///////////////////////////////////////////////////////////////////////

void Tcl::SafeInterp::appendResult(const char* msg) const {
DOTRACE("Tcl::SafeInterp::appendResult");

  resetSuccessFlag();
  Tcl_AppendResult(itsInterp, msg);
}

static const char vcid_tclutil_cc[] = "$Header$";
#endif // !TCLUTIL_CC_DEFINED
