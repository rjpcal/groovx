///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:23:55 1999
// written: Wed Mar 15 20:30:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_CC_DEFINED
#define TCLVALUE_CC_DEFINED

#include "tcl/tclvalue.h"

#include "util/strings.h"

#include <tcl.h>
#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

//---------------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------------

Tcl::TclValue::TclValue(Tcl_Interp* interp, Tcl_Obj* obj) :
  itsInterp(interp),
  itsObj(obj)
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, Tcl_Obj*)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, int val) :
  itsInterp(interp),
  itsObj(Tcl_NewIntObj(val))
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, int)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, long val) :
  itsInterp(interp),
  itsObj(Tcl_NewLongObj(val))
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, long)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, bool val) :
  itsInterp(interp),
  itsObj(Tcl_NewBooleanObj(val))
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, bool)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, double val) :
  itsInterp(interp),
  itsObj(Tcl_NewDoubleObj(val))
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, double)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, const char* val) :
  itsInterp(interp),
  itsObj(Tcl_NewStringObj(val, -1))
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, const char*)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(Tcl_Interp* interp, const Value& rhs) :
  itsInterp(interp),
  itsObj(0)
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Interp*, const Value&)");

  Value::Type rhs_type = rhs.getNativeType();
  DebugEvalNL(rhs_type);

  switch (rhs_type) {
  case Value::INT:
	 itsObj = Tcl_NewIntObj(rhs.getInt());
	 break;
  case Value::LONG:
	 itsObj = Tcl_NewLongObj(rhs.getLong());
	 break;
  case Value::BOOL:
	 itsObj = Tcl_NewBooleanObj(rhs.getBool());
	 break;
  case Value::DOUBLE:
	 itsObj = Tcl_NewDoubleObj(rhs.getDouble());
	 break;
  case Value::CSTRING:
	 itsObj = Tcl_NewStringObj(rhs.getCstring(), -1);
	 break;

  case Value::NONE:
  case Value::UNKNOWN:
  default:
	 itsObj = Tcl_NewStringObj(rhs.getCstring(), -1);
	 break;
  }

  DebugEval((void*) itsObj);
  DebugEvalNL(itsObj->refCount);

  Tcl_IncrRefCount(itsObj);

  DebugEvalNL(itsObj->refCount);
}

Tcl::TclValue::TclValue(const TclValue& rhs) :
  Value(rhs),
  itsInterp(rhs.itsInterp),
  itsObj(rhs.itsObj)
{
DOTRACE("Tcl::TclValue::TclValue");
  Tcl_IncrRefCount(itsObj);
}

//---------------------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------------------

Tcl::TclValue::~TclValue() {
DOTRACE("Tcl::TclValue::~TclValue");
  Tcl_DecrRefCount(itsObj);
}

Tcl_Obj* Tcl::TclValue::getObj() const {
DOTRACE("Tcl::TclValue::getObj");
  return itsObj;
}

void Tcl::TclValue::setObj(Tcl_Obj* obj) {
DOTRACE("Tcl::TclValue::setObj");
  if (itsObj == obj) return; 

  Tcl_DecrRefCount(itsObj);

  itsObj = obj;
  Tcl_IncrRefCount(itsObj);
}

Value* Tcl::TclValue::clone() const {
DOTRACE("Tcl::TclValue::clone");
  return new TclValue(itsInterp, itsObj);
}

Value::Type Tcl::TclValue::getNativeType() const {
DOTRACE("Tcl::TclValue::getNativeType");

  string_literal type_name(getNativeTypeName());

  if (type_name.equals("int")) { return Value::INT; }
  else if (type_name.equals("double")) { return Value::DOUBLE; }
  else if (type_name.equals("boolean")) { return Value::BOOL; }
  else if (type_name.equals("string")) { return Value::CSTRING; }
  else return Value::UNKNOWN;
}

const char* Tcl::TclValue::getNativeTypeName() const {
DOTRACE("Tcl::TclValue::getNativeTypeName");
  Tcl_ObjType* type = itsObj->typePtr;

  if (type == 0) return "cstring";

  DebugEvalNL(type->name);

  return type->name;
}

void Tcl::TclValue::printTo(ostream& os) const {
DOTRACE("Tcl::TclValue::printTo");
  os << Tcl_GetString(itsObj);
}

void Tcl::TclValue::scanFrom(istream& is) {
DOTRACE("Tcl::TclValue::scanFrom");
  char buf[256]; 
  is >> ws;
  is.get(buf, 256, '\n');
  Tcl_SetStringObj(itsObj, buf, -1);
}

//---------------------------------------------------------------------
//
// Return value getters
//
//---------------------------------------------------------------------

int Tcl::TclValue::getInt() const {
DOTRACE("Tcl::TclValue::getInt");
  int val;
  get(val);
  return val;
}

long Tcl::TclValue::getLong() const {
DOTRACE("Tcl::TclValue::getLong");
  long val;
  get(val);
  return val;
}

bool Tcl::TclValue::getBool() const {
DOTRACE("Tcl::TclValue::getBool");
  int val;
  if ( Tcl_GetBooleanFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 fixed_string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg.c_str());
  }
  return bool(val);
}

double Tcl::TclValue::getDouble() const {
DOTRACE("Tcl::TclValue::getDouble");
  double val;
  get(val);
  return val;
}

const char* Tcl::TclValue::getCstring() const {
DOTRACE("Tcl::TclValue::getCstring");
  return Tcl_GetString(itsObj);
}

//---------------------------------------------------------------------
//
// Reference argument getters
//
//---------------------------------------------------------------------

void Tcl::TclValue::get(int& val) const {
DOTRACE("Tcl::TclValue::get");
  if ( Tcl_GetIntFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 fixed_string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg.c_str());
  }
}

void Tcl::TclValue::get(long& val) const {
DOTRACE("Tcl::TclValue::get");
  if ( Tcl_GetLongFromObj(itsInterp, itsObj, &val) != TCL_OK) {
	 fixed_string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg.c_str());
  }
}

void Tcl::TclValue::get(bool& val) const {
DOTRACE("Tcl::TclValue::get");
  val = getBool();
}

void Tcl::TclValue::get(double& val) const {
DOTRACE("Tcl::TclValue::get");
  try {
	 if ( Tcl_GetDoubleFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
		fixed_string msg = Tcl_GetStringResult(itsInterp);
		Tcl_ResetResult(itsInterp);
		throw ValueError(msg.c_str());
	 }
  }
  catch (ValueError&) {
	 throw;
  }
}

void Tcl::TclValue::get(const char*& val) const {
DOTRACE("Tcl::TclValue::get");
  val = Tcl_GetString(itsObj);
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
