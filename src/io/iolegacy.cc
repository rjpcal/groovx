///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 08:40:04 2000
// written: Fri Oct 20 10:35:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOLEGACY_CC_DEFINED
#define IOLEGACY_CC_DEFINED

#include "io/iolegacy.h"

#include "io/iomgr.h"

#include "util/strings.h"
#include "util/value.h"

#include <cctype>
#include <cstring>
#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// LegacyReader::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyReader::Impl {
public:
  Impl(IO::LegacyReader* owner, STD_IO::istream& is, IO::IOFlag flag) :
	 itsOwner(owner),
	 itsInStream(is),
	 itsFlags(flag),
	 itsLegacyVersionId(0)
  {}

  void throwIfError(const char* type) {
	 if (itsInStream.fail()) {
		DebugPrint("throwIfError for"); DebugEvalNL(type);
		throw IO::InputError(type);
	 }
  }

  IO::LegacyReader* itsOwner;
  STD_IO::istream& itsInStream;
  IO::IOFlag itsFlags;
  int itsLegacyVersionId;

  void readTypename(const fixed_string& correct_name)
  {
	 fixed_string name; 
	 itsInStream >> name;

	 if (! name.equals(correct_name)) {
		// If we got here, then none of the substrings matched so we must
		// raise an exception.
		IO::InputError err("couldn't read typename for ");
		err.appendMsg(correct_name.c_str());
		throw err;
	 }
  }

  int getLegacyVersionId()
  {
	 while ( isspace(itsInStream.peek()) )
		{ itsInStream.get(); }

	 int version = -1;

	 if (itsInStream.peek() == '@') {
		int c = itsInStream.get();
		Assert(c == '@');

		itsInStream >> version;
		DebugEvalNL(version);
		throwIfError("versionId");
	 }
	 else {
		throw IO::InputError("missing legacy versionId");
	 }

	 return version;
  }

  void grabLeftBrace()
  {
	 char brace;
	 itsInStream >> brace;
	 if (brace != '{') {
		DebugPrintNL("grabLeftBrace failed");
		throw IO::InputError("missing left-brace");
	 }
  }

  void grabRightBrace()
  {
	 char brace;
	 itsInStream >> brace;
	 if (brace != '}') {
		DebugPrintNL("grabRightBrace failed");
		throw IO::InputError("missing right-brace");
	 }
  }

  void inflateObject(const char* name, IO::IoObject* obj)
  {
	 DebugEvalNL(name);
	 Precondition(obj != 0);

	 itsLegacyVersionId = getLegacyVersionId();
	 if (itsLegacyVersionId != -1) 
		{
		  grabLeftBrace();
		  obj->readFrom(itsOwner);
		  grabRightBrace();
		}

	 throwIfError(name);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyReader member definitions
//
///////////////////////////////////////////////////////////////////////

IO::LegacyReader::LegacyReader(STD_IO::istream& is, IO::IOFlag flag) :
  itsImpl(new Impl(this, is, flag))
{
DOTRACE("IO::LegacyReader::LegacyReader");
}

IO::LegacyReader::~LegacyReader() {
DOTRACE("IO::LegacyReader::~LegacyReader");
  delete itsImpl; 
}

IO::VersionId IO::LegacyReader::readSerialVersionId() {
DOTRACE("IO::LegacyReader::readSerialVersionId");
  DebugEvalNL(itsImpl->itsLegacyVersionId);
  return itsImpl->itsLegacyVersionId;
}

char IO::LegacyReader::readChar(const char* name) {
DOTRACE("IO::LegacyReader::readChar");
  DebugEval(name);
  char val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

int IO::LegacyReader::readInt(const char* name) {
DOTRACE("IO::LegacyReader::readInt");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

bool IO::LegacyReader::readBool(const char* name) {
DOTRACE("IO::LegacyReader::readBool");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return bool(val);
}

double IO::LegacyReader::readDouble(const char* name) {
DOTRACE("IO::LegacyReader::readDouble");
  DebugEval(name);
  double val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

char* IO::LegacyReader::readCstring(const char* name) {
DOTRACE("IO::LegacyReader::readCstring");
  DebugEvalNL(name);

  int numchars = 0;
  itsImpl->itsInStream >> numchars;

  itsImpl->throwIfError(name);

  char* return_buf = 0;

  if (itsImpl->itsInStream.peek() == '\n') { itsImpl->itsInStream.get(); }
  if ( numchars > 0 )
	 {
		return_buf = new char[numchars+1];
		itsImpl->itsInStream.read(&return_buf[0], numchars);
		return_buf[numchars] = '\0';
	 }
  else
	 {
		return_buf = new char[1];
		return_buf[0] = '\0';
	 }

  try {
	 itsImpl->throwIfError(name);
  }
  catch (...) {
	 delete [] return_buf;
	 throw;
  }

  Postcondition(return_buf != 0);
  return return_buf;
}

void IO::LegacyReader::readValueObj(const char* name, Value& value) {
DOTRACE("IO::LegacyReader::readValueObj");
  DebugEvalNL(name);
  value.scanFrom(itsImpl->itsInStream);
  itsImpl->throwIfError(name);
}

IO::IoObject* IO::LegacyReader::readObject(const char* name) {
DOTRACE("IO::LegacyReader::readObject");
  DebugEval(name);
  dynamic_string type;
  itsImpl->itsInStream >> type; DebugEval(type);

  if (type == "NULL")
	 {
		return 0;
	 }

  IO::IoObject* obj = IO::IoMgr::newIO(type.c_str());
  DebugEvalNL(obj->ioTypename());

  itsImpl->inflateObject(name, obj);

  return obj;
}

void IO::LegacyReader::readOwnedObject(const char* name,
													IO::IoObject* obj) {
DOTRACE("IO::LegacyReader::readOwnedObject");
  Precondition(obj != 0);

  itsImpl->readTypename(obj->ioTypename());
  itsImpl->inflateObject(name, obj);
}

void IO::LegacyReader::readBaseClass(const char* baseClassName,
												 IO::IoObject* basePart) {
DOTRACE("IO::LegacyReader::readBaseClass");
  Precondition(basePart != 0);

  itsImpl->readTypename(basePart->ioTypename());
  itsImpl->inflateObject(baseClassName, basePart);
}

IO::IoObject* IO::LegacyReader::readRoot(IO::IoObject* root) {
DOTRACE("IO::LegacyReader::readRoot");
  if (root == 0) {
	 return readObject("rootObject");
  }
  DebugEvalNL(root->ioTypename());

  itsImpl->readTypename(root->ioTypename());
  itsImpl->inflateObject("rootObject", root);

  return root;
}

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter::Impl definition
//
///////////////////////////////////////////////////////////////////////

class IO::LegacyWriter::Impl {
public:
  Impl(IO::LegacyWriter* owner, STD_IO::ostream& os, IO::IOFlag flag) :
	 itsOwner(owner),
	 itsOutStream(os),
	 itsFlags(flag),
	 itsFSep(' ')
  {}

  void throwIfError(const char* type) {
	 if (itsOutStream.fail()) {
		DebugPrint("throwIfError for"); DebugEvalNL(type);
		throw IO::OutputError(type);
	 }
  }

  IO::LegacyWriter* itsOwner;
  STD_IO::ostream& itsOutStream;
  IO::IOFlag itsFlags;
  const char itsFSep;				  // field separator

  void flattenObject(const char* obj_name, const IO::IoObject* obj)
  {
	 if (obj == 0)
		{
		  itsOutStream << "NULL" << itsFSep;
		  throwIfError(obj_name);
		  return;
		}

	 Assert(obj != 0);

	 itsOutStream << obj->ioTypename() << itsFSep;
	 throwIfError(obj->ioTypename().c_str());

	 itsOutStream << '@' << obj->serialVersionId() << " { ";

	 obj->writeTo(itsOwner);

	 itsOutStream << " } ";

	 throwIfError(obj_name);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, IO::IOFlag flag) :
  itsImpl(new Impl(this, os, flag))
{
DOTRACE("IO::LegacyWriter::LegacyWriter");
}

IO::LegacyWriter::~LegacyWriter() {
DOTRACE("IO::LegacyWriter::~LegacyWriter");
  delete itsImpl;
}

void IO::LegacyWriter::writeChar(const char* name, char val) {
DOTRACE("IO::LegacyWriter::writeChar");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeInt(const char* name, int val) {
DOTRACE("IO::LegacyWriter::writeInt");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeBool(const char* name, bool val) {
DOTRACE("IO::LegacyWriter::writeBool");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeDouble(const char* name, double val) {
DOTRACE("IO::LegacyWriter::writeDouble");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeCstring(const char* name, const char* val) {
DOTRACE("IO::LegacyWriter::writeCstring");

  itsImpl->itsOutStream << strlen(val) << '\n'
								<< val << '\n';

  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeValueObj(const char* name,
												 const Value& value) {
DOTRACE("IO::LegacyWriter::writeValueObj");
  value.printTo(itsImpl->itsOutStream);
  itsImpl->itsOutStream << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeObject(const char* name, const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeObject");

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
													 const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeOwnedObject");

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
												  const IO::IoObject* basePart) {
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (itsImpl->itsFlags & IO::BASES) {
	 itsImpl->flattenObject(baseClassName, basePart);
  }
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root) {
DOTRACE("IO::LegacyWriter::writeRoot");

  itsImpl->flattenObject("rootObject", root);
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
