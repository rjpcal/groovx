///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Tue Nov  5 05:28:08 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclpkg.h"

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
  Util::BackTrace::current().print();
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

#include "util/debug.h"

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
    dbgEvalNL(3, p->typePtr ? p->typePtr->name : "");
    dbgEvalNL(3, p->refCount);

    convert(p);

    MyObj* gobj =
      static_cast<MyObj*>(p->internalRep.otherValuePtr);

    dbgEvalNL(3, (void*)p);
    dbgEvalNL(3, p->typePtr ? p->typePtr->name : "");
    dbgEvalNL(3, p->refCount);
    dbgEvalNL(3, p->bytes);
    dbgEvalNL(3, p->length);
    dbgEvalNL(3, (void*)gobj);
    dbgEvalNL(3, gobj->value);
    dbgEvalNL(3, MyObj::counter);
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

#include "tcl/tcllistobj.h"
#include "util/error.h"

#include <map>

class HashObj : public GenericObj
{
public:
  HashObj();
  virtual ~HashObj();

  static Tcl_Obj* make();

  static HashObj* convert(Tcl_Obj* obj);

  virtual HashObj* clone() const;
  virtual char* asString() const;

  void insert(const fstring& key, const Tcl::ObjPtr& val)
  {
    itsMap.insert(MapType::value_type(key, val));
  }

  void erase(const fstring& key)
  {
    MapType::iterator i = itsMap.find(key);
    if (i == itsMap.end())
      throw Util::Error(fstring(key, ": no such key in hash"));
    itsMap.erase(i);
  }

  unsigned int size() const { return itsMap.size(); }

  bool hasKey(const fstring& key) const
  {
    return itsMap.find(key) != itsMap.end();
  }

  Tcl::ObjPtr get(const fstring& key) const
  {
    MapType::const_iterator i = itsMap.find(key);
    if (i == itsMap.end())
      throw Util::Error(fstring(key, ": no such key in hash"));
    return (*i).second;
  }

  Tcl::ObjPtr keys() const;
  Tcl::ObjPtr values() const;

  Tcl::ObjPtr pairs() const;

private:
  typedef std::map<fstring, Tcl::ObjPtr> MapType;
  MapType itsMap;
};

HashObj::HashObj()
{
DOTRACE("HashObj::HashObj");
}

HashObj::~HashObj()
{
DOTRACE("HashObj::~HashObj");
}

Tcl_Obj* HashObj::make()
{
DOTRACE("HashObj::make");

  Tcl_Obj* objPtr = Tcl_NewObj();
  objPtr->typePtr = &genericObjType;
  objPtr->internalRep.otherValuePtr = new HashObj;

  Tcl_InvalidateStringRep(objPtr);

  return objPtr;
}

HashObj* HashObj::convert(Tcl_Obj* objPtr)
{
DOTRACE("HashObj::convert");
  if (objPtr->typePtr == &genericObjType)
    {
      HashObj* p =
        dynamic_cast<HashObj*>
        (static_cast<GenericObj*>(objPtr->internalRep.otherValuePtr));

      if (p) return p;
    }

  Tcl::List l(objPtr);

  HashObj* p = new HashObj;

  for (unsigned int i = 1; i < l.length(); i += 2)
    {
      p->insert(Tcl::toNative<fstring>(l.at(i-1)), l.at(i));
    }

  objPtr->internalRep.otherValuePtr = static_cast<GenericObj*>(p);
  objPtr->typePtr = &genericObjType;

  return p;
}

HashObj* HashObj::clone() const
{
DOTRACE("HashObj::clone");

  return new HashObj(*this);
}

char* HashObj::asString() const
{
DOTRACE("HashObj::asString");

  Tcl::List l;

  for(MapType::const_iterator i = itsMap.begin(), stop = itsMap.end();
      i != stop; ++i)
    {
      l.append((*i).first);
      l.append((*i).second);
    }

  const char* s = l.asObj().as<const char*>();

  char* space = stringAlloc(strlen(s) + 1);
  strcpy(space, s);

  return space;
}

Tcl::ObjPtr HashObj::keys() const
{
DOTRACE("HashObj::keys");

  Tcl::List l;

  for(MapType::const_iterator i = itsMap.begin(), stop = itsMap.end();
      i != stop; ++i)
    {
      l.append((*i).first);
    }

  return l.asObj();
}

Tcl::ObjPtr HashObj::values() const
{
DOTRACE("HashObj::values");

  Tcl::List l;

  for(MapType::const_iterator i = itsMap.begin(), stop = itsMap.end();
      i != stop; ++i)
    {
      l.append((*i).second);
    }

  return l.asObj();
}

Tcl::ObjPtr HashObj::pairs() const
{
DOTRACE("HashObj::pairs");

  Tcl::List l;

  for(MapType::const_iterator i = itsMap.begin(), stop = itsMap.end();
      i != stop; ++i)
    {
      Tcl::List p;
      p.append((*i).first);
      p.append((*i).second);
      l.append(p.asObj());
    }

  return l.asObj();
}

void HashObj_Insert(Tcl_Obj* obj, const fstring& key, Tcl::ObjPtr val)
{
DOTRACE("HashObj_Insert");

  HashObj* p = HashObj::convert(obj);
  p->insert(key, val);
  Tcl_InvalidateStringRep(obj);
}

void HashObj_Erase(Tcl_Obj* obj, const fstring& key)
{
  HashObj* p = HashObj::convert(obj);
  p->erase(key);
  Tcl_InvalidateStringRep(obj);
}

unsigned int HashObj_Size(Tcl_Obj* obj)
{
  HashObj* p = HashObj::convert(obj);
  return p->size();
}

bool HashObj_HasKey(Tcl_Obj* obj, const fstring& key)
{
  HashObj* p = HashObj::convert(obj);
  return p->hasKey(key);
}

Tcl::ObjPtr HashObj_Get(Tcl_Obj* obj, const fstring& key)
{
  HashObj* p = HashObj::convert(obj);
  return p->get(key);
}

Tcl::ObjPtr HashObj_Keys(Tcl_Obj* obj)
{
  HashObj* p = HashObj::convert(obj);
  return p->keys();
}

Tcl::ObjPtr HashObj_Values(Tcl_Obj* obj)
{
  HashObj* p = HashObj::convert(obj);
  return p->values();
}

Tcl::ObjPtr HashObj_Pairs(Tcl_Obj* obj)
{
  HashObj* p = HashObj::convert(obj);
  return p->pairs();
}

#include "gfx/gbvec.h"
#include "util/arrayvalue.h"
#include "util/iter.h"
#include "tcl/itertcl.h"

namespace
{
  GbVec3<double> data[4];

  TArrayValue<GbVec3<double> > arraydata;

  const TArrayValue<GbVec3<double> >& getArray() { return arraydata; }

  unsigned int getArraySize() { return arraydata.arr().size(); }

  void setArray(const TArrayValue<GbVec3<double> >& a) { arraydata = a; }

  Util::FwdIter<GbVec3<double > > getData()
  {
    return Util::FwdIter<GbVec3<double> >(&data[0], &data[4]);
  }

  void setData(Util::FwdIter<const GbVec3<double> > iter)
  {
    int i = 0;
    while (i < 4 && !iter.atEnd())
      {
        data[i] = *iter;
        ++iter; ++i;
      }
  }
}

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Hook", "$Revision$");

  pkg->def( "::hook", "", HookTcl::hook );
  pkg->def( "::memUsage", 0, HookTcl::memUsage );

  Tcl_RegisterObjType(&genericObjType);

  pkg->def( "::myobj", "val", MyObj::make );
  pkg->def( "::myinfo", "obj", MyObj::info );

  pkg->def( "::hash::new", 0, HashObj::make );
  pkg->def( "::hash::insert", "hash key val", HashObj_Insert );
  pkg->def( "::hash::erase", "hash key", HashObj_Erase );
  pkg->def( "::hash::size", "hash", HashObj_Size );
  pkg->def( "::hash::has_key", "hash key", HashObj_HasKey );
  pkg->def( "::hash::get", "hash key", HashObj_Get );
  pkg->def( "::hash::keys", "hash", HashObj_Keys );
  pkg->def( "::hash::values", "hash", HashObj_Values );
  pkg->def( "::hash::pairs", "hash", HashObj_Pairs );

  pkg->def( "::getData", 0, getData );
  pkg->def( "::setData", "list", setData );

  pkg->def( "::getArray", 0, getArray );
  pkg->def( "::getArraySize", 0, getArraySize );
  pkg->def( "::setArray", "list", setArray );

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
