///////////////////////////////////////////////////////////////////////
//
// iofactory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:06 1999
// written: Wed Mar 22 16:47:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_H_DEFINED
#define IOFACTORY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FACTORY_H)
#include "factory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H)
#include "io.h"
#endif

/// Singleton wrapper for Factor<IO>.
class IoFactory : public Factory<IO> {
protected:
  /// Default constructor.
  IoFactory();

public:
  /// Return the singleton instance.
  static IoFactory& theOne();
};

static const char vcid_iofactory_h[] = "$Header$";
#endif // !IOFACTORY_H_DEFINED
