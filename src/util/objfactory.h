///////////////////////////////////////////////////////////////////////
//
// iofactory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:06 1999
// written: Wed Jun 30 15:12:41 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOFACTORY_H_DEFINED
#define IOFACTORY_H_DEFINED

#ifndef FACTORY_H_DEFINED
#include "factory.h"
#define FACTORY_H_DEFINED
#endif

class IoFactory : public Factory<IO> {
protected:
  IoFactory();
public:
  static IoFactory& theOne();
};

static const char vcid_iofactory_h[] = "$Header$";
#endif // !IOFACTORY_H_DEFINED
