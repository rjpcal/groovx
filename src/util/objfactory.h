///////////////////////////////////////////////////////////////////////
//
// objfactory.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 23:40:06 1999
// written: Wed Jun  6 19:44:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJFACTORY_H_DEFINED
#define OBJFACTORY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FACTORY_H_DEFINED)
#include "util/factory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "util/iditem.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

namespace Util
{
  class ObjFactory;
}

/// Singleton wrapper for Factor<IO>.
class Util::ObjFactory : public Factory<IdItem<Util::Object> > {
protected:
  /// Default constructor.
  ObjFactory();

  /// Virtual destructor.
  virtual ~ObjFactory();

public:
  /// Return the singleton instance.
  static ObjFactory& theOne();
};

static const char vcid_objfactory_h[] = "$Header$";
#endif // !OBJFACTORY_H_DEFINED
