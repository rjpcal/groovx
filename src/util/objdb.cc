///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Sun Nov 21 03:10:48 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#include "demangle.h"
#include "iomgr.h"
#include "reader.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

IoPtrList::IoPtrList(int size) :
  VoidPtrList(size)
{
DOTRACE("IoPtrList::IoPtrList");
}

void IoPtrList::serialize(ostream &os, IOFlag flag) const {
DOTRACE("IoPtrList::serialize");
  string ioTag = IO::ioTypename();

  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  const vector<void*>& theVec = VoidPtrList::vec();

  // itsVec: we will serialize only the non-null T*'s in
  // itsVec. In order to correctly deserialize the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << theVec.size() << sep;

  int num_non_null = VoidPtrList::count();
  os << num_non_null << endl;

  // Serialize all non-null ptr's.
  int c = 0;
  for (int i = 0; i < theVec.size(); ++i) {
    if (theVec[i] != NULL) {
      os << i << sep;
      // we must serialize the typename since deserialize requires a
      // typename in order to call the virtual constructor
		IO* obj = fromVoidToIO(theVec[i]);
      obj->serialize(os, flag|TYPENAME);
      ++c;
    }
  }

  if (c != num_non_null) {
	 throw IoLogicError(ioTag);
  }

  // itsFirstVacant
  os << firstVacant() << endl;
  if (os.fail()) throw OutputError(ioTag);
}


void IoPtrList::deserialize(istream &is, IOFlag flag) {
DOTRACE("IoPtrList::deserialize");
 string ioTag = IO::ioTypename();

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { 
	 IO::readTypename(is, ioTag);
  }

  vector<void*>& theVec = VoidPtrList::vec();

  // theVec
  clear();
  int size, num_non_null;
  is >> size >> num_non_null;
  // We must check if the istream has failed in order to avoid
  // attempting to resize theVec to some crazy size.
  if (is.fail()) throw InputError(ioTag);
  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IoValueError(ioTag);
  }
  theVec.resize(size, NULL);
  int ptrid;
  string type;
  for (int i = 0; i < num_non_null; ++i) {
    is >> ptrid;
	 if (ptrid < 0 || ptrid >= size) {
		throw IoValueError(ioTag);
	 }

	 is >> type;

	 IO* obj = IoMgr::newIO(type.c_str());
	 if (!obj) throw InputError(ioTag);

	 insertVoidPtrAt(ptrid, fromIOToVoid(obj));

	 obj->deserialize(is, flag & ~IO::TYPENAME);
	 afterInsertHook(ptrid, fromIOToVoid(obj));
  }
  // itsFirstVacant
  is >> VoidPtrList::firstVacant();
  if (firstVacant() < 0) {
	 throw IoValueError(ioTag);
  }

  // The next character after itsFirstVacant had better be a newline,
  // and we need to remove it from the stream.
  if ( is.get() != '\n' ) throw IoLogicError(ioTag);

  if (is.fail()) throw InputError(ioTag);
}


int IoPtrList::charCount() const {
DOTRACE("IoPtrList::charCount");
  string ioTag = IO::ioTypename();
  int ch_count = ioTag.size() + 1
	 + gCharCount<int>(vec().size()) + 1;
  int num_non_null = VoidPtrList::count();
  ch_count += 
	 gCharCount<int>(num_non_null) + 1;
  
  for (int i = 0; i < vec().size(); ++i) {
	 if (vec()[i] != NULL) {
		ch_count += gCharCount<int>(i) + 1;

		IO* obj = fromVoidToIO(vec()[i]);
		ch_count += obj->charCount() + 1;
	 }
  }

  ch_count += gCharCount<int>(firstVacant()) + 1;
  return ch_count + 5;
}


void IoPtrList::readFrom(Reader* reader) {
DOTRACE("IoPtrList::readFrom");
  firstVacant() = reader->readInt("itsFirstVacant");

  vector<IO*> ioVec;

  reader->readObjectSeq("itsVec", back_inserter(ioVec), (IO*) 0);

  vector<void*>& voidVec = VoidPtrList::vec();

  voidVec.clear();
  voidVec.resize(ioVec.size());

  for (int i = 0; i < ioVec.size(); ++i)
	 if (ioVec[i] != 0) {
		voidVec[i] = fromIOToVoid(ioVec[i]);
		afterInsertHook(i, voidVec[i]);
	 }
}


void IoPtrList::writeTo(Writer* writer) const {
DOTRACE("IoPtrList::writeTo");
  writer->writeInt("itsFirstVacant", firstVacant());

  vector<IO*> ioVec(vec().size());

  for (int i = 0; i < vec().size(); ++i)
	 if ( vec()[i] != 0 )
		ioVec[i] = fromVoidToIO(vec()[i]);

  writer->writeObjectSeq("itsVec", ioVec.begin(), ioVec.end());
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
