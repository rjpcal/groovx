///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Tue Jul 23 13:35:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclpkg.h"

#define LOCAL_TRACE
#include "util/trace.h"

namespace
{
  size_t TOTAL = 0;
}

#if 0
#include <cstdlib>
#include <cstdio>

void* operator new(size_t bytes)
{
  TOTAL += bytes;
  printf("%d bytes  ", (int) bytes);
  Util::Trace::printStackTrace();
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

#include "util/strings.h"

template <class T>
const char* sformat_partial(char x, fstring& str, const char* format,
                            const T& arg)
{
  const char* p = format;
  for (; p != '\0'; ++p)
    {
      if (*p == x)
        {
          str.append(Util::CharData(format, p - format));
          str.append(arg);
          return p+1;
        }
    }
  return p; // here, *p == '\0'
}

template <char x, class T1, class T2>
const char* sformat_partial(fstring& str, const char* format,
                            const T1& arg1, const T2& arg2)
{
  const char* remainder = sformat_partial(x, str, format, arg1);
  return sformat_partial(x, str, remainder, arg2);
}

template <char x, class T1, class T2, class T3>
const char* sformat_partial(fstring& str, const char* format,
                            const T1& arg1, const T2& arg2, const T3& arg3)
{
  const char* remainder = sformat_partial<x>(str, format, arg1, arg2);
  return sformat_partial(x, str, remainder, arg3);
}

template <char x, class T>
fstring sformat(const char* format, const T& arg)
{
  fstring result;
  result.append(sformat_partial(x, result, format, arg));
  return result;
}

template <char x, class T1, class T2>
fstring sformat(const char* format, const T1& arg1, const T2& arg2)
{
  fstring result;
  result.append(sformat_partial<x>(result, format, arg1, arg2));
  return result;
}

namespace HookTcl
{
  fstring hook()
  {
    return sformat<'*'>("The * is *, so there.", "value", 42);
  }

  size_t memUsage() { return TOTAL; }
}

#define LOCAL_DEBUG
#include "util/debug.h"
#include <tcl.h>

class GenericObj
{
public:
  virtual ~GenericObj() {}

  static char* stringAlloc(unsigned int size) { return Tcl_Alloc(size); }
  static void stringFree(char* space) { Tcl_Free(space); }

  virtual GenericObj* clone() const = 0;
  virtual char* asString() const = 0;
};

extern Tcl_ObjType genericObjType;

void genericFreeInternalRepProc(Tcl_Obj* objPtr)
{
DOTRACE("genericFreeInternalRepProc");
  GenericObj* gobj =
    static_cast<GenericObj*>(objPtr->internalRep.otherValuePtr);

  delete gobj;

  objPtr->internalRep.otherValuePtr = 0;
}

void genericDupInternalRepProc(Tcl_Obj* srcPtr, Tcl_Obj* dupPtr)
{
DOTRACE("genericDupInternalRepProc");
  GenericObj* gobj =
    static_cast<GenericObj*>(srcPtr->internalRep.otherValuePtr);

  if (dupPtr->typePtr != 0 && dupPtr->typePtr->freeIntRepProc != 0)
    {
      dupPtr->typePtr->freeIntRepProc(dupPtr);
    }

  dupPtr->internalRep.otherValuePtr = static_cast<void*>(gobj->clone());

  dupPtr->typePtr = &genericObjType;
}

void genericUpdateStringProc(Tcl_Obj* objPtr)
{
DOTRACE("genericUpdateStringProc");
  GenericObj* gobj =
    static_cast<GenericObj*>(objPtr->internalRep.otherValuePtr);

  Assert(gobj != 0);

  Assert(objPtr->bytes == 0);

  objPtr->bytes = gobj->asString();
  objPtr->length = strlen(objPtr->bytes);
}

int genericSetFromAnyProc(Tcl_Interp* interp, Tcl_Obj* /*objPtr*/)
{
DOTRACE("genericSetFromAnyProc");

  Tcl_AppendResult(interp, "can't convert to generic object type", 0);
  return TCL_ERROR;
}

Tcl_ObjType genericObjType =
  {
    "generic",
    genericFreeInternalRepProc,
    genericDupInternalRepProc,
    genericUpdateStringProc,
    genericSetFromAnyProc
  };

#include <cstdio>
#include <cstdlib>

class MyObj : public GenericObj
{
  MyObj(const MyObj&);
  MyObj& operator=(const MyObj&);

  static int counter;

public:
  MyObj(int v) : value(v)
  {
  DOTRACE("MyObj::MyObj");

  ++counter;
  }

  virtual ~MyObj()
  {
  DOTRACE("MyObj::~MyObj");

  value = -12345678;

  --counter;
  }

  static void convert(Tcl_Obj* objPtr)
  {
  DOTRACE("MyObj::convert");

    if (objPtr->typePtr == &genericObjType)
      return;

    const char* strrep = Tcl_GetString(objPtr);

    int v = atoi(strrep);

    if (objPtr->typePtr != 0 && objPtr->typePtr->freeIntRepProc != 0)
      {
        objPtr->typePtr->freeIntRepProc(objPtr);
      }

    objPtr->typePtr = &genericObjType;

    objPtr->internalRep.otherValuePtr = new MyObj(v);
  }

  static Tcl_Obj* make(int v)
  {
  DOTRACE("MyObj::make");
    Tcl_Obj* objPtr = Tcl_NewObj();
    objPtr->typePtr = &genericObjType;
    objPtr->internalRep.otherValuePtr = new MyObj(v);

    Tcl_InvalidateStringRep(objPtr);

    return objPtr;
  }

  static void info(Tcl_Obj* p)
  {
  DOTRACE("MyObj::info");

    DebugEvalNL(p->typePtr ? p->typePtr->name : "");
    DebugEvalNL(p->refCount);

    convert(p);

    MyObj* gobj =
      static_cast<MyObj*>(p->internalRep.otherValuePtr);

    DebugEvalNL((void*)p);
    DebugEvalNL(p->typePtr ? p->typePtr->name : "");
    DebugEvalNL(p->refCount);
    DebugEvalNL(p->bytes);
    DebugEvalNL(p->length);
    DebugEvalNL((void*)gobj);
    DebugEvalNL(gobj->value);
    DebugEvalNL(MyObj::counter);
  }

  virtual MyObj* clone() const
  {
  DOTRACE("MyObj::clone");
    return new MyObj(value);
  }

  virtual char* asString() const
  {
  DOTRACE("MyObj::asString");
    char* space = stringAlloc(32);

    snprintf(space, 32, "%d", value);

    return space;
  }

  int value;
};

int MyObj::counter = 0;

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Hook", "$Revision$");

  Tcl_RegisterObjType(&genericObjType);

  pkg->def( "::hook", "", HookTcl::hook );
  pkg->def( "::memUsage", 0, HookTcl::memUsage );

  pkg->def( "::myobj", "val", MyObj::make );
  pkg->def( "::myinfo", "obj", MyObj::info );

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
