///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Wed Oct 25 16:44:35 2000
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
  Impl(PtrListBase* owner) :
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
	 DOTRACE("PtrListBase::Impl::insertPtrBase");
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
// PtrListBase::Iterator member definitions
//
///////////////////////////////////////////////////////////////////////

PtrListBase::Iterator::Iterator(PtrListBase::ItrImpl* impl) :
  itsImpl(impl)
{
DOTRACE("PtrListBase::Iterator::Iterator()");
}

PtrListBase::Iterator::~Iterator()
{
DOTRACE("PtrListBase::Iterator::~Iterator");
  delete itsImpl;
}

PtrListBase::Iterator::Iterator(const PtrListBase::Iterator& other) :
  itsImpl(new ItrImpl(other.itsImpl->itsIter))
{
DOTRACE("PtrListBase::Iterator::Iterator(copy)");
}

PtrListBase::Iterator&
PtrListBase::Iterator::operator=(const PtrListBase::Iterator& other)
{
DOTRACE("PtrListBase::Iterator::operator=");
  ItrImpl* old_impl = itsImpl;
  itsImpl = new ItrImpl(other.itsImpl->itsIter);
  delete old_impl;
  return *this;
}

bool PtrListBase::Iterator::operator==(
      const PtrListBase::Iterator& other) const
{
  return itsImpl->itsIter == other.itsImpl->itsIter;
}

PtrListBase::Iterator&
PtrListBase::Iterator::operator++()
{
  ++(itsImpl->itsIter);
  return *this;
}

int PtrListBase::Iterator::getId() const
{
  return (*(itsImpl->itsIter)).first;
}

IO::IoObject* PtrListBase::Iterator::getObject() const
{
  return (*(itsImpl->itsIter)).second.ioObject();
}

///////////////////////////////////////////////////////////////////////
//
// PtrListBase member definitions
//
///////////////////////////////////////////////////////////////////////

PtrListBase::Iterator PtrListBase::begin() const {
DOTRACE("PtrListBase::begin");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.begin()));
}

PtrListBase::Iterator PtrListBase::end() const {
DOTRACE("PtrListBase::end");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap.end()));
}

PtrListBase::PtrListBase() :
  itsImpl(new Impl(this))
{
DOTRACE("PtrListBase::PtrListBase");
}

PtrListBase::~PtrListBase() {
DOTRACE("PtrListBase::~PtrListBase");
  delete itsImpl; 
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
