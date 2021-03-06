/** @file visx/tclpkg-hook.cc tcl interface package for miscellaneous
    functions */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Oct  5 13:51:43 2000
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

#include "visx/tclpkg-hook.h"

#include "tcl/pkg.h"

#include "rutz/trace.h"

#include <limits>

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

namespace HookTcl
{
  inline size_t memUsage() { return TOTAL; }
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

namespace {

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

  if (dupPtr->typePtr != nullptr && dupPtr->typePtr->freeIntRepProc != nullptr)
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

  GVX_ASSERT(gobj != nullptr);

  GVX_ASSERT(objPtr->bytes == 0);

  GVX_ASSERT(strlen(objPtr->bytes) < std::numeric_limits<int>::max());

  objPtr->bytes = gobj->asString();
  objPtr->length = int(strlen(objPtr->bytes));
}

int genericSetFromAnyProc(Tcl_Interp* interp, Tcl_Obj* /*objPtr*/)
{
GVX_TRACE("genericSetFromAnyProc");

  Tcl_AppendResult(interp, "can't convert to generic object type", 0);
  return TCL_ERROR;
}

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

    if (objPtr->typePtr != nullptr && objPtr->typePtr->freeIntRepProc != nullptr)
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

  virtual MyObj* clone() const override
  {
    return new MyObj(value);
  }

  virtual char* asString() const override
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
#if 0
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
#endif

  tcl::obj dictGet(const tcl::dict& dict, const char* key)
  {
    return dict.get<tcl::obj>(key);
  }
}

#include "tcl-gfx/toglet.h"
#if defined(GVX_GL_PLATFORM_GLX)
#  include <GL/gl.h>
#elif defined(GVX_GL_PLATFORM_AGL)
#  include <AGL/agl.h>
#endif

namespace
{
  void bug()
  {
    nub::soft_ref<Toglet> t = Toglet::getCurrent();

    glTranslated(-2.56, -2.56, 0.0);

    const int SIZE = 512;

    unsigned char buf[SIZE*SIZE];

    unsigned char* bytes = &buf[0];

    for (int y = 0; y < SIZE; ++y)
      for (int x = 0; x < SIZE; ++x)
        *bytes++ = (unsigned char)((x*y) % 256);

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
  return tcl::pkg::init
    (interp, "Hook", "4.0",
     [](tcl::pkg* pkg) {

      pkg->def( "::bug", "", &bug, SRC_POS );

      pkg->def( "::memUsage", 0, HookTcl::memUsage, SRC_POS );

      Tcl_RegisterObjType(&genericObjType);

      pkg->def( "::myobj", "val", MyObj::make, SRC_POS );
      pkg->def( "::myinfo", "obj", MyObj::info, SRC_POS );

#if 0
      pkg->def( "::getData", 0, getData, SRC_POS );
      pkg->def( "::setData", "list", setData, SRC_POS );

      pkg->def( "::getArray", 0, getArray, SRC_POS );
      pkg->def( "::getArraySize", 0, getArraySize, SRC_POS );
      pkg->def( "::setArray", "list", setArray, SRC_POS );
#endif

      pkg->def( "::dictGet", "dict key", dictGet, SRC_POS );
    });
}
