///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Fri Mar 24 18:24:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include <iostream.h>

#include "iomgr.h"
#include "readutils.h"
#include "util/arrays.h"
#include "util/strings.h"
#include "writeutils.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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
  fixed_string ioTag = IO::ioTypename();

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { 
	 dynamic_string typename_list = ioTag;
	 typename_list += " ";
	 typename_list += alternateIoTags();
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
  fixed_string type;
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

  int count = ReadUtils::readSequenceCount(reader, "itsVec"); DebugEvalNL(count);

  Assert(count >= 0);
  unsigned int uint_count = (unsigned int) count;

  fixed_block<IO*> ioBlock(uint_count);

  ReadUtils::readObjectSeq(reader, "itsVec", ioBlock.begin(),
									(IO*) 0, count);

  voidVecResize(uint_count);

  for (size_t i = 0; i < uint_count; ++i)
	 if (ioBlock[i] != 0) {
		insertVoidPtrAt(i, fromIOToVoid(ioBlock[i]));
	 }
}


void IoPtrList::writeTo(Writer* writer) const {
DOTRACE("IoPtrList::writeTo");
  writer->writeInt("itsFirstVacant", firstVacant());

  unsigned int count = voidVecSize();
  DebugEvalNL(count);

  fixed_block<IO*> ioBlock(count);

  for (size_t i = 0; i < count; ++i)
	 {
		DebugEval(i);
		void* voidptr = getVoidPtr(i);       DebugEval(voidptr);
		if ( voidptr != 0 )
		  {
			 ioBlock[i] = fromVoidToIO(voidptr);
		  }
		DebugEvalNL(ioBlock[i]);
	 }

  WriteUtils::writeObjectSeq(writer, "itsVec", ioBlock.begin(), ioBlock.end());
}

const char* IoPtrList::alternateIoTags() const {
DOTRACE("IoPtrList::alternateIoTags");
  static const char* result = ""; 
  return result;
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
