///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct  5 13:51:43 2000
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

#ifndef GROOVX_VISX_TCLPKG_HOOK_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_HOOK_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-hook.h"

#include "tcl/pkg.h"

#include "rutz/trace.h"

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
  rutz::backtrace::current().print();
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

#include "rutz/fstring.h"

using rutz::fstring;

template <class T>
const char* sformat_partial(char x, fstring& str, const char* format,
                            const T& arg)
{
  const char* p = format;
  for (; *p != '\0'; ++p)
    {
      if (*p == x)
        {
          str.append_range(format, p - format);
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

#include "rutz/debug.h"
GVX_DBG_REGISTER

#include <cstring>
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
GVX_TRACE("genericFreeInternalRepProc");
  GenericObj* gobj =
    static_cast<GenericObj*>(objPtr->internalRep.otherValuePtr);

  delete gobj;

  objPtr->internalRep.otherValuePtr = 0;
}

void genericDupInternalRepProc(Tcl_Obj* srcPtr, Tcl_Obj* dupPtr)
{
GVX_TRACE("genericDupInternalRepProc");
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
GVX_TRACE("genericUpdateStringProc");
  GenericObj* gobj =
    static_cast<GenericObj*>(objPtr->internalRep.otherValuePtr);

  GVX_ASSERT(gobj != 0);

  GVX_ASSERT(objPtr->bytes == 0);

  objPtr->bytes = gobj->asString();
  objPtr->length = strlen(objPtr->bytes);
}

int genericSetFromAnyProc(Tcl_Interp* interp, Tcl_Obj* /*objPtr*/)
{
GVX_TRACE("genericSetFromAnyProc");

  Tcl_AppendResult(interp, "can't convert to generic object type", 0);
  return TCL_ERROR;
}

Tcl_ObjType genericObjType =
  {
    const_cast<char*>("generic"),
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
    ++counter;
  }

  virtual ~MyObj()
  {
    value = -12345678;
    --counter;
  }

  static void convert(Tcl_Obj* objPtr)
  {
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
    Tcl_Obj* objPtr = Tcl_NewObj();
    objPtr->typePtr = &genericObjType;
    objPtr->internalRep.otherValuePtr = new MyObj(v);

    Tcl_InvalidateStringRep(objPtr);

    return objPtr;
  }

  static void info(Tcl_Obj* p)
  {
    dbg_eval_nl(3, p->typePtr ? p->typePtr->name : "");
    dbg_eval_nl(3, p->refCount);

    convert(p);

    MyObj* gobj =
      static_cast<MyObj*>(p->internalRep.otherValuePtr);

    dbg_eval_nl(3, p);
    dbg_eval_nl(3, p->typePtr ? p->typePtr->name : "");
    dbg_eval_nl(3, p->refCount);
    dbg_eval_nl(3, p->bytes);
    dbg_eval_nl(3, p->length);
    dbg_eval_nl(3, gobj);
    dbg_eval_nl(3, gobj->value);
    dbg_eval_nl(3, MyObj::counter);
  }

  virtual MyObj* clone() const
  {
    return new MyObj(value);
  }

  virtual char* asString() const
  {
    char* space = stringAlloc(32);

    snprintf(space, 32, "%d", value);

    return space;
  }

  int value;
};

int MyObj::counter = 0;

#include "gfx/gbvec.h"
#include "rutz/arrayvalue.h"
#include "rutz/iter.h"
#include "tcl/itertcl.h"
#include "tcl/dict.h"

namespace
{
  GbVec3<double> data[4];

  rutz::array_value<GbVec3<double> > arraydata;

  const rutz::array_value<GbVec3<double> >& getArray() { return arraydata; }

  unsigned int getArraySize() { return arraydata.array_size(); }

  void setArray(const rutz::array_value<GbVec3<double> >& a) { arraydata = a; }

  rutz::fwd_iter<GbVec3<double > > getData()
  {
    return rutz::fwd_iter<GbVec3<double> >(&data[0], &data[4]);
  }

  void setData(rutz::fwd_iter<const GbVec3<double> > iter)
  {
    int i = 0;
    while (i < 4 && !iter.at_end())
      {
        data[i] = *iter;
        ++iter; ++i;
      }
  }

  Tcl::Obj dictGet(Tcl::Dict dict, const char* key)
  {
    return dict.get<Tcl::Obj>(key);
  }
}

#include "gfx/toglet.h"
#include <GL/gl.h>

namespace
{
  void bug()
  {
    Nub::SoftRef<Toglet> t = Toglet::getCurrent();

    glTranslated(-2.56, -2.56, 0.0);

    const int SIZE = 512;

    unsigned char buf[SIZE*SIZE];

    unsigned char* bytes = &buf[0];

    for (int y = 0; y < SIZE; ++y)
      for (int x = 0; x < SIZE; ++x)
        *bytes++ = (x*y) % 256;

    glRasterPos3d(2.56, 2.56, 0.0);

    glBitmap(0, 0, 0.0f, 0.0f,
             -256, -256,
             static_cast<const GLubyte*>(0));

    glDrawPixels(SIZE, SIZE,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 static_cast<GLvoid*>(&buf[0]));

    t->swapBuffers();
  }
}

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  GVX_PKG_CREATE(pkg, interp, "Hook", "4.$Revision$");

  pkg->def( "::bug", "", &bug, SRC_POS );

  pkg->def( "::hook", "", HookTcl::hook, SRC_POS );
  pkg->def( "::memUsage", 0, HookTcl::memUsage, SRC_POS );

  Tcl_RegisterObjType(&genericObjType);

  pkg->def( "::myobj", "val", MyObj::make, SRC_POS );
  pkg->def( "::myinfo", "obj", MyObj::info, SRC_POS );

  pkg->def( "::getData", 0, getData, SRC_POS );
  pkg->def( "::setData", "list", setData, SRC_POS );

  pkg->def( "::getArray", 0, getArray, SRC_POS );
  pkg->def( "::getArraySize", 0, getArraySize, SRC_POS );
  pkg->def( "::setArray", "list", setArray, SRC_POS );

  pkg->def( "::dictGet", "dict key", dictGet, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_hook_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_HOOK_CC_UTC20050628171008_DEFINED
