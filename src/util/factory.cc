///////////////////////////////////////////////////////////////////////
//
// factory.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 20 22:37:31 1999
// written: Mon Jan 13 11:08:26 2003
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

struct CreatorMapBase::Impl
{
  Impl() : itsMap() {}

  typedef hash_array<fstring, void*,
    string_hasher<fstring> >
  MapType;

  MapType itsMap;
};

CreatorMapBase::CreatorMapBase() :
  itsImpl(new Impl)
{
DOTRACE("CreatorMapBase::CreatorMapBase");
}

CreatorMapBase::~CreatorMapBase()
{
DOTRACE("CreatorMapBase::~CreatorMapBase");
}

void CreatorMapBase::throwForType(const char* type)
{
  fstring typelist("known types are:");

  for (Impl::MapType::iterator ii = itsImpl->itsMap.begin();
       ii != itsImpl->itsMap.end();
       ++ii)
    {
      if (ii->value != 0)
        typelist.append("\n\t", ii->key);
    }

  throw Util::Error(fstring("unknown object type '", type, "'\n", typelist));
}

void CreatorMapBase::throwForType(const fstring& type)
{
  throwForType(type.c_str());
}

void CreatorMapBase::clear()
{
DOTRACE("CreatorMapBase::clear");
  for (Impl::MapType::iterator ii = itsImpl->itsMap.begin();
       ii != itsImpl->itsMap.end();
       ++ii)
    {
      killPtr(ii->value);
      ii->value = 0;
    }

  delete itsImpl;
}

void* CreatorMapBase::getPtrForName(const fstring& name) const
{
DOTRACE("CreatorMapBase::getPtrForName");
  return itsImpl->itsMap[name];
}

void CreatorMapBase::setPtrForName(const char* name, void* ptr)
{
DOTRACE("CreatorMapBase::setPtrForName");
  fstring sname(name);
  void*& ptr_slot = itsImpl->itsMap[sname];
  killPtr(ptr_slot);
  ptr_slot = ptr;
}

FactoryBase::~FactoryBase()
{
DOTRACE("FactoryBase::~FactoryBase");
}

static const char vcid_factory_cc[] = "$Header$";
#endif // !FACTORY_CC_DEFINED
