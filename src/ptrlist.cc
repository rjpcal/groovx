///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Tue Oct 19 18:25:21 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#include <algorithm> // for ::count
#include <iostream.h>
#include <string>
#include <typeinfo>
#include <strstream.h>

#include "demangle.h"
#include "iomgr.h"
#include "reader.h"
#include "writer.h"

#ifndef NULL
#define NULL 0L
#endif

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
// PtrList member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

template <class T>
PtrList<T>::PtrList(int size):
  itsFirstVacant(0), 
  itsVec()
{
DOTRACE("PtrList<T>::PtrList");

  itsVec.reserve(size); 

  DebugEvalNL(itsVec.size());
}

template <class T>
PtrList<T>::~PtrList() {
DOTRACE("PtrList<T>::~PtrList");
  clear();
}

template <class T>
void PtrList<T>::serialize(ostream &os, IOFlag flag) const {
DOTRACE("PtrList<T>::serialize");
  static string ioTag = demangle(typeid(PtrList).name());

  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // itsVec: we will serialize only the non-null T*'s in
  // itsVec. In order to correctly deserialize the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << itsVec.size() << sep;

  int num_non_null = PtrList<T>::count();
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
void PtrList<T>::deserialize(istream &is, IOFlag flag) {
DOTRACE("PtrList<T>::deserialize");
  static string ioTag = demangle(typeid(PtrList).name());

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { 
	 IO::readTypename(is, demangle(typeid(PtrList).name()) + " " +
							demangle(typeid(*this).name()));
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
int PtrList<T>::charCount() const {
  static string ioTag = demangle(typeid(PtrList).name());
  int ch_count = ioTag.size() + 1
	 + gCharCount<int>(itsVec.size()) + 1;
  int num_non_null = PtrList<T>::count();
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

template <class T>
void PtrList<T>::writeTo(Writer* writer) const {
DOTRACE("PtrList<T>::writeTo");
  writer->writeInt("itsFirstVacant", itsFirstVacant);
  writer->writeObjectSeq("itsVec", itsVec.begin(), itsVec.end());
}

template <class T>
void PtrList<T>::readFrom(Reader* reader) {
DOTRACE("PtrList<T>::readFrom");
  itsFirstVacant = reader->readInt("itsFirstVacant");
  itsVec.clear();
  reader->readObjectSeq("itsVec", back_inserter(itsVec), (T*) 0);
}

///////////////
// accessors //
///////////////

template <class T>
int PtrList<T>::capacity() const {
DOTRACE("PtrList<T>::capacity");
  return itsVec.size();
}

template <class T>
int PtrList<T>::count() const {
DOTRACE("PtrList<T>::count"); 
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
bool PtrList<T>::isValidId(int id) const {
DOTRACE("PtrList<T>::isValidId");

  DebugEval(id);
  DebugEval(id>=0);
  DebugEvalNL(itsVec.size());
  DebugEvalNL(itsVec[id]);
  DebugEvalNL(id<itsVec.size());
  DebugEvalNL(itsVec[id] != NULL);

  return ( id >= 0 && id < itsVec.size() && itsVec[id] != NULL ); 
}

template <class T>
T* PtrList<T>::getCheckedPtr(int id) const throw (InvalidIdError) {
DOTRACE("PtrList<T>::getCheckedPtr");
  if ( !isValidId(id) ) { throw InvalidIdError(); }
  return getPtr(id);
}

//////////////////
// manipulators //
//////////////////

template <class T>
int PtrList<T>::insert(T* ptr) {
DOTRACE("PtrList<T>::insert");
  int new_site = itsFirstVacant;
  insertAt(new_site, ptr);
  return new_site;              // return the id of the inserted T*
}

template <class T>
void PtrList<T>::insertAt(int id, T* ptr) {
DOTRACE("PtrList<T>::insertAt");
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
void PtrList<T>::remove(int id) {
DOTRACE("PtrList<T>::remove");
  if (!isValidId(id)) return;

  delete itsVec[id];
  itsVec[id] = NULL;         // reset the pointer to NULL

  // reset itsFirstVacant in case i would now be the first vacant
  if (itsFirstVacant > id) itsFirstVacant = id;
}

template <class T>
void PtrList<T>::clear() {
DOTRACE("PtrList<T>::clear");
  for (int i = 0; i < itsVec.size(); ++i) {
	 DebugEval(i); DebugEvalNL(itsVec.size());
	 delete itsVec[i];
	 itsVec[i] = NULL;
  }

  itsVec.resize(0);

  itsFirstVacant = 0;
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
