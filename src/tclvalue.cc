///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:23:55 1999
// written: Tue Nov 16 12:12:40 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_CC_DEFINED
#define TCLVALUE_CC_DEFINED

#include "tclvalue.h"

#include <tcl.h>

#define NO_TRACE
#include "trace.h"
#include "debug.h"

//---------------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------------

TclValue::TclValue(Tcl_Interp* interp, Tcl_Obj* obj) :
  itsInterp(interp),
  itsObj(obj)
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, Tcl_Obj*)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, int val) :
  itsInterp(interp),
  itsObj(Tcl_NewIntObj(val))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, int)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, long val) :
  itsInterp(interp),
  itsObj(Tcl_NewLongObj(val))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, long)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, bool val) :
  itsInterp(interp),
  itsObj(Tcl_NewBooleanObj(val))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, bool)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, double val) :
  itsInterp(interp),
  itsObj(Tcl_NewDoubleObj(val))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, double)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, const char* val) :
  itsInterp(interp),
  itsObj(Tcl_NewStringObj(val, -1))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, const char*)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, const string& val) :
  itsInterp(interp),
  itsObj(Tcl_NewStringObj(val.c_str(), -1))
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, const string&)");
  Tcl_IncrRefCount(itsObj);
}

TclValue::TclValue(Tcl_Interp* interp, const Value& rhs) :
  itsInterp(interp)
{
DOTRACE("TclValue::TclValue(Tcl_Interp*, const Value&)");

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
  case Value::STRING:
	 itsObj = Tcl_NewStringObj(rhs.getString().c_str(), -1);
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

TclValue::TclValue(const TclValue& rhs) :
  itsInterp(rhs.itsInterp),
  itsObj(rhs.itsObj)
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

Tcl_Obj* TclValue::getObj() const {
DOTRACE("TclValue::getObj");
  return itsObj;
}

void TclValue::setObj(Tcl_Obj* obj) {
DOTRACE("TclValue::setObj");
  Tcl_DecrRefCount(itsObj);

  itsObj = obj;
  Tcl_IncrRefCount(itsObj);
}

Value* TclValue::clone() const {
DOTRACE("TclValue::clone");
  return new TclValue(itsInterp, itsObj);
}

Value::Type TclValue::getNativeType() const {
DOTRACE("TclValue::getNativeType");

  string type_name(getNativeTypeName());

  if (type_name == "int") { return Value::INT; }
  else if (type_name == "double") { return Value::DOUBLE; }
  else if (type_name == "boolean") { return Value::BOOL; }
  else if (type_name == "string") { return Value::CSTRING; }
  else return Value::UNKNOWN;
}

const char* TclValue::getNativeTypeName() const {
DOTRACE("TclValue::getNativeTypeName");
  Tcl_ObjType* type = itsObj->typePtr;

  if (type == 0) return "cstring";

  DebugEvalNL(type->name);

  return type->name;
}

void TclValue::printTo(ostream& os) const {
DOTRACE("TclValue::printTo");
  os << Tcl_GetString(itsObj);
}

void TclValue::scanFrom(istream& is) {
DOTRACE("TclValue::scanFrom");
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

int TclValue::getInt() const {
DOTRACE("TclValue::getInt");
  int val;
  get(val);
  return val;
}

long TclValue::getLong() const {
DOTRACE("TclValue::getLong");
  long val;
  get(val);
  return val;
}

bool TclValue::getBool() const {
DOTRACE("TclValue::getBool");
  int val;
  if ( Tcl_GetBooleanFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg);
  }
  return bool(val);
}

double TclValue::getDouble() const {
DOTRACE("TclValue::getDouble");
  double val;
  get(val);
  return val;
}

const char* TclValue::getCstring() const {
DOTRACE("TclValue::getCstring");
  return Tcl_GetString(itsObj);
}

string TclValue::getString() const {
DOTRACE("TclValue::getString");
  return string(Tcl_GetString(itsObj));
}

//---------------------------------------------------------------------
//
// Reference argument getters
//
//---------------------------------------------------------------------

void TclValue::get(int& val) const {
DOTRACE("TclValue::get");
  if ( Tcl_GetIntFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
	 string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg);
  }
}

void TclValue::get(long& val) const {
DOTRACE("TclValue::get");
  if ( Tcl_GetLongFromObj(itsInterp, itsObj, &val) != TCL_OK) {
	 string msg = Tcl_GetStringResult(itsInterp);
	 Tcl_ResetResult(itsInterp);
	 throw ValueError(msg);
  }
}

void TclValue::get(bool& val) const {
DOTRACE("TclValue::get");
  val = getBool();
}

void TclValue::get(double& val) const {
DOTRACE("TclValue::get");
  try {
	 if ( Tcl_GetDoubleFromObj(itsInterp, itsObj, &val) != TCL_OK ) {
		string msg = Tcl_GetStringResult(itsInterp);
		Tcl_ResetResult(itsInterp);
		throw ValueError(msg);
	 }
  }
  catch (ValueError&) {
	 throw;
  }
}

void TclValue::get(const char*& val) const {
DOTRACE("TclValue::get");
  val = Tcl_GetString(itsObj);
}

void TclValue::get(string& val) const {
DOTRACE("TclValue::get");
  val = Tcl_GetString(itsObj);
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
