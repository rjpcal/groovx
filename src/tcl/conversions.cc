///////////////////////////////////////////////////////////////////////
//
// tclconvert.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jul 11 08:58:53 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_CONVERSIONS_CC_UTC20050628162420_DEFINED
#define GROOVX_TCL_CONVERSIONS_CC_UTC20050628162420_DEFINED

#include "tcl/conversions.h"

#include "tcl/dict.h"
#include "tcl/list.h"
#include "tcl/obj.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/value.h"

#include <limits>
#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

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
// (Tcl --> C++) fromTclImpl specializations
//
///////////////////////////////////////////////////////////////////////

Tcl_Obj* Tcl::fromTclImpl(Tcl_Obj* obj, Tcl_Obj**)
{ return obj; }

Tcl::Obj Tcl::fromTclImpl(Tcl_Obj* obj, Tcl::Obj*)
{ return obj; }

int Tcl::fromTclImpl(Tcl_Obj* obj, int*)
{
GVX_TRACE("Tcl::fromTclImpl(int*)");

  int val;

  static Tcl_ObjType* const intType = Tcl_GetObjType("int");

  GVX_ASSERT(intType != 0);

  SafeUnshared safeobj(obj, intType);

  if ( Tcl_GetIntFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(fstring("expected integer but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }

  return val;
}

unsigned int Tcl::fromTclImpl(Tcl_Obj* obj, unsigned int*)
{
GVX_TRACE("Tcl::fromTclImpl(unsigned int*)");

  int sval = fromTclImpl(obj, static_cast<int*>(0));

  if (sval < 0)
    {
      throw rutz::error(fstring("expected integer "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value was negative)"), SRC_POS);
    }

  return static_cast<unsigned int>(sval);
}

long Tcl::fromTclImpl(Tcl_Obj* obj, long*)
{
GVX_TRACE("Tcl::fromTclImpl(long*)");

  Tcl_WideInt wideval;

  static Tcl_ObjType* const wideIntType = Tcl_GetObjType("wideInt");

  GVX_ASSERT(wideIntType != 0);

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
                                "\" (value too large, max is ",
                                longmax,
                                ")"), SRC_POS);
    }
  else if (wideval < static_cast<Tcl_WideInt>(longmin))
    {
      throw rutz::error(fstring("expected long value "
                                "but got \"", Tcl_GetString(obj),
                                "\" (value too small, min is ",
                                longmin,
                                ")"), SRC_POS);
    }

  return static_cast<long>(wideval);
}

unsigned long Tcl::fromTclImpl(Tcl_Obj* obj, unsigned long*)
{
GVX_TRACE("Tcl::fromTclImpl(unsigned long*)");

  Tcl_WideInt wideval;

  static Tcl_ObjType* const wideIntType = Tcl_GetObjType("wideInt");

  GVX_ASSERT(wideIntType != 0);

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
                                "\" (value too large, max is ",
                                ulongmax,
                                ")"), SRC_POS);
    }

  return static_cast<unsigned long>(wideval);
}

bool Tcl::fromTclImpl(Tcl_Obj* obj, bool*)
{
GVX_TRACE("Tcl::fromTclImpl(bool*)");

  int int_val;

  static Tcl_ObjType* const booleanType = Tcl_GetObjType("boolean");

  GVX_ASSERT(booleanType != 0);

  SafeUnshared safeobj(obj, booleanType);

  if ( Tcl_GetBooleanFromObj(0, safeobj.get(), &int_val) != TCL_OK )
    {
      throw rutz::error(fstring("expected boolean value but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }
  return bool(int_val);
}

double Tcl::fromTclImpl(Tcl_Obj* obj, double*)
{
GVX_TRACE("Tcl::fromTclImpl(double*)");

  double val;

  static Tcl_ObjType* const doubleType = Tcl_GetObjType("double");

  GVX_ASSERT(doubleType != 0);

  SafeUnshared safeobj(obj, doubleType);

  if ( Tcl_GetDoubleFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(fstring("expected floating-point number but got \"",
                                Tcl_GetString(obj), "\""), SRC_POS);
    }
  return val;
}

float Tcl::fromTclImpl(Tcl_Obj* obj, float*)
{
GVX_TRACE("Tcl::fromTclImpl(float*)");

  return float(fromTclImpl(obj, static_cast<double*>(0)));
}

const char* Tcl::fromTclImpl(Tcl_Obj* obj, const char**)
{
GVX_TRACE("Tcl::fromTclImpl(const char**)");

  return Tcl_GetString(obj);
}

fstring Tcl::fromTclImpl(Tcl_Obj* obj, fstring*)
{
GVX_TRACE("Tcl::fromTclImpl(fstring*)");

  int length;

  char* text = Tcl_GetStringFromObj(obj, &length);

  GVX_ASSERT(length >= 0);

  return fstring(rutz::char_range(text, static_cast<unsigned int>(length)));
}

Tcl::Dict Tcl::fromTclImpl(Tcl_Obj* obj, Tcl::Dict*)
{
GVX_TRACE("Tcl::fromTclImpl(Tcl::Dict*)");

  return Tcl::Dict(obj);
}

Tcl::List Tcl::fromTclImpl(Tcl_Obj* obj, Tcl::List*)
{
GVX_TRACE("Tcl::fromTclImpl(Tcl::List*)");

  return Tcl::List(obj);
}


///////////////////////////////////////////////////////////////////////
//
// (C++ --> Tcl) toTclImpl specializations
//
///////////////////////////////////////////////////////////////////////

Tcl::Obj Tcl::toTclImpl(Tcl_Obj* val)
{
GVX_TRACE("Tcl::toTclImpl(Tcl_Obj*)");
  Tcl::Obj result;
  result = val;
  return result;
}

Tcl::Obj Tcl::toTclImpl(long val)
{
GVX_TRACE("Tcl::toTclImpl(long)");

  return Tcl_NewLongObj(val);
}

Tcl::Obj Tcl::toTclImpl(unsigned long val)
{
GVX_TRACE("Tcl::toTclImpl(unsigned long)");

  long sval(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewLongObj(sval);
}

Tcl::Obj Tcl::toTclImpl(int val)
{
GVX_TRACE("Tcl::toTclImpl(int)");

  return Tcl_NewIntObj(val);
}

Tcl::Obj Tcl::toTclImpl(unsigned int val)
{
GVX_TRACE("Tcl::toTclImpl(unsigned int)");

  int sval(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewIntObj(sval);
}

Tcl::Obj Tcl::toTclImpl(unsigned char val)
{
GVX_TRACE("Tcl::toTclImpl(unsigne char)");

  return Tcl_NewIntObj(val);
}

Tcl::Obj Tcl::toTclImpl(bool val)
{
GVX_TRACE("Tcl::toTclImpl(bool)");

  return Tcl_NewBooleanObj(val);
}

Tcl::Obj Tcl::toTclImpl(double val)
{
GVX_TRACE("Tcl::toTclImpl(double)");

  return Tcl_NewDoubleObj(val);
}

Tcl::Obj Tcl::toTclImpl(float val)
{
GVX_TRACE("Tcl::toTclImpl(float)");

  return Tcl_NewDoubleObj(val);
}

Tcl::Obj Tcl::toTclImpl(const char* val)
{
GVX_TRACE("Tcl::toTclImpl(const char*)");

  return Tcl_NewStringObj(val, -1);
}

Tcl::Obj Tcl::toTclImpl(const fstring& val)
{
GVX_TRACE("Tcl::toTclImpl(const fstring&)");

  return Tcl_NewStringObj(val.c_str(), val.length());
}

Tcl::Obj Tcl::toTclImpl(const rutz::value& val)
{
GVX_TRACE("Tcl::toTclImpl(const rutz::value&)");

  return Tcl_NewStringObj(val.get_string().c_str(), -1);
}

Tcl::Obj Tcl::toTclImpl(Tcl::Dict dictObj)
{
GVX_TRACE("Tcl::toTclImpl(Tcl::Dict)");

  return dictObj.asObj();
}

Tcl::Obj Tcl::toTclImpl(Tcl::List listObj)
{
GVX_TRACE("Tcl::toTclImpl(Tcl::List)");

  return listObj.asObj();
}

Tcl::Obj Tcl::toTclImpl(Tcl::Obj val)
{
GVX_TRACE("Tcl::toTclImpl(Tcl::Obj)");

  return val;
}

static const char vcid_groovx_tcl_conversions_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_CONVERSIONS_CC_UTC20050628162420_DEFINED
