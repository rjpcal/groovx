///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Sep 28 11:21:32 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "util/value.h"

#include "util/cstrstream.h"
#include "util/strings.h"

#include <sstream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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

  static std::string result;

  std::ostringstream oss;
  printTo(oss);

  result = oss.str();

  return result.c_str();
}

fstring Value::getFstring() const
{
  return fstring(getCstring());
}

void Value::setCstring(const char* str)
{
DOTRACE("Value::setCstring");
  rutz::icstrstream ist(str);
  scanFrom(ist);
}

void Value::setFstring(fstring val)
{
  setCstring(val.c_str());
}

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
