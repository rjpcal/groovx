///////////////////////////////////////////////////////////////////////
//
// tclconvert.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jul 11 08:58:53 2001
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

#ifndef TCLCONVERT_CC_DEFINED
#define TCLCONVERT_CC_DEFINED

#include "tcl/tclconvert.h"

#include "tcl/tcldictobj.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclobjptr.h"

#include "util/error.h"
#include "util/strings.h"
#include "util/value.h"

#include <limits>
#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class SafeUnshared
  {
  private:
    Tcl_Obj* itsObj;
    bool isItOwning;

    SafeUnshared(const SafeUnshared&);
    SafeUnshared& operator=(const SafeUnshared&);

  public:
    SafeUnshared(Tcl_Obj* obj, Tcl_ObjType* target_type) :
      itsObj(obj), isItOwning(false)
    {
      if ( (itsObj->typePtr != target_type) && Tcl_IsShared(itsObj) )
        {
          isItOwning = true;
          itsObj = Tcl_DuplicateObj(itsObj);
          Tcl_IncrRefCount(itsObj);
        }
    }

    Tcl_Obj* get() const { return itsObj; }

    ~SafeUnshared()
    {
      if (isItOwning)
        Tcl_DecrRefCount(itsObj);
    }
  };
}

///////////////////////////////////////////////////////////////////////
//
// (Tcl --> C++) fromTcl specializations
//
///////////////////////////////////////////////////////////////////////

Tcl_Obj* Tcl::fromTcl(Tcl_Obj* obj, Tcl_Obj**)
{ return obj; }

Tcl::ObjPtr Tcl::fromTcl(Tcl_Obj* obj, Tcl::ObjPtr*)
{ return obj; }

int Tcl::fromTcl(Tcl_Obj* obj, int*)
{
DOTRACE("Tcl::fromTcl(int*)");

  int val;

  static Tcl_ObjType* const intType = Tcl_GetObjType("int");

  ASSERT(intType != 0);

  SafeUnshared safeobj(obj, intType);

  if ( Tcl_GetIntFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(fstring("expected integer but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }

  return val;
}

unsigned int Tcl::fromTcl(Tcl_Obj* obj, unsigned int*)
{
DOTRACE("Tcl::fromTcl(unsigned int*)");

  int sval = fromTcl(obj, (int*)0);

  if (sval < 0)
    {
      throw rutz::error(fstring("expected integer "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value was negative)"), SRC_POS);
    }

  return (unsigned int) sval;
}

long Tcl::fromTcl(Tcl_Obj* obj, long*)
{
DOTRACE("Tcl::fromTcl(long*)");

  Tcl_WideInt wideval;

  static Tcl_ObjType* const wideIntType = Tcl_GetObjType("wideInt");

  ASSERT(wideIntType != 0);

  SafeUnshared safeobj(obj, wideIntType);

  const long longmax = std::numeric_limits<long>::max();
  const long longmin = std::numeric_limits<long>::min();

  if ( Tcl_GetWideIntFromObj(0, safeobj.get(), &wideval) != TCL_OK )
    {
      throw rutz::error(fstring("expected long value "
                                "but got \"", Tcl_GetString(obj),
                                "\""), SRC_POS);
    }
  else if (wideval > static_cast<Tcl_WideInt>(longmax))
    {
      throw rutz::error(fstring("expected long value "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value too large)"), SRC_POS);
    }
  else if (wideval < static_cast<Tcl_WideInt>(longmin))
    {
      throw rutz::error(fstring("expected long value "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value too small)"), SRC_POS);
    }

  return static_cast<long>(wideval);
}

unsigned long Tcl::fromTcl(Tcl_Obj* obj, unsigned long*)
{
DOTRACE("Tcl::fromTcl(unsigned long*)");

  Tcl_WideInt wideval;

  static Tcl_ObjType* const wideIntType = Tcl_GetObjType("wideInt");

  ASSERT(wideIntType != 0);

  SafeUnshared safeobj(obj, wideIntType);

  const unsigned long ulongmax = std::numeric_limits<unsigned long>::max();

  if ( Tcl_GetWideIntFromObj(0, safeobj.get(), &wideval) != TCL_OK )
    {
      throw rutz::error(fstring("expected unsigned long value "
                                "but got \"", Tcl_GetString(obj),
                                "\""), SRC_POS);
    }
  else if (wideval < 0)
    {
      throw rutz::error(fstring("expected unsigned long value "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value was negative)"), SRC_POS);
    }
  else if (wideval > static_cast<Tcl_WideInt>(ulongmax))
    {
      throw rutz::error(fstring("expected unsigned long value "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value too large)"), SRC_POS);
    }

  return static_cast<unsigned long>(wideval);
}

bool Tcl::fromTcl(Tcl_Obj* obj, bool*)
{
DOTRACE("Tcl::fromTcl(bool*)");

  int int_val;

  static Tcl_ObjType* const booleanType = Tcl_GetObjType("boolean");

  ASSERT(booleanType != 0);

  SafeUnshared safeobj(obj, booleanType);

  if ( Tcl_GetBooleanFromObj(0, safeobj.get(), &int_val) != TCL_OK )
    {
      throw rutz::error(fstring("expected boolean value but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }
  return bool(int_val);
}

double Tcl::fromTcl(Tcl_Obj* obj, double*)
{
DOTRACE("Tcl::fromTcl(double*)");

  double val;

  static Tcl_ObjType* const doubleType = Tcl_GetObjType("double");

  ASSERT(doubleType != 0);

  SafeUnshared safeobj(obj, doubleType);

  if ( Tcl_GetDoubleFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(fstring("expected floating-point number but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }
  return val;
}

float Tcl::fromTcl(Tcl_Obj* obj, float*)
{
DOTRACE("Tcl::fromTcl(float*)");

  return float(fromTcl(obj, (double*)0));
}

const char* Tcl::fromTcl(Tcl_Obj* obj, const char**)
{
DOTRACE("Tcl::fromTcl(const char**)");

  return Tcl_GetString(obj);
}

fstring Tcl::fromTcl(Tcl_Obj* obj, fstring*)
{
DOTRACE("Tcl::fromTcl(fstring*)");

  int length;

  char* text = Tcl_GetStringFromObj(obj, &length);

  ASSERT(length >= 0);

  return fstring(char_range(text, (unsigned int) length));
}

Tcl::Dict Tcl::fromTcl(Tcl_Obj* obj, Tcl::Dict*)
{
DOTRACE("Tcl::fromTcl(Tcl::Dict*)");

  return Tcl::Dict(obj);
}

Tcl::List Tcl::fromTcl(Tcl_Obj* obj, Tcl::List*)
{
DOTRACE("Tcl::fromTcl(Tcl::List*)");

  return Tcl::List(obj);
}


///////////////////////////////////////////////////////////////////////
//
// (C++ --> Tcl) toTcl specializations
//
///////////////////////////////////////////////////////////////////////

Tcl::ObjPtr Tcl::toTcl(Tcl_Obj* val)
{
DOTRACE("Tcl::toTcl(Tcl_Obj*)");
  Tcl::ObjPtr result;
  result = val;
  return result;
}

Tcl::ObjPtr Tcl::toTcl(long val)
{
DOTRACE("Tcl::toTcl(long)");

  return Tcl_NewLongObj(val);
}

Tcl::ObjPtr Tcl::toTcl(unsigned long val)
{
DOTRACE("Tcl::toTcl(unsigned long)");

  long sval(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewLongObj(sval);
}

Tcl::ObjPtr Tcl::toTcl(int val)
{
DOTRACE("Tcl::toTcl(int)");

  return Tcl_NewIntObj(val);
}

Tcl::ObjPtr Tcl::toTcl(unsigned int val)
{
DOTRACE("Tcl::toTcl(unsigned int)");

  int sval(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewIntObj(sval);
}

Tcl::ObjPtr Tcl::toTcl(unsigned char val)
{
DOTRACE("Tcl::toTcl(unsigne char)");

  return Tcl_NewIntObj(val);
}

Tcl::ObjPtr Tcl::toTcl(bool val)
{
DOTRACE("Tcl::toTcl(bool)");

  return Tcl_NewBooleanObj(val);
}

Tcl::ObjPtr Tcl::toTcl(double val)
{
DOTRACE("Tcl::toTcl(double)");

  return Tcl_NewDoubleObj(val);
}

Tcl::ObjPtr Tcl::toTcl(float val)
{
DOTRACE("Tcl::toTcl(float)");

  return Tcl_NewDoubleObj(val);
}

Tcl::ObjPtr Tcl::toTcl(const char* val)
{
DOTRACE("Tcl::toTcl(const char*)");

  return Tcl_NewStringObj(val, -1);
}

Tcl::ObjPtr Tcl::toTcl(const fstring& val)
{
DOTRACE("Tcl::toTcl(const fstring&)");

  return Tcl_NewStringObj(val.c_str(), val.length());
}

Tcl::ObjPtr Tcl::toTcl(const Value& val)
{
DOTRACE("Tcl::toTcl(const Value&)");

  return Tcl_NewStringObj(val.getCstring(), -1);
}

Tcl::ObjPtr Tcl::toTcl(Tcl::Dict dictObj)
{
DOTRACE("Tcl::toTcl(Tcl::Dict)");

  return dictObj.asObj();
}

Tcl::ObjPtr Tcl::toTcl(Tcl::List listObj)
{
DOTRACE("Tcl::toTcl(Tcl::List)");

  return listObj.asObj();
}

Tcl::ObjPtr Tcl::toTcl(Tcl::ObjPtr val)
{
DOTRACE("Tcl::toTcl(Tcl::ObjPtr)");

  return val;
}

static const char vcid_tclconvert_cc[] = "$Header$";
#endif // !TCLCONVERT_CC_DEFINED
