///////////////////////////////////////////////////////////////////////
//
// convert.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:58:53 2001
// written: Wed Jul 11 09:47:28 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CONVERT_CC_DEFINED
#define CONVERT_CC_DEFINED

#include "tcl/convert.h"

#include "tcl/tclerror.h"
#include "tcl/tclvalue.h"

#include <tcl.h>

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

extern Tcl_ObjType   tclBooleanType;
extern Tcl_ObjType   tclDoubleType;
extern Tcl_ObjType   tclIntType;
extern Tcl_ObjType   tclListType;
extern Tcl_ObjType   tclStringType;


///////////////////////////////////////////////////////////////////////
//
// File scope functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  inline Tcl_Obj* checkSharing(Tcl_Obj* obj, Tcl_ObjType* target_type)
    {
      if ( (obj->typePtr != target_type) && Tcl_IsShared(obj) )
        return Tcl_DuplicateObj(obj);
      return obj;
    }

  template <class T>
  inline void ensurePositive(T t)
  {
    if (t < 0)
      throw Tcl::TclError("signed/unsigned conversion failed");
  }
}


///////////////////////////////////////////////////////////////////////
//
// (Tcl --> C++) fromTcl specializations
//
///////////////////////////////////////////////////////////////////////

template <>
int Tcl::fromTcl<int>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<int>");

  int val;

  obj = checkSharing(obj, &tclIntType);

  if ( Tcl_GetIntFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected integer but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

template <>
unsigned int Tcl::fromTcl<unsigned int>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<unsigned int>");

  int sval = fromTcl<int>(obj);

  ensurePositive(sval);

  return (unsigned int) sval;
}

template <>
long Tcl::fromTcl<long>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<long>");

  long val;

  obj = checkSharing(obj, &tclIntType);

  if ( Tcl_GetLongFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected long value but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

template <>
unsigned long Tcl::fromTcl<unsigned long>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<unsigned long>");

  long sval = fromTcl<long>(obj);

  ensurePositive(sval);

  return (unsigned long) sval;
}

template <>
bool Tcl::fromTcl<bool>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<bool>");

  int int_val;

  obj = checkSharing(obj, &tclBooleanType);

  if ( Tcl_GetBooleanFromObj(0, obj, &int_val) != TCL_OK )
    {
      TclError err("expected boolean value but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return bool(int_val);
}

template <>
double Tcl::fromTcl<double>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<double>");

  double val;

  obj = checkSharing(obj, &tclDoubleType);

  if ( Tcl_GetDoubleFromObj(0, obj, &val) != TCL_OK )
    {
      TclError err("expected floating-point number but got ");
      err.appendMsg("\"", Tcl_GetString(obj), "\"");
      throw err;
    }
  return val;
}

template <>
float Tcl::fromTcl<float>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<float>");

  return float(fromTcl<double>(obj));
}

template <>
const char* Tcl::fromTcl<const char*>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<const char*>");

  return Tcl_GetString(obj);
}

template <>
Tcl::TclValue Tcl::fromTcl<Tcl::TclValue>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<Tcl::TclValue>");

  return Tcl::TclValue(obj);
}

template <>
Tcl_Obj* Tcl::fromTcl<Tcl_Obj*>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<Tcl_Obj*>");

  return obj;
}


///////////////////////////////////////////////////////////////////////
//
// (C++ --> Tcl) toTcl specializations
//
///////////////////////////////////////////////////////////////////////

template <>
Tcl_Obj* Tcl::toTcl<int>(int val)
{
DOTRACE("Tcl::toTcl<int>");

  return Tcl_NewIntObj(val);
}

template <>
Tcl_Obj* Tcl::toTcl<long>(long val)
{
DOTRACE("Tcl::toTcl<long>");

  return Tcl_NewLongObj(val);
}

template <>
Tcl_Obj* Tcl::toTcl<unsigned long>(unsigned long val)
{
DOTRACE("Tcl::toTcl<unsigned long>");

  long sval(val);

  ensurePositive(sval);

  return toTcl<long>(sval);
}

template <>
Tcl_Obj* Tcl::toTcl<bool>(bool val)
{
DOTRACE("Tcl::toTcl<bool>");

  return Tcl_NewBooleanObj(val);
}

template <>
Tcl_Obj* Tcl::toTcl<double>(double val)
{
DOTRACE("Tcl::toTcl<double>");

  return Tcl_NewDoubleObj(val);
}

template <>
Tcl_Obj* Tcl::toTcl<float>(float val)
{
DOTRACE("Tcl::toTcl<float>");

  return toTcl<double>(val);
}

template <>
Tcl_Obj* Tcl::toTcl<const char*>(const char* val)
{
DOTRACE("Tcl::toTcl<const char*>");

  return Tcl_NewStringObj(val, -1);
}

template <>
Tcl_Obj* Tcl::toTcl<Tcl_Obj*>(Tcl_Obj* val)
{
DOTRACE("Tcl::toTcl<Tcl_Obj*>");

  return val;
}

static const char vcid_convert_cc[] = "$Header$";
#endif // !CONVERT_CC_DEFINED
