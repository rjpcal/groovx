///////////////////////////////////////////////////////////////////////
//
// voidptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sun Oct  8 16:26:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_CC_DEFINED
#define VOIDPTRLIST_CC_DEFINED

#include "voidptrlist.h"

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
// MasterPtrBase member definitions
//
///////////////////////////////////////////////////////////////////////

MasterPtrBase::MasterPtrBase() :
  itsRefCount(0)
{
DOTRACE("MasterPtrBase::MasterPtrBase");
}

MasterPtrBase::~MasterPtrBase()
{
DOTRACE("MasterPtrBase::~MasterPtrBase");
}

void MasterPtrBase::incrRefCount() {
DOTRACE("MasterPtrBase::incrRefCount");
  ++itsRefCount;
}

void MasterPtrBase::decrRefCount() {
DOTRACE("MasterPtrBase::decrRefCount");
  --itsRefCount;
  if (itsRefCount <= 0)
	 delete this;
}

int MasterPtrBase::refCount() const {
DOTRACE("MasterPtrBase::refCount");
  return itsRefCount;
}

class NullMasterPtr : public MasterPtrBase {
public:
  NullMasterPtr() :
	 MasterPtrBase() {}

  virtual ~NullMasterPtr() {}

  virtual bool isValid() const { return false; }

  virtual bool operator==(const MasterPtrBase& other)
  { 
	 bool otherIsNull = (dynamic_cast<const NullMasterPtr*>(&other) != 0);
	 return otherIsNull;
  }
};

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
	 Assert(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  explicit VoidPtrHandle(MasterPtrBase* master) :
	 itsMaster(master)
  {
	 Assert(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  ~VoidPtrHandle()
  {
	 Assert(itsMaster != 0);
	 itsMaster->decrRefCount();
  }

  VoidPtrHandle(const VoidPtrHandle& other) :
	 itsMaster(other.itsMaster)
  {
	 Assert(itsMaster != 0);
	 itsMaster->incrRefCount();
  }

  VoidPtrHandle& operator=(const VoidPtrHandle& other)
  {
	 VoidPtrHandle otherCopy(other);
	 this->swap(otherCopy);
	 Assert(itsMaster != 0);
	 return *this;
  }

  MasterPtrBase* masterPtr()
  {
	 Assert(itsMaster != 0);
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
// VoidPtrList::Impl definition
//
///////////////////////////////////////////////////////////////////////

class VoidPtrList::Impl {
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
// VoidPtrList member definitions
//
///////////////////////////////////////////////////////////////////////

VoidPtrList::VoidPtrList(int size) :
  itsImpl(new Impl(size))
{
DOTRACE("VoidPtrList::VoidPtrList");
}

VoidPtrList::~VoidPtrList() {
DOTRACE("VoidPtrList::~VoidPtrList");
  delete itsImpl; 
}

int VoidPtrList::capacity() const {
DOTRACE("VoidPtrList::capacity");
  return itsImpl->itsPtrVec.size(); 
}

int VoidPtrList::count() const {
DOTRACE("VoidPtrList::count");
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

bool VoidPtrList::isValidId(int id) const {
DOTRACE("VoidPtrList::isValidId");

  DebugEval(id);
  DebugEval(id>=0);
  DebugEvalNL(itsImpl->itsPtrVec.size());
  DebugEvalNL(id<itsImpl->itsPtrVec.size());

  return ( id >= 0 && size_t(id) < itsImpl->itsPtrVec.size() &&
			  itsImpl->itsPtrVec[id].masterPtr()->isValid() );
}

void VoidPtrList::remove(int id) {
DOTRACE("VoidPtrList::remove");
  if (!isValidId(id)) return;

  itsImpl->itsPtrVec[id] = VoidPtrHandle(new NullMasterPtr);

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void VoidPtrList::clear() {
DOTRACE("VoidPtrList::clear");
  DebugEvalNL(typeid(*this).name());
  for (size_t i = 0; i < itsImpl->itsPtrVec.size(); ++i) {
	 DebugEval(i);
  	 itsImpl->itsPtrVec[i] = VoidPtrHandle(new NullMasterPtr);
  }

  itsImpl->itsPtrVec.resize(0, VoidPtrHandle());

  itsImpl->itsFirstVacant = 0;
}

MasterPtrBase* VoidPtrList::getVoidPtr(int id) const throw () {
DOTRACE("VoidPtrList::getVoidPtr");
  return itsImpl->itsPtrVec[id].masterPtr();
}

MasterPtrBase* VoidPtrList::getCheckedVoidPtr(int id) const throw (InvalidIdError) {
DOTRACE("VoidPtrList::getCheckedVoidPtr");
  if ( !isValidId(id) ) {
	 InvalidIdError err("attempt to access invalid id '");
	 err.appendNumber(id);
	 err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
	 throw err;
  }
  return getVoidPtr(id);
}

int VoidPtrList::insertVoidPtr(MasterPtrBase* ptr) {
DOTRACE("VoidPtrList::insertVoidPtr");
  int new_site = itsImpl->itsFirstVacant;
  insertVoidPtrAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void VoidPtrList::insertVoidPtrAt(int id, MasterPtrBase* ptr) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
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

void VoidPtrList::afterInsertHook(int /* id */, MasterPtrBase* /* ptr */) {
DOTRACE("VoidPtrList::afterInsertHook");
}

int& VoidPtrList::firstVacant() {
  return itsImpl->itsFirstVacant;
}

const int& VoidPtrList::firstVacant() const {
  return itsImpl->itsFirstVacant;
}

unsigned int VoidPtrList::voidVecSize() const {
DOTRACE("VoidPtrList::voidVecEnd");
  return itsImpl->itsPtrVec.size();
}

void VoidPtrList::voidVecResize(unsigned int new_size) {
DOTRACE("VoidPtrList::voidVecResize");
  if ( new_size > itsImpl->itsPtrVec.size() )
	 itsImpl->itsPtrVec.resize(new_size, VoidPtrHandle());
}

static const char vcid_voidptrlist_cc[] = "$Header$";
#endif // !VOIDPTRLIST_CC_DEFINED
