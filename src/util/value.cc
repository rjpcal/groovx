///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:21:32 1999
// written: Fri Jan 18 16:07:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "util/value.h"

#include "util/strings.h"

#include <strstream.h>

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
  static char buf[512];
  ostrstream ost(buf, 256);
  printTo(ost);
  ost << '\0';
  return buf;
}

fstring Value::getFstring() const
{
  return fstring(getCstring());
}

void Value::setCstring(const char* str)
{
DOTRACE("Value::setCstring");
  istrstream ist(str);
  scanFrom(ist);
}

void Value::setFstring(fstring val)
{
  setCstring(val.c_str());
}

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
