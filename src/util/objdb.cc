///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Wed Sep 27 11:51:05 2000
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

void IoPtrList::legacySrlz(IO::Writer* writer) const {
DOTRACE("IoPtrList::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();

	 fixed_string ioTag = IO::IoObject::ioTypename();

	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 // itsVec: we will legacySrlz only the non-null T*'s in
	 // itsVec. In order to correctly legacyDesrlz the object later, we
	 // must write both the size of itsVec (in order to correctly
	 // resize later), as well as the number of non-null objects that we
	 // legacySrlz (so that legacyDesrlz knows when to stop reading).
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
		  // we must legacySrlz the typename since legacyDesrlz requires a
		  // typename in order to call the virtual constructor
		  IO::IoObject* obj = fromVoidToIO(getVoidPtr(i));
		  obj->ioSerialize(os, lwriter->flags() | IO::TYPENAME);
		  ++c;
		}
	 }

	 if (c != num_non_null) {
		throw IO::LogicError(ioTag.c_str());
	 }

	 // itsFirstVacant
	 os << firstVacant() << endl;
	 if (os.fail()) throw IO::OutputError(ioTag.c_str());
  }
}


void IoPtrList::legacyDesrlz(IO::Reader* reader) {
DOTRACE("IoPtrList::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();

	 fixed_string ioTag = IO::IoObject::ioTypename();

	 if (lreader->flags() & IO::TYPENAME) { 
		dynamic_string typename_list = ioTag;
		typename_list += " ";
		typename_list += alternateIoTags();
		IO::IoObject::readTypename(is, typename_list.c_str());
	 }

	 // voidVec
	 clear();
	 int size, num_non_null;
	 is >> size >> num_non_null;
	 // We must check if the STD_IO::istream has failed in order to avoid
	 // attempting to resize the voidVec to some crazy size.
	 if (is.fail()) throw IO::InputError(ioTag.c_str());
	 if ( size < 0 || num_non_null < 0 || num_non_null > size ) {
		throw IO::ValueError(ioTag.c_str());
	 }
	 VoidPtrList::clear();
	 voidVecResize(size);
	 int ptrid;
	 fixed_string type;
	 for (int i = 0; i < num_non_null; ++i) {
		is >> ptrid;
		if (ptrid < 0 || ptrid >= size) {
		  throw IO::ValueError(ioTag.c_str());
		}

		is >> type;

		IO::IoObject* obj = IO::IoMgr::newIO(type.c_str());
		if (!obj) throw IO::InputError(ioTag.c_str());

		insertVoidPtrAt(ptrid, fromIOToVoid(obj));
		obj->ioDeserialize(is, lreader->flags() & ~IO::TYPENAME);
	 }
	 // itsFirstVacant
	 is >> VoidPtrList::firstVacant();
	 if (firstVacant() < 0) {
		throw IO::ValueError(ioTag.c_str());
	 }

	 // The next character after itsFirstVacant had better be a newline,
	 // and we need to remove it from the stream.
	 if ( is.get() != '\n' )
		{ throw IO::LogicError(ioTag.c_str()); }

	 if (is.fail()) throw IO::InputError(ioTag.c_str());
  }
}

void IoPtrList::readFrom(IO::Reader* reader) {
DOTRACE("IoPtrList::readFrom");
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
