///////////////////////////////////////////////////////////////////////
//
// iomap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Oct 24 16:19:25 2000
// written: Wed Oct 25 07:44:04 2000
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
public:
  IoMap();

  ~IoMap();

  void insertObject(IO::IoObject* object);

  void removeObject(IO::IoObject* object);

  class Impl;
  class ItrImpl;

  class Iterator {
  private:
	 ItrImpl* itsImpl;
  public:
	 Iterator(Impl* impl);
	 ~Iterator();
	 Iterator(const Iterator& other);
	 Iterator& operator=(const Iterator& other);

	 IO::IoObject* getObject() const;
	 IO::UID getUID() const;

	 bool hasMore() const;

	 void advance();
  };

  friend class IO::IoObject;

  static IoMap& theIoMap();

  bool objectExists(IO::UID uid) const;

  /// Will return 0 if no object exists for the given UID.
  IO::IoObject* getObject(IO::UID uid) const;

  /// Will throw an exception if no object exists for the given UID.
  IO::IoObject* getCheckedObject(IO::UID uid) const;

  IO::UID smallestUID() const;
  IO::UID largestUID() const;

  Iterator getIterator() const;

private:
  Impl* itsImpl;
};

static const char vcid_iomap_h[] = "$Header$";
#endif // !IOMAP_H_DEFINED
