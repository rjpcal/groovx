///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Wed Oct 25 13:28:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_CC_DEFINED
#define PTRLISTBASE_CC_DEFINED

#include "ptrlistbase.h"

#include "io/io.h"

#include "system/demangle.h"

#include <typeinfo>
#include <map>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"


InvalidIdError::InvalidIdError() : ErrorWithMsg() {}

InvalidIdError::InvalidIdError(const char* msg) : ErrorWithMsg(msg) {}

InvalidIdError::~InvalidIdError() {}

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  static const int GROW_CHUNK = 20;
  static const double GROW_FACTOR = 1.2;

  size_t calculateGrowSize(size_t oldsize) {

	 double oldsize_d = double(oldsize);
	 double newsize_d = (oldsize_d * GROW_FACTOR) + GROW_CHUNK;

	 return size_t(newsize_d);
  }
}

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
// PtrListBase::Impl definition
//
///////////////////////////////////////////////////////////////////////

class PtrListBase::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  PtrListBase* itsOwner;
  int itsFirstVacant;

public:
  typedef std::map<int, IoPtrHandle> MapType;
  MapType itsPtrMap;

private:
  void ensureNotDuplicate(IO::IoObject* ptr);

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

  // Add obj at index 'id', destroying any the object was previously
  // pointed to from that that location. The list will be expanded if
  // 'id' exceeds the size of the list. If id is < 0, the function
  // returns without effect.
  void insertPtrBaseAt(int id, IO::IoObject* ptr);

public:
  Impl(PtrListBase* owner, int size) :
	 itsOwner(owner),
	 itsFirstVacant(0),
	 itsPtrMap()
	 {}

  int capacity() const
	 {
		MapType::const_reverse_iterator itr = itsPtrMap.rbegin();
		if (itr == itsPtrMap.rend()) return 1;
		return itsPtrMap.rbegin()->first + 1;
	 }

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

		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;		
	 }

  void remove(int id)
	 {
		MapType::iterator itr = itsPtrMap.find(id);
		if (itr == itsPtrMap.end()) return;

		if ( (*itr).second.ioObject()->isShared() )
		  throw ErrorWithMsg("can't remove a shared object");

		itsPtrMap.erase(itr);

		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;		
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
				  if (itsFirstVacant > (*itr).first) itsFirstVacant = (*itr).first;
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
		Precondition(ptr != 0);
		int existing_site = findPtr(ptr);

		if (existing_site != -1)
		  return existing_site;

		int new_site = itsFirstVacant;
		insertPtrBaseAt(new_site, ptr);
		return new_site;              // return the id of the inserted void*
	 }
};

///////////////////////////////////////////////////////////////////////
//
// PtrListBase::ItrImpl definition
//
///////////////////////////////////////////////////////////////////////

class PtrListBase::ItrImpl {
public:
  typedef PtrListBase::Impl::MapType MapType;

  ItrImpl(MapType::iterator itr) : itsIter(itr) {}

  MapType::iterator itsIter;
};

///////////////////////////////////////////////////////////////////////
//
// PtrListBase::IdIterator member definitions
//
///////////////////////////////////////////////////////////////////////

PtrListBase::IdIterator::IdIterator(PtrListBase::ItrImpl* impl) :
  itsImpl(impl)
{
DOTRACE("PtrListBase::IdIterator::IdIterator()");
}

PtrListBase::IdIterator::~IdIterator()
{
DOTRACE("PtrListBase::IdIterator::~IdIterator");
  delete itsImpl;
}

PtrListBase::IdIterator::IdIterator(const PtrListBase::IdIterator& other) :
  itsImpl(new ItrImpl(other.itsImpl->itsIter))
{
DOTRACE("PtrListBase::IdIterator::IdIterator(copy)");
}

PtrListBase::IdIterator&
PtrListBase::IdIterator::operator=(const PtrListBase::IdIterator& other)
{
DOTRACE("PtrListBase::IdIterator::operator=");
  ItrImpl* old_impl = itsImpl;
  itsImpl = new ItrImpl(other.itsImpl->itsIter);
  delete old_impl;
  return *this;
}

bool PtrListBase::IdIterator::operator==(
      const PtrListBase::IdIterator& other) const
{
  return itsImpl->itsIter == other.itsImpl->itsIter;
}

PtrListBase::IdIterator&
PtrListBase::IdIterator::operator++()
{
  ++(itsImpl->itsIter);
  return *this;
}

int PtrListBase::IdIterator::operator*() const
{
  return (*(itsImpl->itsIter)).first;
}

IO::IoObject* PtrListBase::IdIterator::getObject() const
{
  return (*(itsImpl->itsIter)).second.ioObject();
}

///////////////////////////////////////////////////////////////////////
//
// PtrListBase::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

// Asserts that there are no duplicate IO::IoObject's in the list, as
// well as no duplicate pointees.
void PtrListBase::Impl::ensureNotDuplicate(IO::IoObject* ptr) {
DOTRACE("PtrListBase::Impl::ensureNotDuplicate");

  Assert(findPtr(ptr) == -1); 
}

void PtrListBase::Impl::insertPtrBaseAt(int id, IO::IoObject* ptr) {
DOTRACE("PtrListBase::Impl::insertPtrBaseAt");

  Precondition(ptr != 0);

  DebugEval(id);
  if (id < 0) return;

  MapType::iterator itr = itsPtrMap.find(id);

  if (itr != itsPtrMap.end())
	 {
		InvalidIdError err("object already exists at id '");
		err.appendNumber(id);
		err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
		throw err;
	 }

  ensureNotDuplicate(ptr);

  //
  // The actual insertion
  //
  itsPtrMap.insert(MapType::value_type(id, IoPtrHandle(ptr)));

  // make sure itsFirstVacant is up-to-date
  while ( itsPtrMap.find(itsFirstVacant) != itsPtrMap.end() )
	 { ++itsFirstVacant; }

  DebugEvalNL(itsFirstVacant);
}

///////////////////////////////////////////////////////////////////////
//
// PtrListBase member definitions
//
///////////////////////////////////////////////////////////////////////

PtrListBase::IdIterator PtrListBase::beginIds() const {
DOTRACE("PtrListBase::beginIds");
  return IdIterator(new ItrImpl(itsImpl->itsPtrMap.begin()));
}

PtrListBase::IdIterator PtrListBase::endIds() const {
DOTRACE("PtrListBase::ebdIds");
  return IdIterator(new ItrImpl(itsImpl->itsPtrMap.end()));
}

PtrListBase::PtrListBase(int size) :
  itsImpl(new Impl(this, size))
{
DOTRACE("PtrListBase::PtrListBase");
}

PtrListBase::~PtrListBase() {
DOTRACE("PtrListBase::~PtrListBase");
  delete itsImpl; 
}

int PtrListBase::capacity() const {
DOTRACE("PtrListBase::capacity");
  return itsImpl->capacity();
}

int PtrListBase::count() const {
DOTRACE("PtrListBase::count");

  return itsImpl->count();
}

bool PtrListBase::isValidId(int id) const {
DOTRACE("PtrListBase::isValidId");
  return itsImpl->isValidId(id);
}

void PtrListBase::remove(int id) {
DOTRACE("PtrListBase::remove");
  itsImpl->remove(id);
}

void PtrListBase::release(int id) {
DOTRACE("PtrListBase::release");
  itsImpl->release(id);
}

void PtrListBase::clear() {
DOTRACE("PtrListBase::clear");
  DebugEvalNL(typeid(*this).name());
  itsImpl->clear();
}

IO::IoObject* PtrListBase::getCheckedPtrBase(int id) const throw (InvalidIdError) {
DOTRACE("PtrListBase::getCheckedPtrBase");
  return itsImpl->getCheckedPtrBase(id);
}

int PtrListBase::insertPtrBase(IO::IoObject* ptr) {
DOTRACE("PtrListBase::insertPtrBase");

  ensureCorrectType(ptr); 

  return itsImpl->insertPtrBase(ptr);
}

static const char vcid_ptrlistbase_cc[] = "$Header$";
#endif // !PTRLISTBASE_CC_DEFINED
