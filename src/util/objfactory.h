///////////////////////////////////////////////////////////////////////
//
// objfactory.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 23:40:06 1999
// written: Fri Jan 18 16:06:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJFACTORY_H_DEFINED
#define OBJFACTORY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FACTORY_H_DEFINED)
#include "util/factory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Util
{
  class ObjFactory;
}

/// Singleton wrapper for Factor<IO>.
class Util::ObjFactory : public Factory<Util::SoftRef<Util::Object> >
{
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
