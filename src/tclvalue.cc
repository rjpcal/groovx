///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Tue Aug  7 11:18:33 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_CC_DEFINED
#define TCLVALUE_CC_DEFINED

#include "tcl/tclvalue.h"

#include "util/strings.h"

#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

//---------------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------------

Tcl::TclValue::TclValue(const Value& rhs) :
  itsObjPtr()
{
DOTRACE("Tcl::TclValue::TclValue(const Value&)");

  Value::Type rhs_type = rhs.getNativeType();
  DebugEvalNL(rhs_type);

  switch (rhs_type)
    {
    case Value::INT:
      itsObjPtr = Tcl::Convert<int>::toTcl(rhs.get(Util::TypeCue<int>()));
      break;
    case Value::LONG:
      itsObjPtr = Tcl::Convert<long>::toTcl(rhs.get(Util::TypeCue<long>()));
      break;
    case Value::BOOL:
      itsObjPtr = Tcl::Convert<bool>::toTcl(rhs.get(Util::TypeCue<bool>()));
      break;
    case Value::DOUBLE:
      itsObjPtr = Tcl::Convert<double>::toTcl(rhs.get(Util::TypeCue<double>()));
      break;

    case Value::CSTRING:
    case Value::NONE:
    case Value::UNKNOWN:
    default:
      itsObjPtr = Tcl::Convert<const char*>::toTcl(rhs.get(Util::TypeCue<const char*>()));
      break;
    }
}

Tcl::TclValue::TclValue(const TclValue& rhs) :
  Value(rhs),
  itsObjPtr(rhs.itsObjPtr)
{
DOTRACE("Tcl::TclValue::TclValue");
}

//---------------------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------------------

Tcl::TclValue::~TclValue()
{
DOTRACE("Tcl::TclValue::~TclValue");
}

Tcl_Obj* Tcl::TclValue::getObj() const
{
DOTRACE("Tcl::TclValue::getObj");
  return itsObjPtr;
}

void Tcl::TclValue::setObj(Tcl_Obj* obj)
{
DOTRACE("Tcl::TclValue::setObj");
  itsObjPtr = obj;
}

Value* Tcl::TclValue::clone() const
{
DOTRACE("Tcl::TclValue::clone");
  return new TclValue(itsObjPtr);
}

Value::Type Tcl::TclValue::getNativeType() const
{
DOTRACE("Tcl::TclValue::getNativeType");

  string_literal type_name(getNativeTypeName());

  if (type_name.equals("int")) { return Value::INT; }
  else if (type_name.equals("double")) { return Value::DOUBLE; }
  else if (type_name.equals("boolean")) { return Value::BOOL; }
  else if (type_name.equals("string")) { return Value::CSTRING; }
  else return Value::UNKNOWN;
}

const char* Tcl::TclValue::getNativeTypeName() const
{
DOTRACE("Tcl::TclValue::getNativeTypeName");
  return itsObjPtr.typeName();
}

void Tcl::TclValue::printTo(STD_IO::ostream& os) const
{
DOTRACE("Tcl::TclValue::printTo");
  os << get(Util::TypeCue<const char*>());
}

void Tcl::TclValue::scanFrom(STD_IO::istream& is)
{
DOTRACE("Tcl::TclValue::scanFrom");
  char buf[256];
  is >> ws;
  is.get(buf, 256, '\n');
  itsObjPtr = Tcl::ObjPtr(buf);
}

//---------------------------------------------------------------------
//
// Return value getters
//
//---------------------------------------------------------------------

int Tcl::TclValue::get(Util::TypeCue<int>) const
{
DOTRACE("Tcl::TclValue::get(Util::TypeCue<int>)");
  return Tcl::Convert<int>::fromTcl(itsObjPtr);
}

long Tcl::TclValue::get(Util::TypeCue<long>) const
{
DOTRACE("Tcl::TclValue::get(Util::TypeCue<long>)");
  return Tcl::Convert<long>::fromTcl(itsObjPtr);
}

bool Tcl::TclValue::get(Util::TypeCue<bool>) const
{
DOTRACE("Tcl::TclValue::get(Util::TypeCue<bool>)");
  return Tcl::Convert<bool>::fromTcl(itsObjPtr);
}

double Tcl::TclValue::get(Util::TypeCue<double>) const
{
DOTRACE("Tcl::TclValue::get(Util::TypeCue<double>)");
  return Tcl::Convert<double>::fromTcl(itsObjPtr);
}

const char* Tcl::TclValue::get(Util::TypeCue<const char*>) const
{
DOTRACE("Tcl::TclValue::get(Util::TypeCue<const char*>)");
  return Tcl::Convert<const char*>::fromTcl(itsObjPtr);
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
