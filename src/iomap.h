///////////////////////////////////////////////////////////////////////
//
// iomap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Oct 24 16:19:25 2000
// written: Tue Oct 24 17:00:32 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMAP_H_DEFINED
#define IOMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

namespace IO {
  class IoMap;
}

class IO::IoMap {
protected:
  IoMap();

  void insertObject(IO::IoObject* object);

  void removeObject(IO::IoObject* object);

public:
  ~IoMap();

  friend class IO::IoObject;

  static IoMap& theIoMap();

  bool objectExists(IO::UID uid) const;

  /// Will return 0 if no object exists for the given UID.
  IO::IoObject* getObject(IO::UID uid) const;

  /// Will throw an exception if no object exists for the given UID.
  IO::IoObject* getCheckedObject(IO::UID uid) const;

  IO::UID smallestUID() const;
  IO::UID largestUID() const;

private:
  class Impl;
  Impl* itsImpl;
};

static const char vcid_iomap_h[] = "$Header$";
#endif // !IOMAP_H_DEFINED
