///////////////////////////////////////////////////////////////////////
//
// iomap.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Oct 24 16:27:13 2000
// written: Fri Nov 10 17:03:57 2000
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

///////////////////////////////////////////////////////////////////////
//
// IO::IoMap::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IO::IoMap::Impl {
public:
  Impl() : itsMap() {}
  ~Impl() {}

  typedef std::map<IO::UID, IO::IoObject*> MapType;
  MapType itsMap;

  bool exists(IO::UID uid) const
	 {
		return getUncheckedObject(uid) != 0;
	 }

  IO::IoObject* getUncheckedObject(IO::UID uid) const
	 {
		MapType::const_iterator itr = itsMap.find(uid);

		if (itr == itsMap.end()) return 0;

		DebugEvalNL((*itr).second);
		return (*itr).second;
	 }
};

IO::IoMap& IO::IoMap::theIoMap() {
  if (theInstance == 0)
	 theInstance = new IoMap;
  return *theInstance;
}

///////////////////////////////////////////////////////////////////////
//
// IO::IoMap::ItrImpl definition
//
///////////////////////////////////////////////////////////////////////

class IO::IoMap::ItrImpl {
public:
  ItrImpl(IO::IoMap::Impl::MapType& map_) :
	 itsIter(map_.begin()),
	 itsEnd(map_.end())
	 {}

  IO::IoMap::Impl::MapType::const_iterator itsIter;
  IO::IoMap::Impl::MapType::const_iterator itsEnd;
};

///////////////////////////////////////////////////////////////////////
//
// IO::IoMap::Iterator definition
//
///////////////////////////////////////////////////////////////////////

IO::IoMap::Iterator::Iterator(IO::IoMap::Impl* impl) :
  itsImpl(new IO::IoMap::ItrImpl(impl->itsMap))
{}

IO::IoMap::Iterator::~Iterator() {
  delete itsImpl;
}

IO::IoMap::Iterator::Iterator(const IO::IoMap::Iterator& other) :
  itsImpl(new IO::IoMap::ItrImpl(*(other.itsImpl)))
{}

IO::IoMap::Iterator& 
IO::IoMap::Iterator::operator=(const IO::IoMap::Iterator& other) {

  ItrImpl* prev_impl = itsImpl;

  itsImpl = new ItrImpl(*(other.itsImpl));

  delete prev_impl;

  return *this;
}

IO::IoObject* IO::IoMap::Iterator::getObject() const {
  return (*(itsImpl->itsIter)).second;
}

IO::UID IO::IoMap::Iterator::getUID() const {
  return (*(itsImpl->itsIter)).first;
}

bool IO::IoMap::Iterator::hasMore() const {
DOTRACE("IO::IoMap::Iterator::hasMore");
  return (itsImpl->itsIter != itsImpl->itsEnd);
}

void IO::IoMap::Iterator::advance() {
DOTRACE("IO::IoMap::Iterator::advance");
  DebugEval((void*)getObject()); DebugEvalNL(getUID()); 
  ++(itsImpl->itsIter);
}

///////////////////////////////////////////////////////////////////////
//
// IoMap member definitions
//
///////////////////////////////////////////////////////////////////////

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
}

void IO::IoMap::removeObject(IO::IoObject* object) {
DOTRACE("IO::IoMap::removeObject");

  Precondition(object != 0); 
  Precondition(itsImpl->exists(object->id()));

  IO::UID uid = object->id();

  Impl::MapType::iterator itr = itsImpl->itsMap.find(uid);

  itsImpl->itsMap.erase(itr);
}

bool IO::IoMap::objectExists(IO::UID uid) const {
DOTRACE("IO::IoMap::objectExists");

  DebugEval(uid); DebugEvalNL(itsImpl->exists(uid));
  return itsImpl->exists(uid);
}

/// Will return 0 if no object exists for the given UID.
IO::IoObject* IO::IoMap::getObject(IO::UID uid) const {
DOTRACE("IO::IoMap::getObject");

  IO::IoObject* obj = itsImpl->getUncheckedObject(uid);

  if (obj == 0)
	 return 0;

  Postcondition(obj != 0);
  Invariant(obj->id() == uid);
  return obj;
}

/// Will throw an exception if no object exists for the given UID.
IO::IoObject* IO::IoMap::getCheckedObject(IO::UID uid) const {
DOTRACE("IO::IoMap::getCheckedObject");

  IO::IoObject* obj = itsImpl->getUncheckedObject(uid);

  if ( obj == 0 )
	 throw InvalidUID(uid);

  Postcondition(obj != 0);
  Invariant(obj->id() == uid);
  return obj;
}

IO::UID IO::IoMap::smallestUID() const {
DOTRACE("IO::IoMap::smallestUID");

  // This works since the map is sorted by the UID's
  return itsImpl->itsMap.begin()->first;
}

IO::UID IO::IoMap::largestUID() const {
DOTRACE("IO::IoMap::largestUID");

  // This works since the map is sorted by the UID's
  return itsImpl->itsMap.rbegin()->first;
}

IO::IoMap::Iterator IO::IoMap::getIterator() const {
DOTRACE("IO::IoMap::getIterator");
  return Iterator(itsImpl); 
}

static const char vcid_iomap_cc[] = "$Header$";
#endif // !IOMAP_CC_DEFINED
