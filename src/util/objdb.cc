///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Tue Dec  5 18:09:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "io/ioptrlist.h"

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
  const IO::VersionId IOPTRLIST_SERIAL_VERSION_ID = 1;
}

InvalidIdError::InvalidIdError() : ErrorWithMsg() {}

InvalidIdError::InvalidIdError(const char* msg) : ErrorWithMsg(msg) {}

InvalidIdError::~InvalidIdError() {}

///////////////////////////////////////////////////////////////////////
//
// IoPtrList::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IoPtrList::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  IoPtrList* itsOwner;

public:

  typedef PtrHandle<IO::IoObject> IoPtrHandle;

  typedef std::map<int, IoPtrHandle> MapType;
  MapType itsPtrMap;

  Impl(IoPtrList* owner) :
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
	 DOTRACE("IoPtrList::Impl::insertPtrBase");
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
// IoPtrList::ItrImpl definition
//
///////////////////////////////////////////////////////////////////////

class IoPtrList::ItrImpl {
public:
  typedef IoPtrList::Impl::MapType MapType;

  ItrImpl(MapType::iterator itr) : itsIter(itr) {}

  MapType::iterator itsIter;
};

///////////////////////////////////////////////////////////////////////
//
// IoPtrList::Iterator member definitions
//
///////////////////////////////////////////////////////////////////////

IoPtrList::Iterator::Iterator(IoPtrList::ItrImpl* impl) :
  itsImpl(impl)
{
DOTRACE("IoPtrList::Iterator::Iterator()");
}

IoPtrList::Iterator::~Iterator()
{
DOTRACE("IoPtrList::Iterator::~Iterator");
  delete itsImpl;
}

IoPtrList::Iterator::Iterator(const IoPtrList::Iterator& other) :
  itsImpl(new ItrImpl(other.itsImpl->itsIter))
{
DOTRACE("IoPtrList::Iterator::Iterator(copy)");
}

IoPtrList::Iterator&
IoPtrList::Iterator::operator=(const IoPtrList::Iterator& other)
{
DOTRACE("IoPtrList::Iterator::operator=");
  ItrImpl* old_impl = itsImpl;
  itsImpl = new ItrImpl(other.itsImpl->itsIter);
  delete old_impl;
  return *this;
}

bool IoPtrList::Iterator::operator==(
      const IoPtrList::Iterator& other) const
{
  return itsImpl->itsIter == other.itsImpl->itsIter;
}

IoPtrList::Iterator&
IoPtrList::Iterator::operator++()
{
  ++(itsImpl->itsIter);
  return *this;
}

int IoPtrList::Iterator::getId() const
{
  return (*(itsImpl->itsIter)).first;
}

IO::IoObject* IoPtrList::Iterator::getObject() const
{
  return (*(itsImpl->itsIter)).second.get();
}

///////////////////////////////////////////////////////////////////////
//
// IoPtrList member definitions
//
///////////////////////////////////////////////////////////////////////

IoPtrList IoPtrList::theInstance;

IoPtrList& IoPtrList::theList() { return theInstance; }

IoPtrList::Iterator IoPtrList::begin() const {
DOTRACE("IoPtrList::begin");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.begin()));
}

IoPtrList::Iterator IoPtrList::end() const {
DOTRACE("IoPtrList::end");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.end()));
}

IoPtrList::IoPtrList() :
  itsImpl(new Impl(this))
{
DOTRACE("IoPtrList::IoPtrList");
}

IoPtrList::~IoPtrList() {
DOTRACE("IoPtrList::~IoPtrList");
  delete itsImpl; 
}

IO::VersionId IoPtrList::serialVersionId() const {
  return IOPTRLIST_SERIAL_VERSION_ID;
}

void IoPtrList::readFrom(IO::Reader* reader) {
DOTRACE("IoPtrList::readFrom");

  reader->ensureReadVersionId("IoPtrList", 1, "Try grsh0.8a3");

  purge();
  IO::ReadUtils::readObjectSeq<IO::IoObject>(reader, "itsVec", inserter());
}

void IoPtrList::writeTo(IO::Writer* writer) const {
DOTRACE("IoPtrList::writeTo");

  writer->ensureWriteVersionId("IoPtrList", IOPTRLIST_SERIAL_VERSION_ID, 1,
										 "Try grsh0.7a3");

  IO::WriteUtils::writeObjectSeq(writer, "itsVec",
											beginPtrs(), endPtrs());
}

int IoPtrList::count() const {
DOTRACE("IoPtrList::count");

  return itsImpl->count();
}

bool IoPtrList::isValidId(int id) const {
DOTRACE("IoPtrList::isValidId");
  return itsImpl->isValidId(id);
}

void IoPtrList::remove(int id) {
DOTRACE("IoPtrList::remove");
  itsImpl->remove(id);
}

void IoPtrList::release(int id) {
DOTRACE("IoPtrList::release");
  itsImpl->release(id);
}

void IoPtrList::purge() {
DOTRACE("IoPtrList::clear");
  DebugEvalNL(typeid(*this).name());
  itsImpl->purge();
}

void IoPtrList::clear() {
DOTRACE("IoPtrList::clear");
  // Call purge until no more items can be removed
  while ( itsImpl->purge() != 0 )
	 { ; }
}

void IoPtrList::clearOnExit() {
DOTRACE("IoPtrList::clearOnExit");
  itsImpl->clearAll(); 
}

IO::IoObject* IoPtrList::getCheckedPtrBase(int id) throw (InvalidIdError) {
DOTRACE("IoPtrList::getCheckedPtrBase");
  return itsImpl->getCheckedPtrBase(id);
}

int IoPtrList::insertPtrBase(IO::IoObject* ptr) {
DOTRACE("IoPtrList::insertPtrBase");

  return itsImpl->insertPtrBase(ptr);
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
