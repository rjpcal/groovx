///////////////////////////////////////////////////////////////////////
//
// objfactory.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Jun 26 23:40:06 1999
// written: Wed Mar 19 12:45:37 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJFACTORY_H_DEFINED
#define OBJFACTORY_H_DEFINED

#include "util/factory.h"
#include "util/object.h"
#include "util/ref.h"

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
