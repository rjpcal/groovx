///////////////////////////////////////////////////////////////////////
//
// convert.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:58:53 2001
// written: Wed Jul 11 11:00:02 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CONVERT_CC_DEFINED
#define CONVERT_CC_DEFINED

#include "tcl/convert.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
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
  class SafeUnshared {
  private:
    bool isItOwning;
    Tcl_Obj* itsObj;

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

  SafeUnshared safeobj(obj, &tclIntType);

  if ( Tcl_GetIntFromObj(0, safeobj.get(), &val) != TCL_OK )
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

  SafeUnshared safeobj(obj, &tclIntType);

  if ( Tcl_GetLongFromObj(0, safeobj.get(), &val) != TCL_OK )
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

  SafeUnshared safeobj(obj, &tclBooleanType);

  if ( Tcl_GetBooleanFromObj(0, safeobj.get(), &int_val) != TCL_OK )
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

  SafeUnshared safeobj(obj, &tclDoubleType);

  if ( Tcl_GetDoubleFromObj(0, safeobj.get(), &val) != TCL_OK )
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
Tcl::List Tcl::fromTcl<Tcl::List>(Tcl_Obj* obj)
{
DOTRACE("Tcl::fromTcl<Tcl::List>");

  return Tcl::List(obj);
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
Tcl_Obj* Tcl::toTcl<int>(int val)
{
DOTRACE("Tcl::toTcl<int>");

  return Tcl_NewIntObj(val);
}

template <>
Tcl_Obj* Tcl::toTcl<unsigned int>(unsigned int val)
{
DOTRACE("Tcl::toTcl<unsigned int>");

  int sval(val);

  ensurePositive(sval);

  return toTcl<int>(sval);
}

template <>
Tcl_Obj* Tcl::toTcl<unsigned char>(unsigned char val)
{
DOTRACE("Tcl::toTcl<unsigne char>");

  return toTcl<unsigned int>(val);
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
Tcl_Obj* Tcl::toTcl<char*>(char* val)
{
DOTRACE("Tcl::toTcl<char*>");

  return toTcl<const char*>(val);
}

template <>
Tcl_Obj* Tcl::toTcl<const Value&>(const Value& val)
{
DOTRACE("Tcl::toTcl<const Value&>");

  Tcl_Obj* obj = 0;

  {
    TclValue tval(val);
    obj = tval.getObj();
    ++(obj->refCount);
  }

  --(obj->refCount);

  return obj;
}

template <>
Tcl_Obj* Tcl::toTcl<Tcl::List>(Tcl::List listObj)
{
DOTRACE("Tcl::toTcl<const Value&>");

  return listObj.asObj();
}

template <>
Tcl_Obj* Tcl::toTcl<Tcl::ObjPtr>(Tcl::ObjPtr val)
{
DOTRACE("Tcl::toTcl<Tcl::ObjPtr>");

  return val;
}

template <>
Tcl_Obj* Tcl::toTcl<Tcl_Obj*>(Tcl_Obj* val)
{
DOTRACE("Tcl::toTcl<Tcl_Obj*>");

  return val;
}

static const char vcid_convert_cc[] = "$Header$";
#endif // !CONVERT_CC_DEFINED
