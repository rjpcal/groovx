///////////////////////////////////////////////////////////////////////
//
// iomap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Oct 24 16:27:13 2000
// written: Tue Oct 24 17:32:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMAP_CC_DEFINED
#define IOMAP_CC_DEFINED

#include "io/iomap.h"

#include "io/io.h"

#include "util/error.h"

#include <map>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class InvalidUID : public ErrorWithMsg {
public:
  InvalidUID(IO::UID uid) :
	 ErrorWithMsg("InvalidUID: no object exists for the UID '")
	 {
		appendNumber(int(uid));
		appendMsg("'");
	 }
};

namespace {
  IO::IoMap* theInstance = 0;
}

class IO::IoMap::Impl {
public:
  Impl() : itsMap(), itsMin(0), itsMax(0) {}
  ~Impl() {}

  typedef std::map<IO::UID, IO::IoObject*> MapType;
  MapType itsMap;

  IO::UID itsMin;
  IO::UID itsMax;

  bool exists(IO::UID uid) const
	 {
		MapType::const_iterator itr = itsMap.find(uid);

		// see if the slot is filled...
		if (itr == itsMap.end())
		  return false;

		// ...if it is, make sure it contains a non-null pointer
		return ((*itr).second != 0);
	 }

  bool check()
	 {
		return ( (itsMin <= itsMax) &&
					(itsMin == 0 || exists(itsMin)) &&
					(itsMax == 0 || exists(itsMax)) );
	 }
};

IO::IoMap& IO::IoMap::theIoMap() {
DOTRACE("IO::IoMap::theIoMap");
  if (theInstance == 0)
	 theInstance = new IoMap;
  return *theInstance;
}

IO::IoMap::IoMap() :
  itsImpl(new Impl)
{}

IO::IoMap::~IoMap() {
  delete itsImpl;
}

void IO::IoMap::insertObject(IO::IoObject* object) {
DOTRACE("IO::IoMap::insertObject");

  Precondition(object != 0); 
  Precondition( !(itsImpl->exists(object->id())) );

  IO::UID uid = object->id();

  itsImpl->itsMap[uid] = object;

  if (uid < itsImpl->itsMin) itsImpl->itsMin = uid;

  if (uid > itsImpl->itsMax) itsImpl->itsMax = uid;

  Invariant(itsImpl->check());
}

void IO::IoMap::removeObject(IO::IoObject* object) {
DOTRACE("IO::IoMap::removeObject");

  Precondition(object != 0); 
  Precondition(itsImpl->exists(object->id()));

  IO::UID uid = object->id();
  Invariant((itsImpl->itsMin) <= uid && (uid <= itsImpl->itsMax));

  Impl::MapType::iterator itr = itsImpl->itsMap.find(uid);

  itsImpl->itsMap.erase(itr);

  if (uid == itsImpl->itsMin)
	 {
		while ( !itsImpl->exists(itsImpl->itsMin) )
		  ++(itsImpl->itsMin);
	 }

  if (uid == itsImpl->itsMax)
	 {
		while ( !itsImpl->exists(itsImpl->itsMax) )
		  --(itsImpl->itsMax);
	 }

  Invariant(itsImpl->check());
}

bool IO::IoMap::objectExists(IO::UID uid) const {
DOTRACE("IO::IoMap::objectExists");

  Invariant(itsImpl->check());

  return itsImpl->exists(uid);
}

/// Will return 0 if no object exists for the given UID.
IO::IoObject* IO::IoMap::getObject(IO::UID uid) const {
DOTRACE("IO::IoMap::getObject");

  if ( !objectExists(uid) )
	 return 0;

  Postcondition(itsImpl->itsMap[uid] != 0);
  Invariant(itsImpl->itsMap[uid]->id() == uid);
  return itsImpl->itsMap[uid];
}

/// Will throw an exception if no object exists for the given UID.
IO::IoObject* IO::IoMap::getCheckedObject(IO::UID uid) const {
DOTRACE("IO::IoMap::getCheckedObject");
  if ( !objectExists(uid) )
	 throw InvalidUID(uid);

  Postcondition(itsImpl->itsMap[uid] != 0);
  Invariant(itsImpl->itsMap[uid]->id() == uid);
  return itsImpl->itsMap[uid];
}

IO::UID IO::IoMap::smallestUID() const {
DOTRACE("IO::IoMap::smallestUID");
  Invariant(itsImpl->check());
  return itsImpl->itsMin;
}

IO::UID IO::IoMap::largestUID() const {
DOTRACE("IO::IoMap::largestUID");
  Invariant(itsImpl->check());
  return itsImpl->itsMax;
}

static const char vcid_iomap_cc[] = "$Header$";
#endif // !IOMAP_CC_DEFINED
