///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 08:40:04 2000
// written: Thu Oct 19 16:03:13 2000
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
#include <string>
#include <iostream.h>

#define LOCAL_TRACE
#include "util/trace.h"
#define LOCAL_DEBUG
#define LOCAL_ASSERT
#include "util/debug.h"

namespace IO {
  // This function reads the next word from theStream, and compares it
  // to the correct names in correctNames. correctNames should consist
  // of one or more space-separated words. If the word read from the
  // stream does not match any of these words, and doCheck is true, an
  // InputError is thrown with the last word in correctNames as its
  // info. If doCheck is false, the function does nothing except read
  // a word from theStream.
  void readTypename(STD_IO::istream& is, const char* correctNames_cstr);

  const char DEFAULT_SEP = ' ';
}

void IO::readTypename(STD_IO::istream& is, const char* correctNames_cstr) {
DOTRACE("IO::readTypename");
  std::string correctNames = correctNames_cstr;

  std::string name;
  is >> name;

  DebugEval(name); DebugPrintNL("");
  DebugEvalNL(is.peek());

  DebugEval(correctNames);

  std::string candidate;

  std::string first_candidate = correctNames.substr(0, correctNames.find(' ', 0));

  std::string::size_type end_pos = 0;
  std::string::size_type beg_pos = 0;

  // Loop over the space-separated substrings of correctNames.
  while ( end_pos < std::string::npos ) {
	 end_pos = correctNames.find(' ', beg_pos);

	 candidate = correctNames.substr(beg_pos, end_pos);

	 DebugEval(name);
	 DebugEval(candidate);
	 DebugEval(beg_pos);
	 DebugEvalNL(end_pos);
	 
	 if (name == candidate) {
		DebugEval(is.peek());
		DebugPrintNL("found a name match");

		// Due to a quirk where Tlist was sometimes written as "Tlist
		// PtrList<Trial>" or "Tlist Tlist", we need to check if the
		// next typename is a duplicate
		if (name == "Tlist") {
#ifdef PRESTANDARD_IOSTREAMS
		  typedef streampos pos_type;
#else
		  typedef std::istream::pos_type pos_type;
#endif
		  pos_type pos = is.tellg();   DebugEvalNL(pos);
		  std::string duplicate;
		  is >> duplicate;   DebugEvalNL(duplicate);

		  // If the string that we read was not actually a duplicate
		  // typename, then reset the stream position so that someone
		  // else can read that info from the stream
		  if (duplicate != "PtrList<Trial>" &&
				duplicate != "Tlist") {
			 is.seekg(pos);
			 DebugEvalNL(is.tellg());
		  }
		}

		return;
	 }

	 // Skip ahead until we find the first non-space character. This
	 // character will mark the beginning of the next candidate
	 // typename.
	 beg_pos = correctNames.find_first_not_of(' ', end_pos+1);
  }

  // If we got here, then none of the substrings matched so we must
  // raise an exception.
  IO::InputError err("couldn't read typename for ");
  err.appendMsg(first_candidate.c_str());
  throw err;
}

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
	 itsStringMode(IO::CHAR_COUNT),
	 itsLegacyVersionId(0)
  {}

  void throwIfError(const char* type) {
	 if (itsInStream.fail()) {
		DOTRACE("IO::LegacyReader::Impl::throwIfError+");
		DebugEvalNL(type);
		throw IO::InputError(type);
	 }
  }

#if 0
  struct ReadState {
  	 IO::IOFlag flags;
	 LegacyStringMode strMode;
  	 const fixed_string objType;
  	 bool usingLegacyVersion;
	 const int attribCount;
	 int curAttribCount;
  };
#endif

  IO::LegacyReader* itsOwner;
  STD_IO::istream& itsInStream;
  IO::IOFlag itsFlags;
  LegacyStringMode itsStringMode;
  int itsLegacyVersionId;

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

IO::IOFlag IO::LegacyReader::flags() const {
DOTRACE("IO::LegacyReader::flags");
  return itsImpl->itsFlags;
}

void IO::LegacyReader::setFlags(IO::IOFlag new_flags) {
DOTRACE("IO::LegacyReader::setFlags");
  DebugEval(itsImpl->itsFlags); DebugEval(new_flags);
  itsImpl->itsFlags = new_flags;
  DebugEvalNL(itsImpl->itsFlags);
}

int IO::LegacyReader::getLegacyVersionId() {
DOTRACE("IO::LegacyReader::getLegacyVersionId");
  return itsImpl->getLegacyVersionId();
}

void IO::LegacyReader::grabLeftBrace() {
DOTRACE("IO::LegacyReader::grabLeftBrace");
  itsImpl->grabLeftBrace();
}

void IO::LegacyReader::grabRightBrace() {
DOTRACE("IO::LegacyReader::grabRightBrace");
  itsImpl->grabRightBrace();
}

void IO::LegacyReader::grabNewline() {
DOTRACE("IO::LegacyReader::grabNewline");
  if (itsImpl->itsInStream.get() != '\n') {
	 throw IO::LogicError("missing newline");
  }
}

void IO::LegacyReader::setStringMode(LegacyStringMode mode) {
DOTRACE("IO::LegacyReader::setStringMode");
  itsImpl->itsStringMode = mode;
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

  char* return_buf = 0;

  switch (itsImpl->itsStringMode) {
  case IO::GETLINE_NEWLINE:
	 {
#ifdef MIPSPRO_COMPILER
		if ( itsImpl->itsInStream.peek() == ' ' )
		  itsImpl->itsInStream.get();
#endif
		fixed_string temp;
		getline(itsImpl->itsInStream, temp, '\n');
		return_buf = new char[temp.length()+1];
		strcpy(return_buf, temp.c_str());
	 }
	 break;
  case IO::GETLINE_TAB:
	 {
#ifdef MIPSPRO_COMPILER
		if ( itsImpl->itsInStream.peek() == ' ' || itsImpl->itsInStream.peek() == '\n' )
		  itsImpl->itsInStream.get();
#endif
		fixed_string temp;
		getline(itsImpl->itsInStream, temp, '\t');
		return_buf = new char[temp.length()+1];
		strcpy(return_buf, temp.c_str());
	 }
	 break;
  case IO::CHAR_COUNT:
	 {
		int numchars = 0;
		itsImpl->itsInStream >> numchars;

		// If the read failed on the number, we assume the string is not
		// present, and let numchars remain at 0
		if ( itsImpl->itsInStream.fail() )
		  {
			 itsImpl->itsInStream.clear();
			 numchars = 0;
		  }
		  
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
	 }
	 break;
  default:
	 throw IO::InputError("LegacyReader was set to unknown string mode");
	 break;
  }

  try {
	 itsImpl->throwIfError(name);
  }
  catch (...) {
	 delete [] return_buf;
	 throw;
  }

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

  IO::IoObject* obj = IO::IoMgr::newIO(type.c_str());
  DebugEvalNL(obj->ioTypename());

  IO::LRFlagJanitor jtr_(*this, itsImpl->itsFlags & ~IO::TYPENAME);

  itsImpl->inflateObject(name, obj);

  return obj;
}

void IO::LegacyReader::readOwnedObject(const char* name,
													IO::IoObject* obj) {
DOTRACE("IO::LegacyReader::readOwnedObject");
  DebugEvalNL(name);
  if (itsImpl->itsFlags & IO::TYPENAME) {
	 IO::readTypename(itsImpl->itsInStream,
							obj->legacyValidTypenames().c_str());
  }

  itsImpl->inflateObject(name, obj);
}

void IO::LegacyReader::readBaseClass(const char* baseClassName,
												 IO::IoObject* basePart) {
DOTRACE("IO::LegacyReader::readBaseClass");
  DebugEval(IO::BASES); DebugEvalNL(itsImpl->itsFlags);

  DebugEvalNL(baseClassName);
  IO::LRFlagJanitor jtr_(*this, itsImpl->itsFlags | IO::TYPENAME);

  // This test should always succeed since we just set flags to
  // include IO::TYPENAME with the FlagJanitor
  if (itsImpl->itsFlags & IO::TYPENAME) {
	 IO::readTypename(itsImpl->itsInStream,
							basePart->legacyValidTypenames().c_str());
  }

  itsImpl->inflateObject(baseClassName, basePart);
}

IO::IoObject* IO::LegacyReader::readRoot(IO::IoObject* root) {
DOTRACE("IO::LegacyReader::readRoot");
  if (root == 0) {
	 return readObject("rootObject");
  }
  DebugEvalNL(root->ioTypename());
  if (itsImpl->itsFlags & IO::TYPENAME) {
	 IO::readTypename(itsImpl->itsInStream,
							root->legacyValidTypenames().c_str());
  }

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
	 itsFSep(DEFAULT_SEP),
	 itsStringMode(IO::CHAR_COUNT)
  {}

  void throwIfError(const char* type) {
	 if (itsOutStream.fail()) {
		DOTRACE("IO::LegacyWriter::Impl::throwIfError+");
		DebugEvalNL(type);
		throw IO::OutputError(type);
	 }
  }

  IO::LegacyWriter* itsOwner;
  STD_IO::ostream& itsOutStream;
  IO::IOFlag itsFlags;
  char itsFSep;				  // field separator
  LegacyStringMode itsStringMode;

  void resetFieldSeparator()
  { itsFSep = DEFAULT_SEP; }

  void flattenObject(const char* obj_name, const IO::IoObject* obj)
  {

	 DebugEvalNL(itsFlags & IO::TYPENAME); 

	 if (itsFlags & IO::TYPENAME) {
		itsOutStream << obj->ioTypename() << itsFSep;
		throwIfError(obj->ioTypename().c_str());
	 }

	 itsOutStream << '@' << obj->serialVersionId() << " { ";

	 obj->writeTo(itsOwner);
	 resetFieldSeparator();

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

IO::IOFlag IO::LegacyWriter::flags() const {
DOTRACE("IO::LegacyWriter::flags");
  return itsImpl->itsFlags;
}

void IO::LegacyWriter::setFlags(IO::IOFlag new_flags) {
DOTRACE("IO::LegacyWriter::setFlags");
  itsImpl->itsFlags = new_flags;
}

void IO::LegacyWriter::setFieldSeparator(char sep) {
DOTRACE("IO::LegacyWriter::setFieldSeparator");
  itsImpl->itsFSep = sep;
}

void IO::LegacyWriter::resetFieldSeparator() {
DOTRACE("IO::LegacyWriter::resetFieldSeparator");
  itsImpl->resetFieldSeparator();
}

void IO::LegacyWriter::insertChar(char c) {
DOTRACE("IO::LegacyWriter::insertChar");
  itsImpl->itsOutStream << c;
}

void IO::LegacyWriter::setStringMode(LegacyStringMode mode) {
DOTRACE("IO::LegacyWriter::setStringMode");
  itsImpl->itsStringMode = mode;
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

  switch (itsImpl->itsStringMode) {
  case IO::GETLINE_NEWLINE:
	 itsImpl->itsOutStream << val << '\n';
	 break;
  case IO::GETLINE_TAB:
	 itsImpl->itsOutStream << val << '\t';
	 break;
  case IO::CHAR_COUNT:
	 itsImpl->itsOutStream << strlen(val) << '\n'
								  << val << '\n';
	 break;
  default:
	 throw IO::InputError("LegacyWriter was set to unknown string mode");
	 break;
  }

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
  resetFieldSeparator();

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
													 const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeOwnedObject");
  resetFieldSeparator();

  itsImpl->flattenObject(name, obj);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
												  const IO::IoObject* basePart) {
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (itsImpl->itsFlags & IO::BASES) {
	 resetFieldSeparator();
	 IO::LWFlagJanitor jtr_(*this, itsImpl->itsFlags | IO::TYPENAME);

	 itsImpl->flattenObject(baseClassName, basePart);
  }
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root) {
DOTRACE("IO::LegacyWriter::writeRoot");

  resetFieldSeparator();

  itsImpl->flattenObject("rootObject", root);
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
