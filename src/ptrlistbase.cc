///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Tue Oct 24 16:14:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_CC_DEFINED
#define PTRLISTBASE_CC_DEFINED

#include "ptrlistbase.h"

#include "util/refcounted.h"

#include "system/demangle.h"

#include <typeinfo>
#include <vector>

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
// VoidPtrHandle
//
///////////////////////////////////////////////////////////////////////

class DummyObject : public RefCounted {
  DummyObject() : RefCounted() {}

public:
  static DummyObject* getDummy() {
	 static DummyObject* theDummy = 0;
	 if (theDummy == 0) {
		theDummy = new DummyObject;
		theDummy->incrRefCount();
	 }
	 Postcondition(theDummy != 0);
	 return theDummy;
  }

  virtual bool isValid() const { return false; }
};

class VoidPtrHandle {
public:
  VoidPtrHandle() :
	 itsRCObject(DummyObject::getDummy())
  {
	 Invariant(itsRCObject != 0);
	 itsRCObject->incrRefCount();
  }

  explicit VoidPtrHandle(RefCounted* master) :
	 itsRCObject(master)
  {
	 Invariant(itsRCObject != 0);
	 itsRCObject->incrRefCount();
  }

  ~VoidPtrHandle()
  {
	 Invariant(itsRCObject != 0);
	 itsRCObject->decrRefCount();
  }

  VoidPtrHandle(const VoidPtrHandle& other) :
	 itsRCObject(other.itsRCObject)
  {
	 Invariant(itsRCObject != 0);
	 itsRCObject->incrRefCount();
  }

  VoidPtrHandle& operator=(const VoidPtrHandle& other)
  {
	 VoidPtrHandle otherCopy(other);
	 this->swap(otherCopy);
	 Invariant(itsRCObject != 0);
	 return *this;
  }

  RefCounted* rcObject()
  {
	 Invariant(itsRCObject != 0);
	 return itsRCObject;
  }

private:
  void swap(VoidPtrHandle& other)
  {
	 RefCounted* otherMaster = other.itsRCObject;
	 other.itsRCObject = this->itsRCObject;
	 this->itsRCObject = otherMaster;
  }

  RefCounted* itsRCObject;
};

///////////////////////////////////////////////////////////////////////
//
// PtrListBase::Impl definition
//
///////////////////////////////////////////////////////////////////////

class PtrListBase::Impl {
public:
  Impl(int size) :
	 itsFirstVacant(0),
	 itsPtrVec()
	 {
		itsPtrVec.reserve(size);
	 }

  int itsFirstVacant;
  typedef std::vector<VoidPtrHandle> VecType;
  VecType itsPtrVec;

  void ensureNotDuplicate(RefCounted* ptr);

  int findPtr(RefCounted* ptr)
  {
	 for (int i = 0; i < itsPtrVec.size(); ++i)
		{
		  if ( itsPtrVec[i].rcObject() == ptr )
			 return i;
		}
	 return -1;
  }

  // Add obj at index 'id', destroying any the object was previously
  // pointed to from that that location. The list will be expanded if
  // 'id' exceeds the size of the list. If id is < 0, the function
  // returns without effect.
  void insertPtrBaseAt(int id, RefCounted* ptr);

private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);
};

///////////////////////////////////////////////////////////////////////
//
// PtrListBase::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

// Asserts that there are no duplicate RefCounted's in the list, as
// well as no duplicate pointees.
void PtrListBase::Impl::ensureNotDuplicate(RefCounted* ptr) {
DOTRACE("PtrListBase::Impl::ensureNotDuplicate");

#ifdef LOCAL_ASSERT
  // First make sure that the we have no duplicates of ptr in the list
  for (int i = 0; i < itsPtrVec.size(); ++i) 
	 {
		DebugEvalNL(i);
		Assert(ptr != itsPtrVec[i].rcObject());
	 }
#endif
}

///////////////////////////////////////////////////////////////////////
//
// PtrListBase member definitions
//
///////////////////////////////////////////////////////////////////////

PtrListBase::PtrListBase(int size) :
  itsImpl(new Impl(size))
{
DOTRACE("PtrListBase::PtrListBase");
}

PtrListBase::~PtrListBase() {
DOTRACE("PtrListBase::~PtrListBase");
  delete itsImpl; 
}

int PtrListBase::capacity() const {
DOTRACE("PtrListBase::capacity");
  return itsImpl->itsPtrVec.size(); 
}

int PtrListBase::count() const {
DOTRACE("PtrListBase::count");
  // Count the number of null pointers. In the STL call count, we must
  // cast the value (NULL) so that template deduction treats it as a
  // pointer rather than an int. Then return the number of non-null
  // pointers, i.e. the size of the container less the number of null
  // pointers.
  int num_non_null=0; 
  for (Impl::VecType::iterator
			itr = itsImpl->itsPtrVec.begin(),
			end = itsImpl->itsPtrVec.end();
		 itr != end;
		 ++itr)
	 {
		if (itr->rcObject()->isValid()) ++num_non_null;
	 }
  
  return num_non_null;
}

bool PtrListBase::isValidId(int id) const {
DOTRACE("PtrListBase::isValidId");

  DebugEval(id);
  DebugEval(id>=0);
  DebugEvalNL(itsImpl->itsPtrVec.size());
  DebugEvalNL(id<itsImpl->itsPtrVec.size());

  return ( id >= 0 && size_t(id) < itsImpl->itsPtrVec.size() &&
			  itsImpl->itsPtrVec[id].rcObject()->isValid() );
}

void PtrListBase::remove(int id) {
DOTRACE("PtrListBase::remove");
  if (!isValidId(id)) return;

  if ( itsImpl->itsPtrVec[id].rcObject()->isShared() )
	 throw ErrorWithMsg("can't remove a shared object");

  release(id);
}

void PtrListBase::release(int id) {
DOTRACE("PtrListBase::release");
  if (!isValidId(id)) return;

  itsImpl->itsPtrVec[id] = VoidPtrHandle();

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void PtrListBase::clear() {
DOTRACE("PtrListBase::clear");
  DebugEvalNL(typeid(*this).name());
  for (size_t i = 0; i < itsImpl->itsPtrVec.size(); ++i) {

	 DebugEval(i); DebugEvalNL(itsImpl->itsPtrVec[i].rcObject()->refCount());
	 DebugEvalNL(itsImpl->itsPtrVec[i].rcObject()->isUnshared());

	 if ( !(itsImpl->itsPtrVec[i].rcObject()->isValid()) )
		{
		  if (itsImpl->itsFirstVacant > i) itsImpl->itsFirstVacant = i;
		}
	 if ( itsImpl->itsPtrVec[i].rcObject()->isValid() &&
			itsImpl->itsPtrVec[i].rcObject()->isUnshared() )
		{
		  release(i);
		}
  }
}

RefCounted* PtrListBase::getPtrBase(int id) const throw () {
DOTRACE("PtrListBase::getPtrBase");
  DebugEvalNL(itsImpl->itsPtrVec[id].rcObject()->refCount());
  return itsImpl->itsPtrVec[id].rcObject();
}

RefCounted* PtrListBase::getCheckedPtrBase(int id) const throw (InvalidIdError) {
DOTRACE("PtrListBase::getCheckedPtrBase");
  if ( !isValidId(id) ) {
	 InvalidIdError err("attempt to access invalid id '");
	 err.appendNumber(id);
	 err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
	 throw err;
  }
  return getPtrBase(id);
}

int PtrListBase::insertPtrBase(RefCounted* ptr) {
DOTRACE("PtrListBase::insertPtrBase");

  int existing_site = itsImpl->findPtr(ptr);

  if (existing_site != -1)
	 return existing_site;

  int new_site = itsImpl->itsFirstVacant;
  itsImpl->insertPtrBaseAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void PtrListBase::Impl::insertPtrBaseAt(int id, RefCounted* ptr) {
DOTRACE("PtrListBase::Impl::insertPtrBaseAt");

  Precondition(ptr != 0);

  DebugEval(id);
  if (id < 0) return;

  size_t uid = size_t(id);

  if (uid >= itsPtrVec.capacity()) {
  	 itsPtrVec.reserve(calculateGrowSize(uid));
  }
  if (uid >= itsPtrVec.size()) {
    itsPtrVec.resize(calculateGrowSize(uid), VoidPtrHandle());
  }

  Assert(itsPtrVec.size() > uid);

  if (itsPtrVec[uid].rcObject()->isValid())
	 {
		InvalidIdError err("object already exists at id '");
		err.appendNumber(id);
		err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
		throw err;
	 }

  // Check to see if we are attempting to insert the same object that
  // is already at location 'id'; if so, we return immediately, since
  // nothing needs to be done (in particular, we had better not delete
  // the "previous" object and then hold on the "new" pointer, since
  // the "new" pointer would then be dangling).
  if ( itsPtrVec[uid].rcObject() == ptr ) return;

  ensureNotDuplicate(ptr);

  //
  // The actual insertion
  //
  itsPtrVec[uid] = VoidPtrHandle(ptr);

  // It is possible that ptr is not valid, in this case, we might need
  // to adjust itsFirstVacant if it is currently beyond than
  // the site that we have just changed.
  if ( !(ptr->isValid()) && id < itsFirstVacant)
	 itsFirstVacant = id;

  // make sure itsFirstVacant is up-to-date
  while ( (size_t(itsFirstVacant) < itsPtrVec.size()) &&
			 (itsPtrVec.at(itsFirstVacant).rcObject()->isValid()) )
	 { ++itsFirstVacant; }

  DebugEvalNL(itsFirstVacant);
}

static const char vcid_ptrlistbase_cc[] = "$Header$";
#endif // !PTRLISTBASE_CC_DEFINED
