///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Fri Sep 29 18:42:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include "io/iolegacy.h"
#include "io/iomgr.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/arrays.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

IoPtrList::IoPtrList(int size) :
  VoidPtrList(size)
{
DOTRACE("IoPtrList::IoPtrList");
}

IoPtrList::~IoPtrList() {}

void IoPtrList::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("IoPtrList::legacySrlz");

  fixed_string ioTag = IO::IoObject::ioTypename();

  // itsVec: we will legacySrlz only the non-null T*'s in
  // itsVec. In order to correctly legacyDesrlz the object later, we
  // must write both the size of itsVec (in order to correctly
  // resize later), as well as the number of non-null objects that we
  // legacySrlz (so that legacyDesrlz knows when to stop reading).

  lwriter->writeValue("voidVecSize", voidVecSize());

  int num_non_null = VoidPtrList::count();
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("num_non_null", num_non_null);
  lwriter->resetFieldSeparator();

  // Serialize all non-null ptr's.
  int c = 0;
  for (size_t i = 0, end = voidVecSize();
		 i < end;
		 ++i) {
	 if (getVoidPtr(i) != NULL) {
		lwriter->writeValue("i", i);

		// we must legacySrlz the typename since legacyDesrlz requires a
		// typename in order to call the virtual constructor
		IO::IoObject* obj = fromVoidToIO(getVoidPtr(i));
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		lwriter->writeObject("ptrListItem", obj);

		++c;
	 }
  }

  if (c != num_non_null) {
	 throw IO::LogicError(ioTag.c_str());
  }

  lwriter->setFieldSeparator('\n');
  lwriter->writeInt("itsFirstVacant", firstVacant());
}

fixed_string IoPtrList::legacyValidTypenames() const {
DOTRACE("IoPtrList::legacyValidTypenames");
  fixed_string ioTag = IO::IoObject::ioTypename();

  dynamic_string typename_list = ioTag;
  typename_list += " ";
  typename_list += alternateIoTags();
  return typename_list.c_str();
}

void IoPtrList::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("IoPtrList::legacyDesrlz");

  fixed_string ioTag = IO::IoObject::ioTypename();

  int size, num_non_null;
  lreader->readValue("voidVecSize", size);
  lreader->readValue("num_non_null", num_non_null);

  if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
	 throw IO::ValueError(ioTag.c_str());
  }

  VoidPtrList::clear();
  voidVecResize(size);

  int ptrid;

  for (int i = 0; i < num_non_null; ++i) {
	 lreader->readValue("i", ptrid);

	 if (ptrid < 0 || ptrid >= size) {
		throw IO::ValueError(ioTag.c_str());
	 }

	 IO::IoObject* obj = lreader->readObject("ptrListItem");

	 insertVoidPtrAt(ptrid, fromIOToVoid(obj));
  }

  firstVacant() = lreader->readInt("itsFirstVacant");

  // The next character after itsFirstVacant had better be a newline,
  // and we need to remove it from the stream.
  lreader->grabNewline();
}

void IoPtrList::readFrom(IO::Reader* reader) {
DOTRACE("IoPtrList::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  firstVacant() = reader->readInt("itsFirstVacant");

  int count = IO::ReadUtils::readSequenceCount(reader, "itsVec"); DebugEvalNL(count);

  Assert(count >= 0);
  unsigned int uint_count = (unsigned int) count;

  fixed_block<IO::IoObject*> ioBlock(uint_count);

  IO::ReadUtils::template readObjectSeq<IO::IoObject>(
                   reader, "itsVec", ioBlock.begin(), count);

  VoidPtrList::clear();
  voidVecResize(uint_count);

  for (size_t i = 0; i < uint_count; ++i)
	 if (ioBlock[i] != 0) {
		insertVoidPtrAt(i, fromIOToVoid(ioBlock[i]));
	 }
}


void IoPtrList::writeTo(IO::Writer* writer) const {
DOTRACE("IoPtrList::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  writer->writeInt("itsFirstVacant", firstVacant());

  unsigned int count = voidVecSize();
  DebugEvalNL(count);

  fixed_block<IO::IoObject*> ioBlock(count);

  for (size_t i = 0; i < count; ++i)
	 {
		DebugEval(i);
		void* voidptr = getVoidPtr(i);       DebugEval(voidptr);

		ioBlock[i] = fromVoidToIO(voidptr);

		DebugEvalNL(ioBlock[i]);
	 }

  IO::WriteUtils::writeObjectSeq(writer, "itsVec", ioBlock.begin(), ioBlock.end());
}

const char* IoPtrList::alternateIoTags() const {
DOTRACE("IoPtrList::alternateIoTags");
  static const char* result = ""; 
  return result;
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
