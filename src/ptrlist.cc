///////////////////////////////////////////////////////////////////////
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Sun Apr 25 21:10:54 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <strstream.h>

#include "iomgr.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

// Explicit template instantiation requests
#include "grobj.h"
#include "position.h"
template class PtrList<GrObj>;
template class PtrList<Position>;

///////////////////////////////////////////////////////////////////////
// File scope declarations
///////////////////////////////////////////////////////////////////////

namespace {
  const int BUF_SIZE = 200;
  const int RESIZE_CHUNK = 20;
}

///////////////////////////////////////////////////////////////////////
// PtrList member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

template <class T>
PtrList::PtrList(int size):
  itsFirstVacant(0), 
  itsVec(size, NULL) 
{
DOTRACE("PtrList::PtrList");
#ifdef LOCAL_DEBUG
  DUMP_VAL2(itsVec.size());
#endif
}

template <class T>
PtrList::~PtrList() {
DOTRACE("PtrList::~PtrList");
  clear();
}

template <class T>
void PtrList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("PtrList::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(PtrList).name() << sep; }

  // itsVec: we will serialize only the non-null T*'s in
  // itsVec. In order to correctly deserialize the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << itsVec.size() << sep;
  int num_null = 0;
  for (vector<void *>::const_iterator ii = itsVec.begin(); 
       ii != itsVec.end(); 
       ii++) {
    if (*ii == NULL) num_null++;
  }
  int num_non_null = itsVec.size() - num_null;
  os << num_non_null << endl;
  int c = 0;
  for (int i = 0; i < itsVec.size(); i++) {
    if (itsVec[i] != NULL) {
      os << i << sep;
      // we must serialize the typename since deserialize requires a
      // typename in order to call the virtual constructor
      static_cast<T *>(itsVec[i])->serialize(os, flag|TYPENAME);
      c++;
    }
  }
  if (c != num_non_null) {
	 throw IoLogicError(typeid(*this));
  }
  // itsFirstVacant
  os << itsFirstVacant << endl;
  if (os.fail()) throw OutputError(typeid(*this));
}

template <class T>
void PtrList::deserialize(istream &is, IOFlag flag) {
DOTRACE("PtrList::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(PtrList).name() && name != typeid(*this).name()) { 
#ifdef LOCAL_DEBUG
		DUMP_VAL1(name);
		DUMP_VAL1(typeid(PtrList).name());
		DUMP_VAL2(typeid(*this).name());
#endif
		throw InputError(typeid(*this));
	 }
  }
  // itsVec
  clear();
  int size, num_non_null;
  is >> size >> num_non_null;
  // We must check if the istream has failed in order to avoid
  // attempting to resize itsVec to some crazy size.
  if (is.fail()) throw InputError(typeid(*this));
  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IoValueError(typeid(*this));
  }
  itsVec.resize(size, NULL);
  int ptrid;
  for (int i = 0; i < num_non_null; i++) {
    is >> ptrid;
	 if (ptrid < 0 || ptrid >= size) {
		throw IoValueError(typeid(*this));
	 }
	 T* temp = dynamic_cast<T *>(IoMgr::newIO(is, flag));
	 if (!temp) throw InputError(typeid(T));
    itsVec[ptrid] = temp;
  }
  // itsFirstVacant
  is >> itsFirstVacant;
  if (itsFirstVacant < 0) {
	 throw IoValueError(typeid(*this));
  }
  if (is.fail()) throw InputError(typeid(*this));
}

///////////////
// accessors //
///////////////

template <class T>
int PtrList::count() const {
DOTRACE("PtrList::count"); 
  int count=0;
  for (vector<void *>::const_iterator ii = itsVec.begin(); 
       ii != itsVec.end(); ii++) {
    if (*ii != NULL) count++;
  }
  return count;
}

template <class T>
bool PtrList::isValidId(int id) const {
DOTRACE("PtrList::isValidId");
#ifdef LOCAL_DEBUG
  DUMP_VAL1(id);
  DUMP_VAL1(id>=0);
  DUMP_VAL2(itsVec.size());
  DUMP_VAL2(itsVec[id]);
  DUMP_VAL2(id<itsVec.size());
  DUMP_VAL2(itsVec[id] != NULL);
#endif
  return ( id >= 0 && id < itsVec.size() && itsVec[id] != NULL); 
}

template <class T>
void PtrList::getValidIds(vector<int>& vec) const {
DOTRACE("PtrList::getValidIds");
  vec.clear();
  for (int i = 0; i < itsVec.size(); i++) {
    if (isValidId(i)) vec.push_back(i);
  }
}

//////////////////
// manipulators //
//////////////////

template <class T>
int PtrList::insert(T *ptr) {
DOTRACE("PtrList::insert");
  int new_site = itsFirstVacant;
  insertAt(new_site, ptr);
  return new_site;              // return the id of the inserted T*
}

template <class T>
void PtrList::insertAt(int id, T *ptr) {
DOTRACE("PtrList::insertAt");
  if (id < 0) return;

  if (id >= itsVec.size()) {
    itsVec.resize(id+RESIZE_CHUNK, NULL);
  }

  delete static_cast<T *>(itsVec[id]);
  itsVec[id] = ptr;

  // make sure itsFirstVacant is up-to-date
  while ( (itsVec[itsFirstVacant] != NULL) &&
          (++itsFirstVacant < itsVec.size()) );
}

// this member function delete's the T pointed to by itsVec[i]
template <class T>
void PtrList::remove(int id) {
DOTRACE("PtrList::remove");
  if (!isValidId(id)) return;

  delete static_cast<T *>(itsVec[id]);
  itsVec[id] = NULL;         // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

template <class T>
void PtrList::clear() {
DOTRACE("PtrList::clear()");
  for (vector<void *>::iterator ii = itsVec.begin(); 
		 ii != itsVec.end(); 
		 ii++) {
    delete static_cast<T *>(*ii);
    *ii = NULL;
  }
  itsFirstVacant = 0;
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
