///////////////////////////////////////////////////////////////////////
//
// tclvalue.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Tue Aug  7 11:52:00 2001
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
  Value(rhs),
  itsObjPtr()
{
DOTRACE("Tcl::TclValue::TclValue(const Value&)");

  rhs.assignTo(*this);
}

Tcl::TclValue::TclValue(const TclValue& rhs) :
  Value(rhs),
  itsObjPtr(rhs.itsObjPtr)
{
DOTRACE("Tcl::TclValue::TclValue");
}

Tcl::TclValue::~TclValue()
{
DOTRACE("Tcl::TclValue::~TclValue");
}

Value* Tcl::TclValue::clone() const
{
DOTRACE("Tcl::TclValue::clone");
  return new TclValue(itsObjPtr);
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

int Tcl::TclValue::get(Util::TypeCue<int> cue) const
{
  return itsObjPtr.as(cue);
}

long Tcl::TclValue::get(Util::TypeCue<long> cue) const
{
  return itsObjPtr.as(cue);
}

bool Tcl::TclValue::get(Util::TypeCue<bool> cue) const
{
  return itsObjPtr.as(cue);
}

double Tcl::TclValue::get(Util::TypeCue<double> cue) const
{
  return itsObjPtr.as(cue);
}

const char* Tcl::TclValue::get(Util::TypeCue<const char*> cue) const
{
  return itsObjPtr.as(cue);
}

void Tcl::TclValue::set(int val)         { itsObjPtr = Tcl::ObjPtr(val); }
void Tcl::TclValue::set(long val)        { itsObjPtr = Tcl::ObjPtr(val); }
void Tcl::TclValue::set(bool val)        { itsObjPtr = Tcl::ObjPtr(val); }
void Tcl::TclValue::set(double val)      { itsObjPtr = Tcl::ObjPtr(val); }
void Tcl::TclValue::set(const char* val) { itsObjPtr = Tcl::ObjPtr(val); }

void Tcl::TclValue::assignTo(Value& other) const
{
DOTRACE("Tcl::TclValue::assignTo");

  string_literal type_name(getNativeTypeName());

  if (type_name.equals("int"))
    {
      other.set(this->get(Util::TypeCue<int>()));
    }
  else if (type_name.equals("double"))
    {
      other.set(this->get(Util::TypeCue<double>()));
    }
  else if (type_name.equals("boolean"))
    {
      other.set(this->get(Util::TypeCue<bool>()));
    }
  else
    {
      other.set(this->get(Util::TypeCue<const char*>()));
    }
}

static const char vcid_tclvalue_cc[] = "$Header$";
#endif // !TCLVALUE_CC_DEFINED
