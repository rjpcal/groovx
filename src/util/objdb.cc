///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Thu Oct 26 10:52:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "system/demangle.h"

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
// IoPtrHandle
//
///////////////////////////////////////////////////////////////////////

class IoPtrHandle {
  IoPtrHandle(); // not allowed

public:

  explicit IoPtrHandle(IO::IoObject* master) :
	 itsIoObject(master)
  {
	 Invariant(itsIoObject != 0);
	 itsIoObject->incrRefCount();
  }

  ~IoPtrHandle()
  {
	 Invariant(itsIoObject != 0);
	 itsIoObject->decrRefCount();
  }

  IoPtrHandle(const IoPtrHandle& other) :
	 itsIoObject(other.itsIoObject)
  {
	 Invariant(itsIoObject != 0);
	 itsIoObject->incrRefCount();
  }

  IoPtrHandle& operator=(const IoPtrHandle& other)
  {
	 IoPtrHandle otherCopy(other);
	 this->swap(otherCopy);
	 Invariant(itsIoObject != 0);
	 return *this;
  }

  IO::IoObject* ioObject() const
  {
	 Invariant(itsIoObject != 0);
	 return itsIoObject;
  }

private:
  void swap(IoPtrHandle& other)
  {
	 IO::IoObject* otherMaster = other.itsIoObject;
	 other.itsIoObject = this->itsIoObject;
	 this->itsIoObject = otherMaster;
  }

  IO::IoObject* itsIoObject;
};

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
#ifdef USE_OLD_ID_SYSTEM
  int itsFirstVacant;
#endif

public:
  typedef std::map<int, IoPtrHandle> MapType;
  MapType itsPtrMap;

private:
  int findPtr(IO::IoObject* ptr)
	 {
		for (MapType::const_iterator
				 itr = itsPtrMap.begin(),
				 end = itsPtrMap.end();
			  itr != end;
			  ++itr)
		  {
			 if ( (*itr).second.ioObject() == ptr )
				return (*itr).first;
		  }
		return -1;
	 }

#ifdef USE_OLD_ID_SYSTEM
  void updateFirstVacantOnErase(int id)
	 {
		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;		
	 }
#else
  void updateFirstVacantOnErase(int /* id */) {}
#endif

#ifdef USE_OLD_ID_SYSTEM
  int getIdForInsertion(IO::IoObject* ptr)
	 {
		DebugEval(itsFirstVacant);

		Assert(itsFirstVacant >= 0);
		Assert(itsPtrMap.find(itsFirstVacant) == itsPtrMap.end());

		const int new_id = itsFirstVacant;

		// make sure itsFirstVacant is up-to-date
		while ( itsPtrMap.find(++itsFirstVacant) != itsPtrMap.end() )
		  { ; }

		DebugEvalNL(itsFirstVacant);

		return new_id;
	 }
#else
  int getIdForInsertion(IO::IoObject* ptr) { return ptr->id(); }
#endif

public:
  Impl(IoPtrList* owner) :
	 itsOwner(owner),
#ifdef USE_OLD_ID_SYSTEM
	 itsFirstVacant(0),
#endif
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

		updateFirstVacantOnErase(id);
	 }

  void remove(int id)
	 {
		MapType::iterator itr = itsPtrMap.find(id);
		if (itr == itsPtrMap.end()) return;

		if ( (*itr).second.ioObject()->isShared() )
		  throw ErrorWithMsg("can't remove a shared object");

		itsPtrMap.erase(itr);

		updateFirstVacantOnErase(id);
	 }

  void clear()
	 {
		MapType new_map;

		for (MapType::const_iterator
				 itr = itsPtrMap.begin(),
				 end = itsPtrMap.end();
			  itr != end;
			  ++itr)
		  {
			 // If the object is unshared, we'll be releasing it, so
			 // update itsFirstVacant accordingly
			 if ( (*itr).second.ioObject()->isUnshared() )
				{
				  updateFirstVacantOnErase((*itr).first);
				}
			 // ...otherwise, we'll be saving the object, so copy it into
			 // the new_map,
			 else
				{
				  new_map.insert(*itr);
				}
		  }

		// Now swap maps so that the old map gets cleared and everything erased
		itsPtrMap.swap(new_map);
	 }

  IO::IoObject* getCheckedPtrBase(int id) const throw (InvalidIdError)
	 {
		MapType::const_iterator itr = itsPtrMap.find(id);
		if (itr == itsPtrMap.end()) {
		  InvalidIdError err("attempt to access invalid id '");
		  err.appendNumber(id);
		  err.appendMsg("' in ", demangle_cstr(typeid(*itsOwner).name()));
		  throw err;
		}

		return (*itr).second.ioObject();
	 }

  int insertPtrBase(IO::IoObject* ptr)
	 {
	 DOTRACE("IoPtrList::Impl::insertPtrBase");
		Precondition(ptr != 0);
		int existing_site = findPtr(ptr);

		if (existing_site != -1)
		  return existing_site;

		const int new_id = getIdForInsertion(ptr);
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
  return (*(itsImpl->itsIter)).second.ioObject();
}

///////////////////////////////////////////////////////////////////////
//
// IoPtrList member definitions
//
///////////////////////////////////////////////////////////////////////

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

  IO::VersionId svid = reader->readSerialVersionId(); 

  if (svid < 1)
	 {
		throw IO::ReadVersionError("IoPtrList", svid, 1,
											"Try grsh0.8a3");
	 }

  Assert(svid >= 1);

  clear();
  IO::ReadUtils::readObjectSeq<IO::IoObject>(reader, "itsVec", inserter());
}

void IoPtrList::writeTo(IO::Writer* writer) const {
DOTRACE("IoPtrList::writeTo");

  if (IOPTRLIST_SERIAL_VERSION_ID < 1)
	 {
		throw IO::WriteVersionError("IoPtrList",
											 IOPTRLIST_SERIAL_VERSION_ID, 1,
											 "Try grsh0.8a3");
	 }

  Assert(IOPTRLIST_SERIAL_VERSION_ID >= 1); 

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

void IoPtrList::clear() {
DOTRACE("IoPtrList::clear");
  DebugEvalNL(typeid(*this).name());
  itsImpl->clear();
}

IO::IoObject* IoPtrList::getCheckedPtrBase(int id) const throw (InvalidIdError) {
DOTRACE("IoPtrList::getCheckedPtrBase");
  return itsImpl->getCheckedPtrBase(id);
}

int IoPtrList::insertPtrBase(IO::IoObject* ptr) {
DOTRACE("IoPtrList::insertPtrBase");

  ensureCorrectType(ptr); 

  return itsImpl->insertPtrBase(ptr);
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
