///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:23:55 1999
// written: Tue Sep 28 12:04:43 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_CC_DEFINED
#define TCLVALUE_CC_DEFINED

#include "tclvalue.h"

#include <tcl.h>

#include "trace.h"

//---------------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------------

TclValue::TclValue(Tcl_Interp* interp, Tcl_Obj* obj) :
  itsInterp(interp),
  itsObj(obj)
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, int val) :
  itsInterp(interp),
  itsObj(Tcl_NewIntObj(val))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, long val) :
  itsInterp(interp),
  itsObj(Tcl_NewLongObj(val))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, bool val) :
  itsInterp(interp),
  itsObj(Tcl_NewBooleanObj(val))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, double val) :
  itsInterp(interp),
  itsObj(Tcl_NewDoubleObj(val))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, const char* val) :
  itsInterp(interp),
  itsObj(Tcl_NewStringObj(val, -1))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

TclValue::TclValue(Tcl_Interp* interp, const string& val) :
  itsInterp(interp),
  itsObj(Tcl_NewStringObj(val.c_str(), -1))
{
DOTRACE("TclValue::TclValue");
  Tcl_IncrRefCount(itsObj); 
}

//---------------------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------------------

TclValue::~TclValue() {
DOTRACE("TclValue::~TclValue");
  Tcl_DecrRefCount(itsObj);
}



Value::Type TclValue::getNativeType() {
DOTRACE("TclValue::getNativeType");
  Tcl_ObjType* type = itsObj->typePtr; 

  if (type == 0) return Value::UNKNOWN;

  string type_name(type->name); 

  if (type_name == "int") { return Value::INT; }
  else if (type_name == "double") { return Value::DOUBLE; }
  else if (type_name == "boolean") { return Value::BOOL; }
  else if (type_name == "string") { return Value::CSTRING; }
  else return Value::UNKNOWN;
}

//---------------------------------------------------------------------
//
// Return value getters
//
//---------------------------------------------------------------------

int TclValue::getInt() {
DOTRACE("TclValue::getInt");
  int val;
  get(val);
  return val;
}

long TclValue::getLong() {
DOTRACE("TclValue::getLong");
  long val;
  get(val);
  return val;
}

bool TclValue::getBool() {
DOTRACE("TclValue::getBool");
  int val;
  if ( Tcl_GetBooleanFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 throw ValueError();
  }
  return bool(val);
}

double TclValue::getDouble() {
DOTRACE("TclValue::getDouble");
  double val;
  get(val);
  return val;
}

const char* TclValue::getCstring() {
DOTRACE("TclValue::getCstring");
  return Tcl_GetString(itsObj);
}

string TclValue::getString() {
DOTRACE("TclValue::getString");
  return string(Tcl_GetString(itsObj)); 
}

//---------------------------------------------------------------------
//
// Reference argument getters
//
//---------------------------------------------------------------------

void TclValue::get(int& val) {
DOTRACE("TclValue::get");
  if ( Tcl_GetIntFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 throw ValueError();
  }
}

void TclValue::get(long& val) {
DOTRACE("TclValue::get");
  if ( Tcl_GetLongFromObj(itsInterp, itsObj, &val) != TCL_OK) {
	 throw ValueError();
  }
}

void TclValue::get(bool& val) {
DOTRACE("TclValue::get");
  val = getBool(); 
}

void TclValue::get(double& val) {
DOTRACE("TclValue::get");
  try {
	 if ( Tcl_GetDoubleFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
		throw ValueError();
	 }
  }
  catch (ValueError&) {
	 throw;
  }
}

void TclValue::get(const char*& val) {
DOTRACE("TclValue::get");
  val = Tcl_GetString(itsObj);
}

void TclValue::get(string& val) {
DOTRACE("TclValue::get");
  val = Tcl_GetString(itsObj); 
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
