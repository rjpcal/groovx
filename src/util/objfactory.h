///////////////////////////////////////////////////////////////////////
//
// iofactory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:06 1999
// written: Wed Jun 30 16:25:21 1999
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

class IoFactory : public Factory<IO> {
protected:
  IoFactory();
public:
  static IoFactory& theOne();
};

static const char vcid_iofactory_h[] = "$Header$";
#endif // !IOFACTORY_H_DEFINED
