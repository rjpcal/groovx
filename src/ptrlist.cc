///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Mon Sep 27 11:03:13 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <strstream.h>

#include "iomgr.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  const int RESIZE_CHUNK = 20;
}

///////////////////////////////////////////////////////////////////////
//
// PtrList member functions
//
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

  DebugEvalNL(itsVec.size());

}

template <class T>
PtrList::~PtrList() {
DOTRACE("PtrList::~PtrList");
  clear();
}

template <class T>
void PtrList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("PtrList::serialize");
  static string ioTag = typeid(PtrList).name();	 

  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(PtrList).name() << sep; }

  // itsVec: we will serialize only the non-null T*'s in
  // itsVec. In order to correctly deserialize the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << itsVec.size() << sep;

  int num_non_null = PtrList::count();
  os << num_non_null << endl;

  // Serialize all non-null ptr's.
  int c = 0;
  for (int i = 0; i < itsVec.size(); ++i) {
    if (itsVec[i] != NULL) {
      os << i << sep;
      // we must serialize the typename since deserialize requires a
      // typename in order to call the virtual constructor
      itsVec[i]->serialize(os, flag|TYPENAME);
      ++c;
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
  static string ioTag = typeid(PtrList).name();	 

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { 
	 IO::readTypename(is, string(typeid(PtrList).name()) + " " +
							string(typeid(*this).name()));
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
  string type;
  for (int i = 0; i < num_non_null; ++i) {
    is >> ptrid;
	 if (ptrid < 0 || ptrid >= size) {
		throw IoValueError(typeid(*this));
	 }
	 is >> type;
	 T* temp = dynamic_cast<T *>(IoMgr::newIO(type.c_str()));
	 if (!temp) throw InputError(typeid(T));
	 insertAt(ptrid, temp);
	 temp->deserialize(is, flag & ~IO::TYPENAME);
  }
  // itsFirstVacant
  is >> itsFirstVacant;
  if (itsFirstVacant < 0) {
	 throw IoValueError(typeid(*this));
  }

  // The next character after itsFirstVacant had better be a newline,
  // and we need to remove it from the stream.
  if ( is.get() != '\n' ) throw IoLogicError(typeid(*this));

  if (is.fail()) throw InputError(typeid(*this));
}

template <class T>
int PtrList::charCount() const {
  static string ioTag = typeid(PtrList).name();	 
  int ch_count = ioTag.size() + 1
	 + gCharCount<int>(itsVec.size()) + 1;
  int num_non_null = PtrList::count();
  ch_count += 
	 gCharCount<int>(num_non_null) + 1;
  
  for (int i = 0; i < itsVec.size(); ++i) {
	 if (itsVec[i] != NULL) {
		ch_count += gCharCount<int>(i) + 1;
		ch_count += itsVec[i]->charCount() + 1;
	 }
  }

  ch_count += gCharCount<int>(itsFirstVacant) + 1;
  return ch_count + 5;
}

///////////////
// accessors //
///////////////

template <class T>
int PtrList::capacity() const {
DOTRACE("PtrList::capacity");
  return itsVec.size();
}

template <class T>
int PtrList::count() const {
DOTRACE("PtrList::count"); 
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

template <class T>
bool PtrList::isValidId(int id) const {
DOTRACE("PtrList::isValidId");

  DebugEval(id);
  DebugEval(id>=0);
  DebugEvalNL(itsVec.size());
  DebugEvalNL(itsVec[id]);
  DebugEvalNL(id<itsVec.size());
  DebugEvalNL(itsVec[id] != NULL);

  return ( id >= 0 && id < itsVec.size() && itsVec[id] != NULL ); 
}

template <class T>
T* PtrList::getCheckedPtr(int id) const throw (InvalidIdError) {
DOTRACE("PtrList::getCheckedPtr");
  if ( !isValidId(id) ) { throw InvalidIdError(); }
  return getPtr(id);
}

//////////////////
// manipulators //
//////////////////

template <class T>
int PtrList::insert(T* ptr) {
DOTRACE("PtrList::insert");
  int new_site = itsFirstVacant;
  insertAt(new_site, ptr);
  return new_site;              // return the id of the inserted T*
}

template <class T>
void PtrList::insertAt(int id, T* ptr) {
DOTRACE("PtrList::insertAt");
  if (id < 0) return;

  if (id >= itsVec.size()) {
    itsVec.resize(id+RESIZE_CHUNK, NULL);
  }

  delete itsVec[id];
  itsVec[id] = ptr;

  // It is possible that ptr is NULL, in this case, we might need to
  // adjust itsFirstVacant if it is currently beyond than the site
  // that we have just changed.
  if (ptr == NULL && id < itsFirstVacant)
	 itsFirstVacant = id;

  // make sure itsFirstVacant is up-to-date
  while ( (itsVec[itsFirstVacant] != NULL) &&
          (++itsFirstVacant < itsVec.size()) );
}

// this member function delete's the T pointed to by itsVec[i]
template <class T>
void PtrList::remove(int id) {
DOTRACE("PtrList::remove");
  if (!isValidId(id)) return;

  delete itsVec[id];
  itsVec[id] = NULL;         // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

template <class T>
void PtrList::clear() {
DOTRACE("PtrList::clear");
  for (int i = 0; i < itsVec.size(); ++i) {
	 DebugEval(i); DebugEvalNL(itsVec.size());
	 delete itsVec[i];
	 itsVec[i] = NULL;
  }

  itsFirstVacant = 0;
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
