///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Wed Mar  8 11:38:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include <iostream.h>
#include <typeinfo>
#include <string>
#include <vector>

#include "demangle.h"
#include "iomgr.h"
#include "readutils.h"
#include "util/strings.h"
#include "writeutils.h"

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
  fixed_string ioTag = IO::ioTypename();

  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // itsVec: we will serialize only the non-null T*'s in
  // itsVec. In order to correctly deserialize the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // serialize (so that deserialize knows when to stop reading).
  os << voidVecSize() << sep;

  int num_non_null = VoidPtrList::count();
  os << num_non_null << endl;

  // Serialize all non-null ptr's.
  int c = 0;
  for (size_t i = 0, end = voidVecSize();
		 i < end;
		 ++i) {
    if (getVoidPtr(i) != NULL) {
      os << i << sep;
      // we must serialize the typename since deserialize requires a
      // typename in order to call the virtual constructor
		IO* obj = fromVoidToIO(getVoidPtr(i));
      obj->serialize(os, flag|TYPENAME);
      ++c;
    }
  }

  if (c != num_non_null) {
	 throw IoLogicError(ioTag.c_str());
  }

  // itsFirstVacant
  os << firstVacant() << endl;
  if (os.fail()) throw OutputError(ioTag.c_str());
}


void IoPtrList::deserialize(istream &is, IOFlag flag) {
DOTRACE("IoPtrList::deserialize");
  string ioTag = IO::ioTypename().c_str();

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { 
	 string typename_list = ioTag + " " + alternateIoTags();
	 IO::readTypename(is, typename_list.c_str());
  }

  // voidVec
  clear();
  int size, num_non_null;
  is >> size >> num_non_null;
  // We must check if the istream has failed in order to avoid
  // attempting to resize the voidVec to some crazy size.
  if (is.fail()) throw InputError(ioTag.c_str());
  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IoValueError(ioTag.c_str());
  }
  voidVecResize(size);
  int ptrid;
  string type;
  for (int i = 0; i < num_non_null; ++i) {
    is >> ptrid;
	 if (ptrid < 0 || ptrid >= size) {
		throw IoValueError(ioTag.c_str());
	 }

	 is >> type;

	 IO* obj = IoMgr::newIO(type.c_str());
	 if (!obj) throw InputError(ioTag.c_str());

	 insertVoidPtrAt(ptrid, fromIOToVoid(obj));

	 obj->deserialize(is, flag & ~IO::TYPENAME);
	 afterInsertHook(ptrid, fromIOToVoid(obj));
  }
  // itsFirstVacant
  is >> VoidPtrList::firstVacant();
  if (firstVacant() < 0) {
	 throw IoValueError(ioTag.c_str());
  }

  // The next character after itsFirstVacant had better be a newline,
  // and we need to remove it from the stream.
  if ( is.get() != '\n' )
	 { throw IoLogicError(ioTag.c_str()); }

  if (is.fail()) throw InputError(ioTag.c_str());
}


int IoPtrList::charCount() const {
DOTRACE("IoPtrList::charCount");
  fixed_string ioTag = IO::ioTypename();
  int ch_count = ioTag.length() + 1
	 + gCharCount<int>(voidVecSize()) + 1;
  int num_non_null = VoidPtrList::count();
  ch_count += 
	 gCharCount<int>(num_non_null) + 1;
  
  for (size_t i = 0, end = voidVecSize();
		 i < end;
		 ++i) {
	 if (getVoidPtr(i) != NULL) {
		ch_count += gCharCount<int>(i) + 1;

		IO* obj = fromVoidToIO(getVoidPtr(i));
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

  ReadUtils::readObjectSeq(reader, "itsVec", back_inserter(ioVec), (IO*) 0);

  voidVecResize(ioVec.size());

  for (size_t i = 0, end = ioVec.size();
		 i < end;
		 ++i)
	 if (ioVec[i] != 0) {
		insertVoidPtrAt(i, fromIOToVoid(ioVec[i]));
	 }
}


void IoPtrList::writeTo(Writer* writer) const {
DOTRACE("IoPtrList::writeTo");
  writer->writeInt("itsFirstVacant", firstVacant());

  vector<IO*> ioVec(voidVecSize());

  for (size_t i = 0, end = voidVecSize();
		 i < end;
		 ++i)
	 if ( getVoidPtr(i) != 0 )
		ioVec[i] = fromVoidToIO(getVoidPtr(i));

  WriteUtils::writeObjectSeq(writer, "itsVec", ioVec.begin(), ioVec.end());
}

const char* IoPtrList::alternateIoTags() const {
DOTRACE("IoPtrList::alternateIoTags");
  static const char* result = ""; 
  return result;
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
