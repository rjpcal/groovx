///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Tue Aug  7 10:32:33 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_CC_DEFINED
#define TCLVALUE_CC_DEFINED

#include "tcl/tclvalue.h"

#include "tcl/tclconvert.h"

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

Tcl::TclValue::TclValue(Tcl_Obj* obj) :
  itsObj(obj)
{
DOTRACE("Tcl::TclValue::TclValue(Tcl_Obj*)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(int val) :
  itsObj(Tcl::Convert<int>::toTcl(val))
{
DOTRACE("Tcl::TclValue::TclValue(int)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(long val) :
  itsObj(Tcl::Convert<long>::toTcl(val))
{
DOTRACE("Tcl::TclValue::TclValue(long)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(bool val) :
  itsObj(Tcl::Convert<bool>::toTcl(val))
{
DOTRACE("Tcl::TclValue::TclValue(bool)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(double val) :
  itsObj(Tcl::Convert<double>::toTcl(val))
{
DOTRACE("Tcl::TclValue::TclValue(double)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(const char* val) :
  itsObj(Tcl::Convert<const char*>::toTcl(val))
{
DOTRACE("Tcl::TclValue::TclValue(const char*)");
  Tcl_IncrRefCount(itsObj);
}

Tcl::TclValue::TclValue(const Value& rhs) :
  itsObj(0)
{
DOTRACE("Tcl::TclValue::TclValue(const Value&)");

  Value::Type rhs_type = rhs.getNativeType();
  DebugEvalNL(rhs_type);

  switch (rhs_type) {
  case Value::INT:
    itsObj = Tcl::Convert<int>::toTcl(rhs.get(Util::TypeCue<int>()));
    break;
  case Value::LONG:
    itsObj = Tcl::Convert<long>::toTcl(rhs.get(Util::TypeCue<long>()));
    break;
  case Value::BOOL:
    itsObj = Tcl::Convert<bool>::toTcl(rhs.get(Util::TypeCue<bool>()));
    break;
  case Value::DOUBLE:
    itsObj = Tcl::Convert<double>::toTcl(rhs.get(Util::TypeCue<double>()));
    break;

  case Value::CSTRING:
  case Value::NONE:
  case Value::UNKNOWN:
  default:
    itsObj = Tcl::Convert<const char*>::toTcl(rhs.get(Util::TypeCue<const char*>()));
    break;
  }

  DebugEval((void*) itsObj);
  DebugEvalNL(itsObj->refCount);

  Tcl_IncrRefCount(itsObj);

  DebugEvalNL(itsObj->refCount);
}

Tcl::TclValue::TclValue(const TclValue& rhs) :
  Value(rhs),
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
  return new TclValue(itsObj);
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

void Tcl::TclValue::printTo(STD_IO::ostream& os) const {
DOTRACE("Tcl::TclValue::printTo");
  os << Tcl_GetString(itsObj);
}

void Tcl::TclValue::scanFrom(STD_IO::istream& is) {
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

int Tcl::TclValue::get(Util::TypeCue<int>) const {
DOTRACE("Tcl::TclValue::get(Util::TypeCue<int>)");
  return Tcl::Convert<int>::fromTcl(itsObj);
}

long Tcl::TclValue::get(Util::TypeCue<long>) const {
DOTRACE("Tcl::TclValue::get(Util::TypeCue<long>)");
  return Tcl::Convert<long>::fromTcl(itsObj);
}

bool Tcl::TclValue::get(Util::TypeCue<bool>) const {
DOTRACE("Tcl::TclValue::get(Util::TypeCue<bool>)");
  return Tcl::Convert<bool>::fromTcl(itsObj);
}

double Tcl::TclValue::get(Util::TypeCue<double>) const {
DOTRACE("Tcl::TclValue::get(Util::TypeCue<double>)");
  return Tcl::Convert<double>::fromTcl(itsObj);
}

const char* Tcl::TclValue::get(Util::TypeCue<const char*>) const {
DOTRACE("Tcl::TclValue::get(Util::TypeCue<const char*>)");
  return Tcl::Convert<const char*>::fromTcl(itsObj);
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
