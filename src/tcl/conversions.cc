/** @file tcl/conversions.cc tcl conversion functions for basic types */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jul 11 08:58:53 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "tcl/conversions.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"
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
  class safe_unshared_obj
  {
  private:
    Tcl_Obj* m_obj;
    bool m_is_owning;

    safe_unshared_obj(const safe_unshared_obj&);
    safe_unshared_obj& operator=(const safe_unshared_obj&);

  public:
    safe_unshared_obj(Tcl_Obj* obj, const Tcl_ObjType* target_type) :
      m_obj(obj), m_is_owning(false)
    {
      if ( (m_obj->typePtr != target_type) && Tcl_IsShared(m_obj) )
        {
          m_is_owning = true;
          m_obj = Tcl_DuplicateObj(m_obj);
          Tcl_IncrRefCount(m_obj);
        }
    }

    Tcl_Obj* get() const { return m_obj; }

    ~safe_unshared_obj()
    {
      if (m_is_owning)
        Tcl_DecrRefCount(m_obj);
    }
  };
}

///////////////////////////////////////////////////////////////////////
//
// (Tcl --> C++) help_convert<>::from_tcl specializations
//
///////////////////////////////////////////////////////////////////////

int tcl::help_convert<int>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<int>::from_tcl");

  int val;

  static const Tcl_ObjType* const int_type = Tcl_GetObjType("int");

  GVX_ASSERT(int_type != nullptr);

  safe_unshared_obj safeobj(obj, int_type);

  if ( Tcl_GetIntFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(rutz::sfmt("expected integer but got \"%s\"",
                                   Tcl_GetString(obj)), SRC_POS);
    }

  return val;
}

unsigned int tcl::help_convert<unsigned int>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<unsigned int>::from_tcl");

  int sval = help_convert<int>::from_tcl(obj);

  if (sval < 0)
    {
      throw rutz::error(rutz::sfmt("expected integer but got \"%s\" "
                                   "(value was negative)",
                                   Tcl_GetString(obj)), SRC_POS);
    }

  return static_cast<unsigned int>(sval);
}

long tcl::help_convert<long>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<long>::from_tcl");

  long val;

  static const Tcl_ObjType* const int_type = Tcl_GetObjType("int");

  GVX_ASSERT(int_type != nullptr);

  safe_unshared_obj safeobj(obj, int_type);

  if ( Tcl_GetLongFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(rutz::sfmt("expected integer but got \"%s\"",
                                   Tcl_GetString(obj)), SRC_POS);
    }

  return val;
}

unsigned long tcl::help_convert<unsigned long>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<unsigned long>::from_tcl");

  long long wideval = help_convert<long long>::from_tcl(obj);

  const unsigned long ulongmax = std::numeric_limits<unsigned long>::max();

  if (wideval < 0)
    {
      throw rutz::error(rutz::sfmt("expected unsigned long value "
                                   "but got \"%s\" (value was negative)",
                                   Tcl_GetString(obj)), SRC_POS);
    }
  // OK, now we know our wideval is non-negative, so we can safely
  // cast it to an unsigned type (Tcl_WideUInt) for comparison against
  // ulongmax (note: don't try to do this comparison by casting
  // ulongmax to a signed type like Tcl_WideInt, since the result of
  // the cast will be a negative number, leading to a bogus
  // comparison)
  else if (static_cast<Tcl_WideUInt>(wideval) > ulongmax)
    {
      throw rutz::error(rutz::sfmt("expected unsigned long value "
                                   "but got \"%s\" "
                                   "(value too large, max is %lu)",
                                   Tcl_GetString(obj), ulongmax),
                        SRC_POS);
    }

  return static_cast<unsigned long>(wideval);
}

long long tcl::help_convert<long long>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<longlong>::from_tcl");

#ifdef TCL_WIDE_INT_IS_LONG

  return help_convert<long>::from_tcl(obj);

#else

  static const Tcl_ObjType* const wide_int_type = Tcl_GetObjType("wideInt");

  GVX_ASSERT(wide_int_type != nullptr);

  safe_unshared_obj safeobj(obj, wide_int_type);

  Tcl_WideInt wideval;

  if ( Tcl_GetWideIntFromObj(0, safeobj.get(), &wideval) != TCL_OK )
    {
      throw rutz::error(rutz::sfmt("expected long value but got \"%s\"",
                                   Tcl_GetString(obj)), SRC_POS);
    }

  return wideval;

#endif
}

bool tcl::help_convert<bool>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<bool>::from_tcl");

  int int_val;

  static const Tcl_ObjType* const boolean_type = Tcl_GetObjType("boolean");

  GVX_ASSERT(boolean_type != nullptr);

  safe_unshared_obj safeobj(obj, boolean_type);

  if ( Tcl_GetBooleanFromObj(0, safeobj.get(), &int_val) != TCL_OK )
    {
      throw rutz::error(rutz::sfmt("expected boolean value but got \"%s\"",
                                   Tcl_GetString(obj)), SRC_POS);
    }
  return bool(int_val);
}

double tcl::help_convert<double>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<double>::from_tcl");

  double val;

  static const Tcl_ObjType* const double_type = Tcl_GetObjType("double");

  GVX_ASSERT(double_type != nullptr);

  safe_unshared_obj safeobj(obj, double_type);

  if ( Tcl_GetDoubleFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw rutz::error(rutz::sfmt("expected floating-point number "
                                   "but got \"%s\"",
                                   Tcl_GetString(obj)), SRC_POS);
    }
  return val;
}

float tcl::help_convert<float>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<float>::from_tcl");

  return float(help_convert<double>::from_tcl(obj));
}

const char* tcl::help_convert<const char*>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<const char*>::from_tcl");

  return Tcl_GetString(obj);
}

fstring tcl::help_convert<fstring>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<fstring>::from_tcl");

  int length;

  char* text = Tcl_GetStringFromObj(obj, &length);

  GVX_ASSERT(length >= 0);

  return fstring(rutz::char_range(text, static_cast<unsigned int>(length)));
}


///////////////////////////////////////////////////////////////////////
//
// (C++ --> Tcl) convert_from overloads
//
///////////////////////////////////////////////////////////////////////

tcl::obj tcl::help_convert<long long>::to_tcl(long long val)
{
GVX_TRACE("tcl::convert_from(long long)");

  return Tcl_NewWideIntObj(val);
}

tcl::obj tcl::help_convert<long>::to_tcl(long val)
{
GVX_TRACE("tcl::convert_from(long)");

  return Tcl_NewLongObj(val);
}

tcl::obj tcl::help_convert<unsigned long>::to_tcl(unsigned long val)
{
GVX_TRACE("tcl::convert_from(unsigned long)");

  const long sval = long(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewLongObj(sval);
}

tcl::obj tcl::help_convert<int>::to_tcl(int val)
{
GVX_TRACE("tcl::convert_from(int)");

  return Tcl_NewIntObj(val);
}

tcl::obj tcl::help_convert<unsigned int>::to_tcl(unsigned int val)
{
GVX_TRACE("tcl::convert_from(unsigned int)");

  const long sval = long(val);

  if (sval < 0)
    throw rutz::error("signed/unsigned conversion failed", SRC_POS);

  return Tcl_NewLongObj(sval);
}

tcl::obj tcl::help_convert<unsigned char>::to_tcl(unsigned char val)
{
GVX_TRACE("tcl::convert_from(unsigne char)");

  return Tcl_NewIntObj(val);
}

tcl::obj tcl::help_convert<bool>::to_tcl(bool val)
{
GVX_TRACE("tcl::convert_from(bool)");

  return Tcl_NewBooleanObj(val);
}

tcl::obj tcl::help_convert<double>::to_tcl(double val)
{
GVX_TRACE("tcl::convert_from(double)");

  return Tcl_NewDoubleObj(val);
}

tcl::obj tcl::help_convert<float>::to_tcl(float val)
{
GVX_TRACE("tcl::convert_from(float)");

  return Tcl_NewDoubleObj(double(val));
}

tcl::obj tcl::help_convert<const char*>::to_tcl(const char* val)
{
GVX_TRACE("tcl::convert_from(const char*)");

  return Tcl_NewStringObj(val, -1);
}

tcl::obj tcl::help_convert<rutz::fstring>::to_tcl(const rutz::fstring& val)
{
GVX_TRACE("tcl::convert_from(const fstring&)");

  if (val.length() > std::numeric_limits<int>::max())
    throw rutz::error("string too long to store in tcl string object", SRC_POS);

  return Tcl_NewStringObj(val.c_str(), int(val.length()));
}

tcl::obj tcl::help_convert<rutz::value>::to_tcl(const rutz::value& val)
{
GVX_TRACE("tcl::convert_from(const rutz::value&)");

  return Tcl_NewStringObj(val.get_string().c_str(), -1);
}
