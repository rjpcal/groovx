///////////////////////////////////////////////////////////////////////
//
// iodb.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Mon Dec 11 17:46:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IODB_CC_DEFINED
#define IODB_CC_DEFINED

#include "io/iodb.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "system/demangle.h"

#include "util/ptrhandle.h"

#include <typeinfo>
#include <map>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const IO::VersionId IODB_SERIAL_VERSION_ID = 1;
}

InvalidIdError::InvalidIdError() : ErrorWithMsg() {}

InvalidIdError::InvalidIdError(const char* msg) : ErrorWithMsg(msg) {}

InvalidIdError::~InvalidIdError() {}

///////////////////////////////////////////////////////////////////////
//
// IoDb::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IoDb::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  IoDb* itsOwner;

public:

  typedef PtrHandle<IO::IoObject> IoPtrHandle;

  typedef std::map<int, IoPtrHandle> MapType;
  MapType itsPtrMap;

  Impl(IoDb* owner) :
	 itsOwner(owner),
	 itsPtrMap()
	 {}

  int count() const
	 {
		// Count the number of null pointers. In the STL call count, we must
		// cast the value (NULL) so that template deduction treats it as a
		// pointer rather than an int. Then return the number of non-null
		// pointers, i.e. the size of the container less the number of null
		// pointers.
		int num_non_null=0; 
		for (MapType::const_iterator
				 itr = itsPtrMap.begin(),
				 end = itsPtrMap.end();
			  itr != end;
			  ++itr)
		  {
			 ++num_non_null;
		  }

		return num_non_null;
	 }

  bool isValidId(int id) const
	 {
		DebugEval(id);
		MapType::const_iterator itr = itsPtrMap.find(id);
		return ( (itr != itsPtrMap.end()) );
	 }

  void release(int id)
	 {
		MapType::iterator itr = itsPtrMap.find(id);

		itsPtrMap.erase(itr);
	 }

  void remove(int id)
	 {
		MapType::iterator itr = itsPtrMap.find(id);
		if (itr == itsPtrMap.end()) return;

		if ( (*itr).second.get()->isShared() )
		  throw ErrorWithMsg("can't remove a shared object");

		itsPtrMap.erase(itr);
	 }

  // Return the number of items removed
  int purge()
	 {
		MapType new_map;

		int num_removed = 0;

		for (MapType::const_iterator
				 itr = itsPtrMap.begin(),
				 end = itsPtrMap.end();
			  itr != end;
			  ++itr)
		  {
			 // If the object is shared, we'll be saving the object, so
			 // copy it into the new_map,
			 if ( (*itr).second.get()->isShared() )
				{
				  new_map.insert(*itr);
				}
			 else
				{
				  ++num_removed;
				}
		  }

		// Now swap maps so that the old map gets cleared and everything erased
		itsPtrMap.swap(new_map);
		return num_removed;
	 }

  void clearAll()
    { itsPtrMap.clear(); }

  IO::IoObject* getCheckedPtrBase(int id) throw (InvalidIdError)
	 {
		MapType::iterator itr = itsPtrMap.find(id);
		if (itr == itsPtrMap.end()) {
		  InvalidIdError err("attempt to access invalid id '");
		  err.appendNumber(id);
		  err.appendMsg("' in ", demangle_cstr(typeid(*itsOwner).name()));
		  throw err;
		}

		return (*itr).second.get();
	 }

  int insertPtrBase(IO::IoObject* ptr)
	 {
	 DOTRACE("IoDb::Impl::insertPtrBase");
		Precondition(ptr != 0);

		// Check if the object is already in the map
		MapType::iterator existing_site = itsPtrMap.find(ptr->id());
		if (existing_site != itsPtrMap.end())
		  {
			 // Make sure the existing object is the same as the object
			 // that we're trying to insert
			 Assert( (*existing_site).second.get() == ptr );

			 return ptr->id();
		  }

		const int new_id = ptr->id();
		itsPtrMap.insert(MapType::value_type(new_id, IoPtrHandle(ptr)));

		return new_id;
	 }
};

///////////////////////////////////////////////////////////////////////
//
// IoDb::ItrImpl definition
//
///////////////////////////////////////////////////////////////////////

class IoDb::ItrImpl {
public:
  typedef IoDb::Impl::MapType MapType;

  ItrImpl(MapType::iterator itr) : itsIter(itr) {}

  MapType::iterator itsIter;
};

///////////////////////////////////////////////////////////////////////
//
// IoDb::Iterator member definitions
//
///////////////////////////////////////////////////////////////////////

IoDb::Iterator::Iterator(IoDb::ItrImpl* impl) :
  itsImpl(impl)
{
DOTRACE("IoDb::Iterator::Iterator()");
}

IoDb::Iterator::~Iterator()
{
DOTRACE("IoDb::Iterator::~Iterator");
  delete itsImpl;
}

IoDb::Iterator::Iterator(const IoDb::Iterator& other) :
  itsImpl(new ItrImpl(other.itsImpl->itsIter))
{
DOTRACE("IoDb::Iterator::Iterator(copy)");
}

IoDb::Iterator&
IoDb::Iterator::operator=(const IoDb::Iterator& other)
{
DOTRACE("IoDb::Iterator::operator=");
  ItrImpl* old_impl = itsImpl;
  itsImpl = new ItrImpl(other.itsImpl->itsIter);
  delete old_impl;
  return *this;
}

bool IoDb::Iterator::operator==(
      const IoDb::Iterator& other) const
{
  return itsImpl->itsIter == other.itsImpl->itsIter;
}

IoDb::Iterator&
IoDb::Iterator::operator++()
{
  ++(itsImpl->itsIter);
  return *this;
}

int IoDb::Iterator::getId() const
{
  return (*(itsImpl->itsIter)).first;
}

IO::IoObject* IoDb::Iterator::getObject() const
{
  return (*(itsImpl->itsIter)).second.get();
}

///////////////////////////////////////////////////////////////////////
//
// IoDb member definitions
//
///////////////////////////////////////////////////////////////////////

IoDb IoDb::theInstance;

IoDb& IoDb::theDb() { return theInstance; }

IoDb::Iterator IoDb::begin() const {
DOTRACE("IoDb::begin");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.begin()));
}

IoDb::Iterator IoDb::end() const {
DOTRACE("IoDb::end");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.end()));
}

IoDb::IoDb() :
  itsImpl(new Impl(this))
{
DOTRACE("IoDb::IoDb");
}

IoDb::~IoDb() {
DOTRACE("IoDb::~IoDb");
  delete itsImpl; 
}

IO::VersionId IoDb::serialVersionId() const {
  return IODB_SERIAL_VERSION_ID;
}

void IoDb::readFrom(IO::Reader* reader) {
DOTRACE("IoDb::readFrom");

  reader->ensureReadVersionId("IoDb", 1, "Try grsh0.8a3");

  purge();
  IO::ReadUtils::readObjectSeq<IO::IoObject>(reader, "itsVec", inserter());
}

void IoDb::writeTo(IO::Writer* writer) const {
DOTRACE("IoDb::writeTo");

  writer->ensureWriteVersionId("IoDb", IODB_SERIAL_VERSION_ID, 1,
										 "Try grsh0.7a3");

  IO::WriteUtils::writeObjectSeq(writer, "itsVec",
											beginPtrs(), endPtrs());
}

int IoDb::count() const {
DOTRACE("IoDb::count");

  return itsImpl->count();
}

bool IoDb::isValidId(int id) const {
DOTRACE("IoDb::isValidId");
  return itsImpl->isValidId(id);
}

void IoDb::remove(int id) {
DOTRACE("IoDb::remove");
  itsImpl->remove(id);
}

void IoDb::release(int id) {
DOTRACE("IoDb::release");
  itsImpl->release(id);
}

void IoDb::purge() {
DOTRACE("IoDb::clear");
  DebugEvalNL(typeid(*this).name());
  itsImpl->purge();
}

void IoDb::clear() {
DOTRACE("IoDb::clear");
  // Call purge until no more items can be removed
  while ( itsImpl->purge() != 0 )
	 { ; }
}

void IoDb::clearOnExit() {
DOTRACE("IoDb::clearOnExit");
  itsImpl->clearAll(); 
}

IO::IoObject* IoDb::getCheckedPtrBase(int id) throw (InvalidIdError) {
DOTRACE("IoDb::getCheckedPtrBase");
  return itsImpl->getCheckedPtrBase(id);
}

int IoDb::insertPtrBase(IO::IoObject* ptr) {
DOTRACE("IoDb::insertPtrBase");

  return itsImpl->insertPtrBase(ptr);
}

static const char vcid_iodb_cc[] = "$Header$";
#endif // !IODB_CC_DEFINED
