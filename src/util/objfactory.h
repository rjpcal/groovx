///////////////////////////////////////////////////////////////////////
//
// iofactory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:06 1999
// written: Sun Nov 21 15:55:46 1999
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
  ///
  IoFactory();
public:
  /// Return the singleton instance.
  static IoFactory& theOne();
};

static const char vcid_iofactory_h[] = "$Header$";
#endif // !IOFACTORY_H_DEFINED
