///////////////////////////////////////////////////////////////////////
//
// iofactory.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 23:40:06 1999
// written: Wed Jun  6 15:54:57 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_H_DEFINED
#define IOFACTORY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FACTORY_H_DEFINED)
#include "util/factory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "util/iditem.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

namespace IO { class IoFactory; }

/// Singleton wrapper for Factor<IO>.
class IO::IoFactory : public Factory<IdItem<IO::IoObject> > {
protected:
  /// Default constructor.
  IoFactory();

  /// Virtual destructor.
  virtual ~IoFactory();

public:
  /// Return the singleton instance.
  static IoFactory& theOne();
};

static const char vcid_iofactory_h[] = "$Header$";
#endif // !IOFACTORY_H_DEFINED
