///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Sun Oct 22 15:58:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPTRLIST_CC_DEFINED
#define IOPTRLIST_CC_DEFINED

#include "ioptrlist.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/arrays.h"
#include "util/ptrhandle.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

IoPtrList::IoPtrList(int size) :
  PtrListBase(size)
{
DOTRACE("IoPtrList::IoPtrList");
}

IoPtrList::~IoPtrList() {
DOTRACE("IoPtrList::~IoPtrList");
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

  PtrListBase::clear();
  baseVecResize(uint_count);

  for (size_t i = 0; i < uint_count; ++i)
	 if (ioBlock[i] != 0) {
		insertPtrBaseAt(i, ioBlock[i]);
	 }
}

void IoPtrList::writeTo(IO::Writer* writer) const {
DOTRACE("IoPtrList::writeTo");

  writer->writeInt("itsFirstVacant", firstVacant());

  unsigned int count = baseVecSize();
  DebugEvalNL(count);

  fixed_block<IO::IoObject*> ioBlock(count);

  for (size_t i = 0; i < count; ++i)
	 {
		DebugEval(i);

		IO::IoObject* obj = dynamic_cast<IO::IoObject*>(getPtrBase(i));
		ioBlock[i] = obj;

		DebugEvalNL(ioBlock[i]);
	 }

  IO::WriteUtils::writeObjectSeq(writer, "itsVec", ioBlock.begin(), ioBlock.end());
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
