///////////////////////////////////////////////////////////////////////
//
// iofactory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:06 1999
// written: Fri Feb 18 22:52:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_H_DEFINED
#define IOFACTORY_H_DEFINED

#ifndef FACTORY_H_DEFINED
#include "factory.h"
#endif

#ifndef IO_H_DEFINED
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
