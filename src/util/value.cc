///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Sep 28 11:21:32 1999
// written: Wed Mar 19 12:45:34 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "util/value.h"

#include "util/cstrstream.h"
#include "util/strings.h"

#include <sstream>

#include "util/trace.h"

Value::Value()
{
DOTRACE("Value::Value");
}

Value::~Value()
{
DOTRACE("Value::~Value");
}

const char* Value::getCstring() const
{
DOTRACE("Value::getCstring");

  static std::ostringstream oss;
  oss.str(std::string());
  printTo(oss);
  oss << '\0';
  return oss.str().c_str();
}

fstring Value::getFstring() const
{
  return fstring(getCstring());
}

void Value::setCstring(const char* str)
{
DOTRACE("Value::setCstring");
  Util::icstrstream ist(str);
  scanFrom(ist);
}

void Value::setFstring(fstring val)
{
  setCstring(val.c_str());
}

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
