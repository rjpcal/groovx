///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Wed Oct 25 10:47:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_CC_DEFINED
#define PTRLISTBASE_CC_DEFINED

#include "ptrlistbase.h"

#include "io/io.h"

#include "system/demangle.h"

#include <typeinfo>
// #include <vector>
#include <map>

// #define OLD_VERSION

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

class DummyObject : public IO::IoObject {
  DummyObject() : IO::IoObject() {}

public:
  virtual ~DummyObject() {}

  virtual void readFrom(IO::Reader*) { Assert(false); }
  virtual void writeTo(IO::Writer*) const { Assert(false); }

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

class IoPtrHandle {
public:
  IoPtrHandle() :
	 itsIoObject(DummyObject::getDummy())
  {
	 Invariant(itsIoObject != 0);
	 itsIoObject->incrRefCount();
  }

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
//   typedef std::vector<IoPtrHandle> VecType;
  typedef std::map<int, IoPtrHandle> VecType;
  VecType itsPtrVec;

  void ensureNotDuplicate(IO::IoObject* ptr);

  int findPtr(IO::IoObject* ptr)
	 {
#ifdef OLD_VERSION
		for (int i = 0; i < itsPtrVec.size(); ++i)
		  {
			 if ( itsPtrVec[i].ioObject() == ptr )
				return i;
		  }
		return -1;
#else
		for (VecType::const_iterator
				 itr = itsPtrVec.begin(),
				 end = itsPtrVec.end();
			  itr != end;
			  ++itr)
		  {
			 if ( (*itr).second.ioObject() == ptr )
				return (*itr).first;
		  }
		return -1;
#endif
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
	 itsPtrVec()
	 {
#ifdef OLD_VERSION
		itsPtrVec.reserve(size);
#endif
	 }

  int capacity() const
	 {
#ifdef OLD_VERSION
		return itsPtrVec.size();
#else
		VecType::const_reverse_iterator itr = itsPtrVec.rbegin();
		if (itr == itsPtrVec.rend()) return 1;
		return itsPtrVec.rbegin()->first + 1;
#endif
	 }

  int count() const
	 {
		// Count the number of null pointers. In the STL call count, we must
		// cast the value (NULL) so that template deduction treats it as a
		// pointer rather than an int. Then return the number of non-null
		// pointers, i.e. the size of the container less the number of null
		// pointers.
		int num_non_null=0; 
		for (VecType::const_iterator
				 itr = itsPtrVec.begin(),
				 end = itsPtrVec.end();
			  itr != end;
			  ++itr)
		  {
#ifdef OLD_VERSION
			 if (itr->ioObject()->isValid()) ++num_non_null;
#else
			 if ((*itr).second.ioObject()->isValid()) ++num_non_null;
#endif
		  }

		return num_non_null;
	 }

  bool isValidId(int id) const
	 {
		DebugEval(id);
		DebugEval(id>=0);
#ifdef OLD_VERSION
		DebugEvalNL(itsPtrVec.size());
		DebugEvalNL(id<itsPtrVec.size());

		return ( id >= 0 && size_t(id) < itsPtrVec.size() &&
					itsPtrVec[id].ioObject()->isValid() );
#else
		VecType::const_iterator itr = itsPtrVec.find(id);
		return ( (itr != itsPtrVec.end()) &&
					((*itr).second.ioObject()->isValid()) );
#endif
	 }

  void release(int id)
	 {
#ifdef OLD_VERSION
		if (!isValidId(id)) return;

		itsPtrVec[id] = IoPtrHandle();

		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;
#else
		VecType::iterator itr = itsPtrVec.find(id);

		itsPtrVec.erase(itr);

		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;		
#endif
	 }

  void remove(int id)
	 {
#ifdef OLD_VERSION
		if (!isValidId(id)) return;

		if ( itsPtrVec[id].ioObject()->isShared() )
		  throw ErrorWithMsg("can't remove a shared object");

		release(id);
#else
		VecType::iterator itr = itsPtrVec.find(id);
		if (itr == itsPtrVec.end()) return;

		if ( (*itr).second.ioObject()->isShared() )
		  throw ErrorWithMsg("can't remove a shared object");

		itsPtrVec.erase(itr);

		// reset itsFirstVacant in case i would now be the first vacant
		if (itsFirstVacant > id) itsFirstVacant = id;		

#endif
	 }

  void clear()
	 {
#ifdef OLD_VERSION
		for (size_t i = 0; i < itsPtrVec.size(); ++i) {

		  DebugEval(i); DebugEvalNL(itsPtrVec[i].ioObject()->refCount());
		  DebugEvalNL(itsPtrVec[i].ioObject()->isUnshared());

		  if ( !(itsPtrVec[i].ioObject()->isValid()) )
			 {
				if (itsFirstVacant > i) itsFirstVacant = i;
			 }
		  if ( itsPtrVec[i].ioObject()->isValid() &&
				 itsPtrVec[i].ioObject()->isUnshared() )
			 {
				release(i);
			 }
		}
#else
		VecType new_map;

		for (VecType::const_iterator
				 itr = itsPtrVec.begin(),
				 end = itsPtrVec.end();
			  itr != end;
			  ++itr)
		  {
			 // If the object is unshared or invalid, we'll be releasing it, so
			 // update itsFirstVacant accordingly
			 if ( (*itr).second.ioObject()->isUnshared() ||
					!((*itr).second.ioObject()->isValid()) )
				{
				  if (itsFirstVacant > (*itr).first) itsFirstVacant = (*itr).first;
				}
			 // ...otherwise, we'll be saving the object, so copy it into
			 // the new_map,
			 else
				{
				  new_map[(*itr).first] = (*itr).second;
				}
		  }

		// Now swap maps so that the old map gets cleared and everything erased
		itsPtrVec.swap(new_map);
#endif
	 }

  IO::IoObject* getCheckedPtrBase(int id) const throw (InvalidIdError)
	 {
#ifdef OLD_VERSION
		if ( !isValidId(id) ) {
		  InvalidIdError err("attempt to access invalid id '");
		  err.appendNumber(id);
		  err.appendMsg("' in ", demangle_cstr(typeid(*itsOwner).name()));
		  throw err;
		}

		DebugEvalNL(itsPtrVec[id].ioObject()->refCount());
		return itsPtrVec[id].ioObject();
#else
		VecType::const_iterator itr = itsPtrVec.find(id);
		if (itr == itsPtrVec.end()) {
		  InvalidIdError err("attempt to access invalid id '");
		  err.appendNumber(id);
		  err.appendMsg("' in ", demangle_cstr(typeid(*itsOwner).name()));
		  throw err;
		}

		return (*itr).second.ioObject();
#endif
	 }

  int insertPtrBase(IO::IoObject* ptr)
	 {
#ifdef OLD_VERSION
		int existing_site = findPtr(ptr);

		if (existing_site != -1)
		  return existing_site;

		int new_site = itsFirstVacant;
		insertPtrBaseAt(new_site, ptr);
		return new_site;              // return the id of the inserted void*
#else
		int existing_site = findPtr(ptr);

		if (existing_site != -1)
		  return existing_site;

		int new_site = itsFirstVacant;
		insertPtrBaseAt(new_site, ptr);
		return new_site;              // return the id of the inserted void*
#endif
	 }
};

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

#ifdef OLD_VERSION
void PtrListBase::Impl::insertPtrBaseAt(int id, IO::IoObject* ptr) {
DOTRACE("PtrListBase::Impl::insertPtrBaseAt");

  Precondition(ptr != 0);

  DebugEval(id);
  if (id < 0) return;

  size_t uid = size_t(id);

  if (uid >= itsPtrVec.capacity()) {
  	 itsPtrVec.reserve(calculateGrowSize(uid));
  }
  if (uid >= itsPtrVec.size()) {
    itsPtrVec.resize(calculateGrowSize(uid), IoPtrHandle());
  }

  Assert(itsPtrVec.size() > uid);

  if (itsPtrVec[uid].ioObject()->isValid())
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
  if ( itsPtrVec[uid].ioObject() == ptr ) return;

  ensureNotDuplicate(ptr);

  //
  // The actual insertion
  //
  itsPtrVec[uid] = IoPtrHandle(ptr);

  // It is possible that ptr is not valid, in this case, we might need
  // to adjust itsFirstVacant if it is currently beyond than
  // the site that we have just changed.
  if ( !(ptr->isValid()) && id < itsFirstVacant)
	 itsFirstVacant = id;

  // make sure itsFirstVacant is up-to-date
  while ( (size_t(itsFirstVacant) < itsPtrVec.size()) &&
			 (itsPtrVec.at(itsFirstVacant).ioObject()->isValid()) )
	 { ++itsFirstVacant; }

  DebugEvalNL(itsFirstVacant);
}
#else
void PtrListBase::Impl::insertPtrBaseAt(int id, IO::IoObject* ptr) {
DOTRACE("PtrListBase::Impl::insertPtrBaseAt");

  Precondition(ptr != 0);

  DebugEval(id);
  if (id < 0) return;

  VecType::iterator itr = itsPtrVec.find(id);

  if (itr != itsPtrVec.end())
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
  if ( ptr->isValid() )
	 itsPtrVec[id] = IoPtrHandle(ptr);

  // make sure itsFirstVacant is up-to-date
  while ( itsPtrVec.find(itsFirstVacant) != itsPtrVec.end() )
	 { ++itsFirstVacant; }

  DebugEvalNL(itsFirstVacant);
}
#endif

///////////////////////////////////////////////////////////////////////
//
// PtrListBase member definitions
//
///////////////////////////////////////////////////////////////////////

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
