///////////////////////////////////////////////////////////////////////
//
// ptrlistbase.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Mon Oct  9 19:48:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLISTBASE_CC_DEFINED
#define PTRLISTBASE_CC_DEFINED

#include "ptrlistbase.h"

#include "masterptr.h"

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
  const int RESERVE_CHUNK = 20;
}

///////////////////////////////////////////////////////////////////////
//
// VoidPtrHandle
//
///////////////////////////////////////////////////////////////////////


class VoidPtrHandle {
public:
  VoidPtrHandle() :
	 itsMaster(new NullMasterPtr)
  {
	 Invariant(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  explicit VoidPtrHandle(MasterPtrBase* master) :
	 itsMaster(master)
  {
	 Invariant(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  ~VoidPtrHandle()
  {
	 Invariant(itsMaster != 0);
	 itsMaster->decrRefCount();
  }

  VoidPtrHandle(const VoidPtrHandle& other) :
	 itsMaster(other.itsMaster)
  {
	 Invariant(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  VoidPtrHandle& operator=(const VoidPtrHandle& other)
  {
	 VoidPtrHandle otherCopy(other);
	 this->swap(otherCopy);
	 Invariant(itsMaster != 0);
	 return *this;
  }

  MasterPtrBase* masterPtr()
  {
	 Invariant(itsMaster != 0);
	 return itsMaster;
  }

private:
  void swap(VoidPtrHandle& other)
  {
	 MasterPtrBase* otherMaster = other.itsMaster;
	 other.itsMaster = this->itsMaster;
	 this->itsMaster = otherMaster;
  }

  MasterPtrBase* itsMaster;
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

private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);
};

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
		if (itr->masterPtr()->isValid()) ++num_non_null;
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
			  itsImpl->itsPtrVec[id].masterPtr()->isValid() );
}

void PtrListBase::remove(int id) {
DOTRACE("PtrListBase::remove");
  if (!isValidId(id)) return;

  if ( itsImpl->itsPtrVec[id].masterPtr()->isShared() )
	 throw ErrorWithMsg("can't remove a shared object");

  itsImpl->itsPtrVec[id] = VoidPtrHandle(new NullMasterPtr);

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void PtrListBase::release(int id) {
DOTRACE("PtrListBase::release");
  if (!isValidId(id)) return;

  itsImpl->itsPtrVec[id] = VoidPtrHandle(new NullMasterPtr);

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void PtrListBase::clear() {
DOTRACE("PtrListBase::clear");
  DebugEvalNL(typeid(*this).name());
  for (size_t i = 0; i < itsImpl->itsPtrVec.size(); ++i) {
	 DebugEval(i);
	 if ( itsImpl->itsPtrVec[i].masterPtr()->isUnshared() )
		itsImpl->itsPtrVec[i] = VoidPtrHandle(new NullMasterPtr);
  }

  itsImpl->itsPtrVec.resize(0, VoidPtrHandle());

  itsImpl->itsFirstVacant = 0;
}

MasterPtrBase* PtrListBase::getPtrBase(int id) const throw () {
DOTRACE("PtrListBase::getPtrBase");
  return itsImpl->itsPtrVec[id].masterPtr();
}

MasterPtrBase* PtrListBase::getCheckedPtrBase(int id) const throw (InvalidIdError) {
DOTRACE("PtrListBase::getCheckedPtrBase");
  if ( !isValidId(id) ) {
	 InvalidIdError err("attempt to access invalid id '");
	 err.appendNumber(id);
	 err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
	 throw err;
  }
  return getPtrBase(id);
}

int PtrListBase::insertPtrBase(MasterPtrBase* ptr) {
DOTRACE("PtrListBase::insertPtrBase");
  int new_site = itsImpl->itsFirstVacant;
  insertPtrBaseAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void PtrListBase::insertPtrBaseAt(int id, MasterPtrBase* ptr) {
DOTRACE("PtrListBase::insertPtrBaseAt");
  DebugEval(id); DebugEvalNL(ptr->ptr());
  if (id < 0) return;

  size_t uid = size_t(id);

  if (uid >= itsImpl->itsPtrVec.capacity()) {
	 itsImpl->itsPtrVec.reserve(uid+RESERVE_CHUNK);
  }
  if (uid >= itsImpl->itsPtrVec.size()) {
    itsImpl->itsPtrVec.resize(uid+1, VoidPtrHandle());
  }

  Assert(itsImpl->itsPtrVec.size() > uid);

  if (itsImpl->itsPtrVec[uid].masterPtr()->isValid())
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
  if ( *(itsImpl->itsPtrVec[uid].masterPtr()) == *ptr) return;

  itsImpl->itsPtrVec[uid] = VoidPtrHandle(ptr);

  // It is possible that ptr is not valid, in this case, we might need
  // to adjust itsImpl->itsFirstVacant if it is currently beyond than
  // the site that we have just changed.
  if ( !(ptr->isValid()) && id < itsImpl->itsFirstVacant)
	 itsImpl->itsFirstVacant = id;

  // make sure itsImpl->itsFirstVacant is up-to-date
  while ( (size_t(itsImpl->itsFirstVacant) < itsImpl->itsPtrVec.size()) &&
			 (itsImpl->itsPtrVec.at(itsImpl->itsFirstVacant).masterPtr()->isValid()) )
	 { ++(itsImpl->itsFirstVacant); }

  afterInsertHook(id, ptr);

  DebugEvalNL(itsImpl->itsFirstVacant);
}

void PtrListBase::afterInsertHook(int /* id */, MasterPtrBase* /* ptr */) {
DOTRACE("PtrListBase::afterInsertHook");
}

int& PtrListBase::firstVacant() {
  return itsImpl->itsFirstVacant;
}

const int& PtrListBase::firstVacant() const {
  return itsImpl->itsFirstVacant;
}

unsigned int PtrListBase::baseVecSize() const {
DOTRACE("PtrListBase::baseVecSize");
  return itsImpl->itsPtrVec.size();
}

void PtrListBase::baseVecResize(unsigned int new_size) {
DOTRACE("PtrListBase::baseVecResize");
  if ( new_size > itsImpl->itsPtrVec.size() )
	 itsImpl->itsPtrVec.resize(new_size, VoidPtrHandle());
}

static const char vcid_ptrlistbase_cc[] = "$Header$";
#endif // !PTRLISTBASE_CC_DEFINED
