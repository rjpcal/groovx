///////////////////////////////////////////////////////////////////////
//
// voidptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sat Oct  7 11:46:42 2000
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
// MasterVoidPtr member definitions
//
///////////////////////////////////////////////////////////////////////

void MasterVoidPtr::swap(MasterVoidPtr& other)
{
DOTRACE("MasterVoidPtr::swap");
  VoidPtrList* otherList = other.itsList;
  other.itsList = this->itsList;
  this->itsList = otherList;

  void* otherPtr = other.itsPtr;
  other.itsPtr = this->itsPtr;
  this->itsPtr = otherPtr;
}

MasterVoidPtr::MasterVoidPtr(VoidPtrList* vpl, void* address) :
  itsList(vpl),
  itsPtr(address)
{
DOTRACE("MasterVoidPtr::MasterVoidPtr(VoidPtrList*, void*)");
}

MasterVoidPtr::MasterVoidPtr (const MasterVoidPtr& other) :
  itsList(other.itsList),
  itsPtr(other.itsPtr)
{
DOTRACE("MasterVoidPtr::MasterVoidPtr(const MasterVoidPtr&)");
}

MasterVoidPtr& MasterVoidPtr::operator=(const MasterVoidPtr& other) {
DOTRACE("MasterVoidPtr::operator=");
  MasterVoidPtr otherCopy(other);

  this->swap(otherCopy);

  return *this;
}

MasterVoidPtr::~MasterVoidPtr()
{
DOTRACE("MasterVoidPtr::~MasterVoidPtr");
  if (itsPtr != 0) itsList->destroyPtr(itsPtr);
  itsPtr = 0;
}

void* MasterVoidPtr::ptr() const
{
  DebugEvalNL(itsPtr);
  return itsPtr;
}

bool MasterVoidPtr::isValid() const
{
DOTRACE("MasterVoidPtr::isValid");
  DebugEvalNL(itsPtr);
  DebugEvalNL(itsPtr != 0);
  return itsPtr != 0;
}

class NullMasterPtr : public MasterVoidPtr {
public:
  NullMasterPtr(VoidPtrList* vpl) :
	 MasterVoidPtr(vpl, 0) {}

  virtual ~NullMasterPtr() {}

  virtual bool itsValid() const { return false; }
};

///////////////////////////////////////////////////////////////////////
//
// VoidPtrHandle
//
///////////////////////////////////////////////////////////////////////


class VoidPtrHandle {
public:
  explicit VoidPtrHandle(VoidPtrList* vlist) :
	 itsMaster(new NullMasterPtr(vlist)),
	 itsRefCount(new int(1))
  {
	 Assert(itsMaster != 0);
  }

  explicit VoidPtrHandle(MasterVoidPtr* master) :
	 itsMaster(master),
	 itsRefCount(new int(1))
  {
	 Assert(itsMaster != 0);
  }

  ~VoidPtrHandle()
  {
	 Assert(itsMaster != 0);

	 --*itsRefCount;

	 if (*itsRefCount <= 0)
		{
		  delete itsRefCount;
		  delete itsMaster;
		}
  }

  VoidPtrHandle(const VoidPtrHandle& other) :
	 itsMaster(other.itsMaster),
	 itsRefCount(other.itsRefCount)
  {
	 Assert(itsMaster != 0);
	 ++*itsRefCount;
  }

  VoidPtrHandle& operator=(const VoidPtrHandle& other)
  {
	 VoidPtrHandle otherCopy(other);
	 this->swap(otherCopy);
	 Assert(itsMaster != 0);
	 return *this;
  }

  MasterVoidPtr* masterPtr()
  {
	 Assert(itsMaster != 0);
	 return itsMaster;
  }

private:
  void swap(VoidPtrHandle& other)
  {
	 MasterVoidPtr* otherMaster = other.itsMaster;
	 other.itsMaster = this->itsMaster;
	 this->itsMaster = otherMaster;

	 int* otherRefCount = other.itsRefCount;
	 other.itsRefCount = this->itsRefCount;
	 this->itsRefCount = otherRefCount;
  }

  MasterVoidPtr* itsMaster;

  int* itsRefCount;
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

  itsImpl->itsPtrVec[id] = VoidPtrHandle(new MasterVoidPtr(this, 0));

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void VoidPtrList::clear() {
DOTRACE("VoidPtrList::clear");
  DebugEvalNL(typeid(*this).name());
  for (size_t i = 0; i < itsImpl->itsPtrVec.size(); ++i) {
	 DebugEval(i);
  	 itsImpl->itsPtrVec[i] = VoidPtrHandle(new MasterVoidPtr(this, 0));
  }

  itsImpl->itsPtrVec.resize(0, VoidPtrHandle(this));

  itsImpl->itsFirstVacant = 0;
}

MasterVoidPtr* VoidPtrList::getVoidPtr(int id) const throw () {
DOTRACE("VoidPtrList::getVoidPtr");
  return itsImpl->itsPtrVec[id].masterPtr();
}

MasterVoidPtr* VoidPtrList::getCheckedVoidPtr(int id) const throw (InvalidIdError) {
DOTRACE("VoidPtrList::getCheckedVoidPtr");
  if ( !isValidId(id) ) {
	 InvalidIdError err("attempt to access invalid id '");
	 err.appendNumber(id);
	 err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
	 throw err;
  }
  return getVoidPtr(id);
}

int VoidPtrList::insertVoidPtr(MasterVoidPtr* ptr) {
DOTRACE("VoidPtrList::insertVoidPtr");
  int new_site = itsImpl->itsFirstVacant;
  insertVoidPtrAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void VoidPtrList::insertVoidPtrAt(int id, MasterVoidPtr* ptr) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
  DebugEval(id); DebugEvalNL(ptr->ptr());
  if (id < 0) return;

  size_t uid = size_t(id);

  if (uid >= itsImpl->itsPtrVec.capacity()) {
	 itsImpl->itsPtrVec.reserve(uid+RESERVE_CHUNK);
  }
  if (uid >= itsImpl->itsPtrVec.size()) {
    itsImpl->itsPtrVec.resize(uid+1, VoidPtrHandle(this));
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

void VoidPtrList::afterInsertHook(int /* id */, MasterVoidPtr* /* ptr */) {
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
	 itsImpl->itsPtrVec.resize(new_size, VoidPtrHandle(this));
}

static const char vcid_voidptrlist_cc[] = "$Header$";
#endif // !VOIDPTRLIST_CC_DEFINED
