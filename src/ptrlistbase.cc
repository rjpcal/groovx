///////////////////////////////////////////////////////////////////////
//
// voidptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sun Nov 21 01:26:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_CC_DEFINED
#define VOIDPTRLIST_CC_DEFINED

#include "voidptrlist.h"

#include <algorithm> // for ::count

#include "trace.h"
#include "debug.h"


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
// VoidPtrList member definitions
//
///////////////////////////////////////////////////////////////////////

VoidPtrList::VoidPtrList(int size) :
  itsFirstVacant(0),
  itsVec()
{
DOTRACE("VoidPtrList::VoidPtrList");

  itsVec.reserve(size); 
}

VoidPtrList::~VoidPtrList() {
DOTRACE("VoidPtrList::~VoidPtrList");
}

int VoidPtrList::capacity() const {
DOTRACE("VoidPtrList::capacity");
}

int VoidPtrList::count() const {
DOTRACE("VoidPtrList::count");
  // Count the number of null pointers. In the STL call count, we must
  // cast the value (NULL) so that template deduction treats it as a
  // pointer rather than an int. Then return the number of non-null
  // pointers, i.e. the size of the container less the number of null
  // pointers.
  int num_null=0;
  ::count(itsVec.begin(), itsVec.end(),
			 static_cast<void *>(NULL), num_null);
  return (itsVec.size() - num_null);
}

bool VoidPtrList::isValidId(int id) const {
DOTRACE("VoidPtrList::isValidId");

  DebugEval(id);
  DebugEval(id>=0);
  DebugEvalNL(itsVec.size());
  DebugEvalNL(itsVec[id]);
  DebugEvalNL(id<itsVec.size());
  DebugEvalNL(itsVec[id] != NULL);

  return ( id >= 0 && id < itsVec.size() && itsVec[id] != NULL ); 
}

void VoidPtrList::remove(int id) {
DOTRACE("VoidPtrList::remove");
  if (!isValidId(id)) return;

  destroyPtr(itsVec[id]);
  itsVec[id] = NULL;         // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

void VoidPtrList::clear() {
DOTRACE("VoidPtrList::clear");
  for (int i = 0; i < itsVec.size(); ++i) {
	 DebugEval(i); DebugEvalNL(itsVec.size());
	 destroyPtr(itsVec[i]);
	 itsVec[i] = NULL;
  }

  itsVec.resize(0);

  itsFirstVacant = 0;
}

void* VoidPtrList::getVoidPtr(int id) const throw () {
DOTRACE("VoidPtrList::getVoidPtr");
  return itsVec[id]; 
}

void* VoidPtrList::getCheckedVoidPtr(int id) const throw (InvalidIdError) {
DOTRACE("VoidPtrList::getCheckedVoidPtr");
  if ( !isValidId(id) ) { throw InvalidIdError(); }
  return getVoidPtr(id);
}

int VoidPtrList::insertVoidPtr(void* ptr) {
DOTRACE("VoidPtrList::insertVoidPtr");
  int new_site = itsFirstVacant;
  insertVoidPtrAt(new_site, ptr);
  return new_site;              // return the id of the inserted void*
}

void VoidPtrList::insertVoidPtrAt(int id, void* ptr) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
  if (id < 0) return;

  if (id >= itsVec.capacity()) {
	 itsVec.reserve(id+RESERVE_CHUNK);
  }
  if (id >= itsVec.size()) {
    itsVec.resize(id+1, NULL);
  }

  // Check to see if we are attempting to insert the same object that
  // is already at location 'id'; if so, we return immediately, since
  // nothing needs to be done (in particular, we had better not delete
  // the "previous" object and then hold on the "new" pointer, since
  // the "new" pointer would then be dangling).
  if (itsVec[id] == ptr) return;

  destroyPtr(itsVec[id]);
  itsVec[id] = ptr;

  // It is possible that ptr is NULL, in this case, we might need to
  // adjust itsFirstVacant if it is currently beyond than the site
  // that we have just changed.
  if (ptr == NULL && id < itsFirstVacant)
	 itsFirstVacant = id;

  // make sure itsFirstVacant is up-to-date
  while ( (itsVec[itsFirstVacant] != NULL) &&
          (++itsFirstVacant < itsVec.size()) );

  afterInsertHook(id, ptr);
}

void VoidPtrList::afterInsertHook(int /* id */, void* /* ptr */) {
DOTRACE("VoidPtrList::insertVoidPtrAt");
}

static const char vcid_voidptrlist_cc[] = "$Header$";
#endif // !VOIDPTRLIST_CC_DEFINED
