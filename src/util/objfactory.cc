///////////////////////////////////////////////////////////////////////
//
// objfactory.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 30 15:01:02 1999
// written: Fri Jan 18 16:07:04 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJFACTORY_CC_DEFINED
#define OBJFACTORY_CC_DEFINED

#include "util/objfactory.h"

namespace
{
  Util::ObjFactory* instance = 0;
}

Util::ObjFactory::ObjFactory() :
  Factory<SoftRef<Util::Object> >() {}

Util::ObjFactory::~ObjFactory() {}

Util::ObjFactory& Util::ObjFactory::theOne()
{
  if (instance == 0)
    {
      instance = new ObjFactory;
    }
  return *instance;
}

static const char vcid_objfactory_cc[] = "$Header$";
#endif // !OBJFACTORY_CC_DEFINED
