///////////////////////////////////////////////////////////////////////
//
// ioptrlist.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Nov 21 00:26:29 1999
// written: Wed Oct 25 09:44:10 2000
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

namespace {
  const IO::VersionId IOPTRLIST_SERIAL_VERSION_ID = 1;
}

IoPtrList::IoPtrList(int size) :
  PtrListBase(size)
{
DOTRACE("IoPtrList::IoPtrList");
}

IoPtrList::~IoPtrList() {
DOTRACE("IoPtrList::~IoPtrList");
}

IO::VersionId IoPtrList::serialVersionId() const {
  return IOPTRLIST_SERIAL_VERSION_ID;
}

void IoPtrList::readFrom(IO::Reader* reader) {
DOTRACE("IoPtrList::readFrom");

  IO::VersionId svid = reader->readSerialVersionId(); 

  if (svid < 1)
	 {
		throw IO::ReadVersionError("IoPtrList", svid, 1,
											"Try grsh0.8a3");
	 }

  Assert(svid >= 1);

  int count = IO::ReadUtils::readSequenceCount(reader, "itsVec"); DebugEvalNL(count);

  Assert(count >= 0);
  unsigned int uint_count = (unsigned int) count;

  fixed_block<IO::IoObject*> ioBlock(uint_count);

  IO::ReadUtils::readObjectSeq<IO::IoObject>(
                   reader, "itsVec", ioBlock.begin(), count);

  clear();

  for (size_t i = 0; i < uint_count; ++i)
	 if (ioBlock[i] != 0) {
		insertPtrBase(ioBlock[i]);
	 }
}

void IoPtrList::writeTo(IO::Writer* writer) const {
DOTRACE("IoPtrList::writeTo");

  if (IOPTRLIST_SERIAL_VERSION_ID < 1)
	 {
		throw IO::WriteVersionError("IoPtrList",
											 IOPTRLIST_SERIAL_VERSION_ID, 1,
											 "Try grsh0.8a3");
	 }

  Assert(IOPTRLIST_SERIAL_VERSION_ID >= 1); 

  fixed_block<IO::IoObject*> ioBlock(capacity());

  for (size_t i = 0; i < capacity(); ++i)
	 {
		DebugEval(i);

		if (isValidId(i))
		  {
			 IO::IoObject* obj = getCheckedPtrBase(i);
			 Assert(obj != 0);
			 Assert(obj->isValid());
			 ioBlock[i] = obj;
		  }
		else
		  {
			 ioBlock[i] = 0;
		  }

		DebugEvalNL(ioBlock[i]);
	 }

  IO::WriteUtils::writeObjectSeq(writer, "itsVec", ioBlock.begin(), ioBlock.end());
}

static const char vcid_ioptrlist_cc[] = "$Header$";
#endif // !IOPTRLIST_CC_DEFINED
