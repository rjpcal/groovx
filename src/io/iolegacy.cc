///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 27 08:40:04 2000
// written: Wed Nov 15 11:46:44 2000
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
  Impl(IO::LegacyReader* owner, STD_IO::istream& is) :
	 itsOwner(owner),
	 itsInStream(is),
	 itsLegacyVersionId(0)
  {}

  void throwIfError(const char* type) {
	 if (itsInStream.fail()) {
		DebugPrint("throwIfError for"); DebugEvalNL(type);
		throw IO::InputError(type);
	 }
  }

  void throwIfError(const fixed_string& type) {
	 throwIfError(type.c_str());
  }

  IO::LegacyReader* itsOwner;
  STD_IO::istream& itsInStream;
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

  void inflateObject(const fixed_string& name, IO::IoObject* obj)
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

IO::LegacyReader::LegacyReader(STD_IO::istream& is) :
  itsImpl(new Impl(this, is))
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

char IO::LegacyReader::readChar(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readChar");
  DebugEval(name);
  char val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

int IO::LegacyReader::readInt(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readInt");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

bool IO::LegacyReader::readBool(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readBool");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return bool(val);
}

double IO::LegacyReader::readDouble(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readDouble");
  DebugEval(name);
  double val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  itsImpl->throwIfError(name);
  return val;
}

fixed_string IO::LegacyReader::readStringImpl(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readStringImpl");
  DebugEvalNL(name);

  int numchars = 0;
  itsImpl->itsInStream >> numchars;

  itsImpl->throwIfError(name);

  if (numchars < 0)
	 {
		throw IO::LogicError("LegacyReader::readStringImpl "
									"saw negative character count");
	 }

  int c = itsImpl->itsInStream.get();
  if (c != ' ')
	 {
		throw IO::LogicError("LegacyReader::readStringImpl "
									"did not have whitespace after character count");
	 }

//   if (itsImpl->itsInStream.peek() == '\n') { itsImpl->itsInStream.get(); }

  fixed_string new_string(numchars);

  if ( numchars > 0 )
	 {
		itsImpl->itsInStream.read(new_string.data(), numchars);
		itsImpl->throwIfError(name);
	 }
  new_string.data()[numchars] = '\0';

  Postcondition(new_string.length() == numchars);
  return new_string;
}

void IO::LegacyReader::readValueObj(const fixed_string& name, Value& value) {
DOTRACE("IO::LegacyReader::readValueObj");
  DebugEvalNL(name);
  value.scanFrom(itsImpl->itsInStream);
  itsImpl->throwIfError(name);
}

IO::IoObject* IO::LegacyReader::readObject(const fixed_string& name) {
DOTRACE("IO::LegacyReader::readObject");
  DebugEval(name);
  fixed_string type;
  itsImpl->itsInStream >> type; DebugEval(type);

  if (type == "NULL")
	 {
		return 0;
	 }

  IO::IoObject* obj = IO::IoMgr::newIO(type);
  DebugEvalNL(obj->ioTypename());

  itsImpl->inflateObject(name, obj);

  return obj;
}

void IO::LegacyReader::readOwnedObject(const fixed_string& name,
													IO::IoObject* obj) {
DOTRACE("IO::LegacyReader::readOwnedObject");
  Precondition(obj != 0);

  itsImpl->readTypename(obj->ioTypename());
  itsImpl->inflateObject(name, obj);
}

void IO::LegacyReader::readBaseClass(const fixed_string& baseClassName,
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
  Impl(IO::LegacyWriter* owner, STD_IO::ostream& os, bool write_bases) :
	 itsOwner(owner),
	 itsOutStream(os),
	 itsWriteBases(write_bases),
	 itsFSep(' '),
	 itsIndentLevel(0),
	 itsNeedsNewline(false)
  {}

  void throwIfError(const char* type) {
	 if (itsOutStream.fail()) {
		DebugPrint("throwIfError for"); DebugEvalNL(type);
		throw IO::OutputError(type);
	 }
  }

  IO::LegacyWriter* itsOwner;
private:
  STD_IO::ostream& itsOutStream;
public:
  const bool itsWriteBases;
  const char itsFSep;				  // field separator
  int itsIndentLevel;
  bool itsNeedsNewline;

  STD_IO::ostream& stream()
	 {
		if (itsNeedsNewline)
		  {
			 doNewline();
			 itsNeedsNewline = false;
		  }
		return itsOutStream;
	 }

private:
  class Indenter {
  private:
	 Impl* itsOwner;
  public:
	 Indenter(Impl* impl) : itsOwner(impl) { ++(itsOwner->itsIndentLevel); }
	 ~Indenter() { --(itsOwner->itsIndentLevel); }
  };

  void doNewline()
	 {
		itsOutStream << '\n';
		for (int i = 0; i < itsIndentLevel; ++i)
		  itsOutStream << '\t';
	 }

  void requestNewline() { itsNeedsNewline = true; }

public:
  void flattenObject(const char* obj_name, const IO::IoObject* obj,
							bool stub_out = false)
  {
	 if (obj == 0)
		{
		  stream() << "NULL" << itsFSep;
		  throwIfError(obj_name);
		  return;
		}

	 Assert(obj != 0);

	 requestNewline();

	 stream() << obj->ioTypename() << itsFSep;
	 throwIfError(obj->ioTypename().c_str());

	 stream() << '@';

	 if (stub_out)
		{
		  stream() << "-1 ";
		}
	 else
		{
		  stream() << obj->serialVersionId() << " {";
		  {
			 Indenter indent(this);
			 requestNewline();
			 obj->writeTo(itsOwner);
		  }
		  requestNewline();
		  stream() << "}";
		}

	 requestNewline();

	 throwIfError(obj_name);
  }
};

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, bool write_bases) :
  itsImpl(new Impl(this, os, write_bases))
{
DOTRACE("IO::LegacyWriter::LegacyWriter");
}

IO::LegacyWriter::~LegacyWriter() {
DOTRACE("IO::LegacyWriter::~LegacyWriter");
  delete itsImpl;
}

void IO::LegacyWriter::writeChar(const char* name, char val) {
DOTRACE("IO::LegacyWriter::writeChar");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeInt(const char* name, int val) {
DOTRACE("IO::LegacyWriter::writeInt");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeBool(const char* name, bool val) {
DOTRACE("IO::LegacyWriter::writeBool");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeDouble(const char* name, double val) {
DOTRACE("IO::LegacyWriter::writeDouble");
  itsImpl->stream() << val << itsImpl->itsFSep;
  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeCstring(const char* name, const char* val) {
DOTRACE("IO::LegacyWriter::writeCstring");

//   itsImpl->stream() << strlen(val) << '\n'
  itsImpl->stream() << strlen(val) << " "
						  << val << itsImpl->itsFSep;

  itsImpl->throwIfError(name);
}

void IO::LegacyWriter::writeValueObj(const char* name,
												 const Value& value) {
DOTRACE("IO::LegacyWriter::writeValueObj");
  value.printTo(itsImpl->stream());
  itsImpl->stream() << itsImpl->itsFSep;
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
  if (itsImpl->itsWriteBases) {
	 itsImpl->flattenObject(baseClassName, basePart);
  }
  else {
	 itsImpl->flattenObject(baseClassName, basePart, true);
  }
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root) {
DOTRACE("IO::LegacyWriter::writeRoot");

  itsImpl->flattenObject("rootObject", root);
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
