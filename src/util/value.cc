///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:21:32 1999
// written: Mon Sep  3 13:41:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "util/value.h"

#include "util/strings.h"

#include <iostream.h>

#include "util/trace.h"

namespace VALUE_CC_LOCAL
{
  void raiseValueError(const char* msg ="") { throw ValueError(msg); }
}

using namespace VALUE_CC_LOCAL;

ValueError::~ValueError() {}

Value::Value()
{
DOTRACE("Value::Value");
}

Value::~Value()
{
DOTRACE("Value::~Value");
}


void Value::printTo(STD_IO::ostream&) const { raiseValueError("printTo"); }
void Value::scanFrom(STD_IO::istream&) { raiseValueError("scanFrom"); }

int Value::get(Util::TypeCue<int>) const { raiseValueError("getInt"); return 0; }
long Value::get(Util::TypeCue<long>) const { raiseValueError("getLong"); return 0; }
bool Value::get(Util::TypeCue<bool>) const { raiseValueError("getBool"); return false; }
double Value::get(Util::TypeCue<double>) const { raiseValueError("getDouble"); return 0.0; }
const char* Value::get(Util::TypeCue<const char*>) const { raiseValueError("getCstring"); return ""; }

fstring Value::get(Util::TypeCue<fstring>) const
{
  return fstring(get(Util::TypeCue<const char*>()));
}

void Value::set(int) { raiseValueError("set(int)"); }
void Value::set(long) { raiseValueError("set(long)"); }
void Value::set(bool) { raiseValueError("set(bool)"); }
void Value::set(double) { raiseValueError("set(double)"); }
void Value::set(const char*) { raiseValueError("set(const char*)"); }

void Value::set(fstring val)
{
  set(val.c_str());
}

void Value::assignFrom(const Value& other) { other.assignTo(*this); }

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
