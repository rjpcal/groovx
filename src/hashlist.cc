///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Mon Jul  5 15:30:06 1999
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
  const int BUF_SIZE = 200;
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
PtrList::PtrList(int) :
  itsHashTable(new Tcl_HashTable),
  itsFirstVacant(0)
{
DOTRACE("PtrList::PtrList");
  Tcl_InitHashTable(itsHashTable, TCL_ONE_WORD_KEYS);
}

template <class T>
PtrList::~PtrList() {
DOTRACE("PtrList::~PtrList");
  clear();
  Tcl_DeleteHashTable(itsHashTable);
  delete itsHashTable;
}

template <class T>
void PtrList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("PtrList::serialize");
  static string ioTag = typeid(PtrList).name();	 

  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(PtrList).name() << sep; }

  // Serialize all non-null ptr's.
  os << count() << sep;

  for (const_iterator itr = begin(); itr != end(); ++itr) {
	 os << itr.toInt() << sep;
	 (*itr)->serialize(os, flag|TYPENAME);
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
  
  int n;
  is >> n; // count of how many items we'll read into the hash table

  int ptrid;
  string type;
  for (int i = 0; i < n; ++i) {
	 is >> ptrid;
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
  if (is.fail()) throw InputError(typeid(*this));
}

template <class T>
int PtrList::charCount() const {
  static string ioTag = typeid(PtrList).name();	 
  int ch_count = ioTag.size() + 1
	 + gCharCount<int>(count()) + 1;
  
  for (const_iterator itr = begin(); itr != end(); ++itr) {
	 ch_count += gCharCount<int>(itr.toInt()) + 1;
	 ch_count += (*itr)->charCount() + 1;
  }

  ch_count += gCharCount<int>(itsFirstVacant) + 1;
  return ch_count + 5;
}

///////////////
// accessors //
///////////////

template <class T>
int PtrList::count() const {
DOTRACE("PtrList::count");
  int c = 0;
  for (const_iterator itr = begin(); itr != end(); ++itr)
	 { ++c; }
  return c;
}

template <class T>
T* PtrList::getPtr(int id) const {
DOTRACE("PtrList::getPtr");
  Tcl_HashEntry* entry = Tcl_FindHashEntry(itsHashTable,
														 reinterpret_cast<char*>(id));
  return static_cast<T*>(Tcl_GetHashValue(entry));
}

//////////////////
// manipulators //
//////////////////

template <class T>
void PtrList::insertAt(int id, T* ptr) {
DOTRACE("PtrList::insertAt");
  int isNew;
  // Either returns the current entry, or a new entry if there
  // wasn't one previously
  Tcl_HashEntry* entry = Tcl_CreateHashEntry(itsHashTable,
															reinterpret_cast<char*>(id),
															&isNew);
  
  if (!isNew) {
	 T* oldPtr = static_cast<T*>(Tcl_GetHashValue(entry));
	 delete oldPtr;
  }
  
  Tcl_SetHashValue(entry, static_cast<ClientData>(ptr));
  
  if (ptr == 0) {
		Tcl_DeleteHashEntry(entry);
  }
  
  if (id < itsFirstVacant && ptr == 0) {
	 itsFirstVacant = id;
  }
  
  else if (id == itsFirstVacant && ptr != 0) {
	 // increment itsFirstVacant until we find a non-valid id
	 while ( isValidId(++itsFirstVacant) ) { /* nothing */ }
  }
}

template <class T>
void PtrList::remove(int id) {
DOTRACE("PtrList::remove");
  if (!isValidId(id)) return;
  
  delete getPtr(id);
  insertAt(id, 0);
}

template <class T>
void PtrList::clear() {
DOTRACE("PtrList::clear");
  for (iterator itr = begin(); itr != end(); ++itr) {
	 delete *itr;
  }
  Tcl_DeleteHashTable(itsHashTable);
  Tcl_InitHashTable(itsHashTable, TCL_ONE_WORD_KEYS);
  itsFirstVacant = 0;
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
