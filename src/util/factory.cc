///////////////////////////////////////////////////////////////////////
//
// factory.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Nov 20 22:37:31 1999
// written: Wed Mar 19 12:45:40 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "util/factory.h"

#include "util/error.h"
#include "util/strings.h"

#include "util/hash.h"

#include "util/trace.h"

struct AssocArray::Impl
{
  Impl(KillFunc* f) : funcMap(), killFunc(f) {}

  typedef hash_array<fstring, void*, string_hasher<fstring> > MapType;

  MapType funcMap;
  KillFunc* killFunc;
};

AssocArray::AssocArray(KillFunc* f) :
  rep(new Impl(f))
{
DOTRACE("AssocArray::AssocArray");
}

AssocArray::~AssocArray()
{
DOTRACE("AssocArray::~AssocArray");
  clear();
}

void AssocArray::throwForType(const char* type)
{
  fstring typelist("known types are:");

  for (Impl::MapType::iterator ii = rep->funcMap.begin();
       ii != rep->funcMap.end();
       ++ii)
    {
      if (ii->value != 0)
        typelist.append("\n\t", ii->key);
    }

  throw Util::Error(fstring("unknown object type '", type, "'\n", typelist));
}

void AssocArray::throwForType(const fstring& type)
{
  throwForType(type.c_str());
}

void AssocArray::clear()
{
DOTRACE("AssocArray::clear");
  for (Impl::MapType::iterator ii = rep->funcMap.begin();
       ii != rep->funcMap.end();
       ++ii)
    {
      if (rep->killFunc != 0) rep->killFunc(ii->value);
      ii->value = 0;
    }

  delete rep;
}

void* AssocArray::getPtrForName(const fstring& name) const
{
DOTRACE("AssocArray::getPtrForName");
  return rep->funcMap[name];
}

void AssocArray::setPtrForName(const char* name, void* ptr)
{
DOTRACE("AssocArray::setPtrForName");
  fstring sname(name);
  void*& ptr_slot = rep->funcMap[sname];
  if (rep->killFunc != 0) rep->killFunc(ptr_slot);
  ptr_slot = ptr;
}

FactoryBase::~FactoryBase()
{
DOTRACE("FactoryBase::~FactoryBase");
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
