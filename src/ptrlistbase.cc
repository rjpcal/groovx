///////////////////////////////////////////////////////////////////////
//
// voidptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Fri Oct  6 13:42:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_CC_DEFINED
#define VOIDPTRLIST_CC_DEFINED

#include "voidptrlist.h"

#include "system/demangle.h"

#include <algorithm> // for std::count
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


  class MasterVoidPtr {
  private:
	 VoidPtrList* itsList;
	 void* itsPtr;

	 void destroy()
	 {
		itsList->destroyPtr(itsPtr);
	 }

	 void swap(MasterVoidPtr& other)
	 {
		VoidPtrList* otherList = other.itsList;
		other.itsList = this->itsList;
		this->itsList = otherList;

		void* otherPtr = other.itsPtr;
		other.itsPtr = this->itsPtr;
		this->itsPtr = otherPtr;
	 }

  public:
	 MasterVoidPtr(VoidPtrList* vpl) : itsList(vpl), itsPtr(0) {}

    MasterVoidPtr (const MasterVoidPtr& other) :
		itsList(other.itsList),
		itsPtr(other.itsPtr)
	 {}

    MasterVoidPtr& operator=(const MasterVoidPtr& other)
	 {
		MasterVoidPtr otherCopy(other);

		this->swap(otherCopy);

		return *this;
	 }


	 // XXX Should we destroy here ?
	 virtual ~MasterVoidPtr() { /* destroy(); */ }

	 void* ptr() const
	 {
		DebugEvalNL(itsPtr);
		return itsPtr;
	 }

	 // Set pointer to 0, but do not destroy
	 void release() { itsPtr = 0; }

	 void reset(void* new_address)
	 {
		DebugEval(itsPtr);
		destroy();
		DebugPrintNL("...destroyed");
		itsPtr = new_address;
	 }

	 bool isValid() const
	 {
		DebugEvalNL(itsPtr);
		DebugEvalNL(itsPtr != 0);
		return itsPtr != 0;
	 }

	 bool operator==(const void* other)
	 { return itsPtr == other; }

//  	 int refCount;
  };

  int itsFirstVacant;
  std::vector<MasterVoidPtr> itsPtrVec;

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

void VoidPtrList::incrRefCount(int id) const {
DOTRACE("VoidPtrList::incrRefCount");
//    if (id > 0 && id < itsImpl->itsPtrVec.size())
//  	 ++(itsImpl->itsPtrVec[id].refCount);
}

void VoidPtrList::decrRefCount(int id) const {
DOTRACE("VoidPtrList::decrRefCount");
//    if (id > 0 && id < itsImpl->itsPtrVec.size())
//  	 --(itsImpl->itsPtrVec[id].refCount); 
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
  for (std::vector<Impl::MasterVoidPtr>::iterator
			itr = itsImpl->itsPtrVec.begin(),
			end = itsImpl->itsPtrVec.end();
		 itr != end;
		 ++itr)
	 {
		if (itr->isValid()) ++num_non_null;
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
			  itsImpl->itsPtrVec[id].isValid() );
}

void VoidPtrList::remove(int id) {
DOTRACE("VoidPtrList::remove");
  if (!isValidId(id)) return;

  itsImpl->itsPtrVec[id].reset(0);

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;
}

void VoidPtrList::clear() {
DOTRACE("VoidPtrList::clear");
  DebugEvalNL(typeid(*this).name());
  for (size_t i = 0; i < itsImpl->itsPtrVec.size(); ++i) {
	 DebugEval(i);
	 itsImpl->itsPtrVec[i].reset(0);
  }

  itsImpl->itsPtrVec.resize(0, Impl::MasterVoidPtr(this));

  itsImpl->itsFirstVacant = 0;
}

void* VoidPtrList::getVoidPtr(int id) const throw () {
DOTRACE("VoidPtrList::getVoidPtr");
  return itsImpl->itsPtrVec[id].ptr(); 
}

void* VoidPtrList::getCheckedVoidPtr(int id) const throw (InvalidIdError) {
DOTRACE("VoidPtrList::getCheckedVoidPtr");
  if ( !isValidId(id) ) {
	 InvalidIdError err("attempt to access invalid id '");
	 err.appendNumber(id);
	 err.appendMsg("' in ", demangle_cstr(typeid(*this).name()));
	 throw err;
  }
  return getVoidPtr(id);
}

void* VoidPtrList::releaseVoidPtr(int id) throw (InvalidIdError) {
DOTRACE("VoidPtrList::releaseVoidPtr");
  void* ptr = getCheckedVoidPtr(id);
  itsImpl->itsPtrVec[id].release();

  // reset itsImpl->itsFirstVacant in case i would now be the first vacant
  if (itsImpl->itsFirstVacant > id) itsImpl->itsFirstVacant = id;

  DebugEvalNL(itsImpl->itsFirstVacant);

  return ptr;
}

int VoidPtrList::insertVoidPtr(void* ptr) {
DOTRACE("VoidPtrList::insertVoidPtr");
  int new_site = itsImpl->itsFirstVacant;
  insertVoidPtrAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void VoidPtrList::insertVoidPtrAt(int id, void* ptr) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
  if (id < 0) return;

  size_t uid = size_t(id);

  if (uid >= itsImpl->itsPtrVec.capacity()) {
	 itsImpl->itsPtrVec.reserve(uid+RESERVE_CHUNK);
  }
  if (uid >= itsImpl->itsPtrVec.size()) {
    itsImpl->itsPtrVec.resize(uid+1, Impl::MasterVoidPtr(this));
  }

  Assert(itsImpl->itsPtrVec.size() > uid);

  if (itsImpl->itsPtrVec[uid].isValid())
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
  if (itsImpl->itsPtrVec[uid] == ptr) return;

  itsImpl->itsPtrVec[uid].reset(ptr);

  // It is possible that ptr is NULL, in this case, we might need to
  // adjust itsImpl->itsFirstVacant if it is currently beyond than the site
  // that we have just changed.
  if (ptr == NULL && id < itsImpl->itsFirstVacant)
	 itsImpl->itsFirstVacant = id;

  // make sure itsImpl->itsFirstVacant is up-to-date
  while ( (size_t(itsImpl->itsFirstVacant) < itsImpl->itsPtrVec.size()) &&
			 (itsImpl->itsPtrVec.at(itsImpl->itsFirstVacant).isValid()) )
	 { ++(itsImpl->itsFirstVacant); }

  afterInsertHook(id, ptr);

  DebugEvalNL(itsImpl->itsFirstVacant);
}

void VoidPtrList::afterInsertHook(int /* id */, void* /* ptr */) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
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
	 itsImpl->itsPtrVec.resize(new_size, Impl::MasterVoidPtr(this));
}

static const char vcid_voidptrlist_cc[] = "$Header$";
#endif // !VOIDPTRLIST_CC_DEFINED
