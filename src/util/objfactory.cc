///////////////////////////////////////////////////////////////////////
//
// objfactory.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 30 15:01:02 1999
// written: Tue Aug 21 15:23:32 2001
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
