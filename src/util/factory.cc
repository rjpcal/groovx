///////////////////////////////////////////////////////////////////////
//
// factory.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Nov 20 22:37:31 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_CC_DEFINED
#define FACTORY_CC_DEFINED

#include "util/factory.h"

#include "util/error.h"
#include "util/strings.h"

#include <map>

#include "util/trace.h"

struct AssocArray::Impl
{
  Impl(KillFunc* f) : funcMap(), killFunc(f) {}

  typedef std::map<fstring, void*> MapType;

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
      if (ii->second != 0)
        typelist.append("\n\t", ii->first);
    }

  throw Util::Error(fstring(typelist, "\nunknown object type '", type, "'"));
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
      if (rep->killFunc != 0) rep->killFunc(ii->second);
      ii->second = 0;
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
