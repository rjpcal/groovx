///////////////////////////////////////////////////////////////////////
//
// tclconvert.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:58:53 2001
// written: Thu Sep 12 15:09:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCONVERT_CC_DEFINED
#define TCLCONVERT_CC_DEFINED

#include "tcl/tclconvert.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclobjptr.h"

#include "util/strings.h"
#include "util/value.h"

#include <tcl.h>

#include "util/trace.h"
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

  template <class T>
  inline void ensurePositive(T t)
  {
    if (t < 0)
      throw Tcl::TclError("signed/unsigned conversion failed");
  }
}

template <>
Tcl_Obj* Tcl::Convert<Tcl_Obj*>::fromTcl( Tcl_Obj* obj ) { return obj; }

template <>
Tcl::ObjPtr Tcl::Convert<Tcl::ObjPtr>::fromTcl( Tcl_Obj* obj ) { return obj; }

template <>
Tcl::ObjPtr Tcl::Convert<const Tcl::ObjPtr&>::fromTcl( Tcl_Obj* obj )
{ return obj; }

template <>
Tcl::ObjPtr Tcl::Convert<Tcl_Obj*>::toTcl( Tcl_Obj* val )
{
  Tcl::ObjPtr result;
  result = val;
  return result;
}

///////////////////////////////////////////////////////////////////////
//
// (Tcl --> C++) fromTcl specializations
//
///////////////////////////////////////////////////////////////////////

template <>
int Tcl::Convert<int>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<int>::fromTcl");

  int val;

  SafeUnshared safeobj(obj, &tclIntType);

  if ( Tcl_GetIntFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw TclError(fstring("expected integer but got \"",
                             Tcl_GetString(obj), "\""));
    }
  return val;
}

template <>
unsigned int Tcl::Convert<unsigned int>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<unsigned int>::fromTcl");

  int sval = Convert<int>::fromTcl(obj);

  ensurePositive(sval);

  return (unsigned int) sval;
}

template <>
long Tcl::Convert<long>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<long>::fromTcl");

  long val;

  SafeUnshared safeobj(obj, &tclIntType);

  if ( Tcl_GetLongFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw TclError(fstring("expected long value but got \"",
                             Tcl_GetString(obj), "\""));
    }
  return val;
}

template <>
unsigned long Tcl::Convert<unsigned long>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<unsigned long>::fromTcl");

  long sval = Convert<long>::fromTcl(obj);

  ensurePositive(sval);

  return (unsigned long) sval;
}

template <>
bool Tcl::Convert<bool>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<bool>::fromTcl");

  int int_val;

  SafeUnshared safeobj(obj, &tclBooleanType);

  if ( Tcl_GetBooleanFromObj(0, safeobj.get(), &int_val) != TCL_OK )
    {
      throw TclError(fstring("expected boolean value but got \"",
                             Tcl_GetString(obj), "\""));
    }
  return bool(int_val);
}

template <>
double Tcl::Convert<double>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<double>::fromTcl");

  double val;

  SafeUnshared safeobj(obj, &tclDoubleType);

  if ( Tcl_GetDoubleFromObj(0, safeobj.get(), &val) != TCL_OK )
    {
      throw TclError(fstring("expected floating-point number but got \"",
                             Tcl_GetString(obj), "\""));
    }
  return val;
}

template <>
float Tcl::Convert<float>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<float>::fromTcl");

  return float(Convert<double>::fromTcl(obj));
}

template <>
const char* Tcl::Convert<const char*>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<const char*>::fromTcl");

  return Tcl_GetString(obj);
}

template <>
fstring Tcl::Convert<fstring>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<fstring>::fromTcl");

  int length;

  char* text = Tcl_GetStringFromObj(obj, &length);

  Assert(length >= 0);

  return fstring(Util::CharData(text, (unsigned int) length));
}

template <>
fstring Tcl::Convert<const fstring&>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<fstring>::fromTcl");

  return Convert<fstring>::fromTcl(obj);
}

template <>
Tcl::List Tcl::Convert<Tcl::List>::fromTcl(Tcl_Obj* obj)
{
DOTRACE("Tcl::Convert<Tcl::List>::fromTcl");

  return Tcl::List(obj);
}


///////////////////////////////////////////////////////////////////////
//
// (C++ --> Tcl) toTcl specializations
//
///////////////////////////////////////////////////////////////////////

template <>
Tcl::ObjPtr Tcl::Convert<long>::toTcl(long val)
{
DOTRACE("Tcl::Convert<long>::toTcl");

  return Tcl_NewLongObj(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<unsigned long>::toTcl(unsigned long val)
{
DOTRACE("Tcl::Convert<unsigned long>::toTcl");

  long sval(val);

  ensurePositive(sval);

  return Convert<long>::toTcl(sval);
}

template <>
Tcl::ObjPtr Tcl::Convert<int>::toTcl(int val)
{
DOTRACE("Tcl::Convert<int>::toTcl");

  return Tcl_NewIntObj(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<unsigned int>::toTcl(unsigned int val)
{
DOTRACE("Tcl::Convert<unsigned int>::toTcl");

  int sval(val);

  ensurePositive(sval);

  return Convert<int>::toTcl(sval);
}

template <>
Tcl::ObjPtr Tcl::Convert<unsigned char>::toTcl(unsigned char val)
{
DOTRACE("Tcl::Convert<unsigne char>::toTcl");

  return Convert<unsigned int>::toTcl(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<bool>::toTcl(bool val)
{
DOTRACE("Tcl::Convert<bool>::toTcl");

  return Tcl_NewBooleanObj(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<double>::toTcl(double val)
{
DOTRACE("Tcl::Convert<double>::toTcl");

  return Tcl_NewDoubleObj(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<float>::toTcl(float val)
{
DOTRACE("Tcl::Convert<float>::toTcl");

  return Convert<double>::toTcl(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<const char*>::toTcl(const char* val)
{
DOTRACE("Tcl::Convert<const char*>::toTcl");

  return Tcl_NewStringObj(val, -1);
}

template <>
Tcl::ObjPtr Tcl::Convert<char*>::toTcl(char* val)
{
  return Convert<const char*>::toTcl(val);
}

template <>
Tcl::ObjPtr Tcl::Convert<const fstring&>::toTcl(const fstring& val)
{
DOTRACE("Tcl::Convert<const fstring&>::toTcl");

  return Tcl_NewStringObj(val.c_str(), val.length());
}

template <>
Tcl::ObjPtr Tcl::Convert<fstring>::toTcl(fstring val)
{
  return Convert<const fstring&>::toTcl(val);
}

namespace Tcl
{
  // FIXME FIXME FIXME !!!
  template <>
  struct Return<const Value&>
  {
    typedef void Type;
  };
}

template <>
Tcl::ObjPtr Tcl::Convert<const Value&>::toTcl(const Value& val)
{
DOTRACE("Tcl::Convert<const Value&>::toTcl");

  return Tcl::ObjPtr(val.getCstring());
}

template <>
Tcl::ObjPtr Tcl::Convert<Tcl::List>::toTcl(Tcl::List listObj)
{
DOTRACE("Tcl::Convert<Tcl::List>::toTcl");

  return listObj.asObj();
}

template <>
Tcl::ObjPtr Tcl::Convert<Tcl::ObjPtr>::toTcl(Tcl::ObjPtr val)
{
DOTRACE("Tcl::Convert<Tcl::ObjPtr>::toTcl");

  return val;
}

static const char vcid_tclconvert_cc[] = "$Header$";
#endif // !TCLCONVERT_CC_DEFINED
