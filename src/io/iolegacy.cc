///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 08:40:04 2000
// written: Fri Sep 29 16:20:21 2000
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

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace IO {
  /** This function reads the next word from \a theStream, and
      compares it to the correct names in \a correctNames. \a
      correctNames should consist of one or more space-separated
      words. If the word read from the stream does not match any of
      these words, and \a doCheck is true, an \c InputError is thrown
      with the last word in correctNames as its info. If \a doCheck is
      false, the function does nothing except read a word from \a
      theStream. */
  void readTypename(STD_IO::istream& is,
						  const char* correctNames_cstr,
						  bool doCheck);
}

void IO::readTypename(STD_IO::istream& is,
							 const char* correctNames_cstr,
							 bool doCheck) {
DOTRACE("IO::readTypename");
  std::string correctNames = correctNames_cstr;

  std::string name;
  is >> name;

  DebugEval(name); DebugPrintNL("");
  DebugEvalNL(is.peek());

  // If we aren't checking, then we can skip the rest of the routine.
  if (!doCheck) return;
  
  DebugEval(correctNames);
  DebugEvalNL(doCheck);

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

class IO::LegacyReader::Impl {
public:
  Impl(STD_IO::istream& is, IO::IOFlag flag) :
	 itsInStream(is),
	 itsFlags(flag)
  {}

  STD_IO::istream& itsInStream;
  IO::IOFlag itsFlags;
  LegacyStringMode itsStringMode;
};

///////////////////////////////////////////////////////////////////////
//
// LegacyReader member definitions
//
///////////////////////////////////////////////////////////////////////

IO::LegacyReader::LegacyReader(STD_IO::istream& is, IO::IOFlag flag) :
  itsImpl(new Impl(is, flag))
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
  itsImpl->itsFlags = new_flags;
}

void IO::LegacyReader::throwIfError(const char* type) {
DOTRACE("IO::LegacyReader::throwIfError");
  if (itsImpl->itsInStream.fail()) {
	 DebugEvalNL(type);
	 throw IO::InputError(type);
  }
}

int IO::LegacyReader::eatWhitespace() {
DOTRACE("IO::LegacyReader::eatWhitespace");
  int c=0;
  while ( isspace(itsImpl->itsInStream.peek()) )
	 { itsImpl->itsInStream.get(); ++c; }
  return c;
}

int IO::LegacyReader::peek() {
DOTRACE("IO::LegacyReader::peek");
  return itsImpl->itsInStream.peek();
}

int IO::LegacyReader::getChar() {
DOTRACE("IO::LegacyReader::getChar");
  return itsImpl->itsInStream.get();
}

void IO::LegacyReader::setStringMode(LegacyStringMode mode) {
DOTRACE("IO::LegacyReader::setStringMode");
  itsImpl->itsStringMode = mode;
}

IO::VersionId IO::LegacyReader::readSerialVersionId() {
DOTRACE("IO::LegacyReader::readSerialVersionId");
  throw IO::InputError("LegacyReader--not implemented");
  return -1;
}

char IO::LegacyReader::readChar(const char* name) {
DOTRACE("IO::LegacyReader::readChar");
  DebugEval(name);
  char val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  throwIfError(name);
  return val;
}

int IO::LegacyReader::readInt(const char* name) {
DOTRACE("IO::LegacyReader::readInt");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  throwIfError(name);
  return val;
}

bool IO::LegacyReader::readBool(const char* name) {
DOTRACE("IO::LegacyReader::readBool");
  DebugEval(name);
  int val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  throwIfError(name);
  return bool(val);
}

double IO::LegacyReader::readDouble(const char* name) {
DOTRACE("IO::LegacyReader::readDouble");
  DebugEval(name);
  double val;
  itsImpl->itsInStream >> val;   DebugEvalNL(val);
  throwIfError(name);
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
		int numchars;
		itsImpl->itsInStream >> numchars;
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
	 throw IO::InputError("LegacyWriter was set to unknown string mode");
	 break;
  }

  try {
	 throwIfError(name);
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
  throwIfError(name);
}

IO::IoObject* IO::LegacyReader::readObject(const char* name) {
DOTRACE("IO::LegacyReader::readObject");
  DebugEval(name);
  dynamic_string type;
  itsImpl->itsInStream >> type; DebugEval(type);

  IO::IoObject* obj = IO::IoMgr::newIO(type.c_str());
  DebugEvalNL(obj->ioTypename());

  IO::LRFlagJanitor jtr_(*this, itsImpl->itsFlags & ~IO::TYPENAME);
  obj->readFrom(this);

  throwIfError(name);
  return obj;
}

void IO::LegacyReader::readOwnedObject(const char* name,
													IO::IoObject* obj) {
DOTRACE("IO::LegacyReader::readOwnedObject");
  DebugEvalNL(name);
  if (itsImpl->itsFlags & IO::TYPENAME) {
	 IO::readTypename(itsImpl->itsInStream,
							obj->legacyValidTypenames().c_str(), true);
  }
  obj->readFrom(this);
  throwIfError(name);
}

void IO::LegacyReader::readBaseClass(const char* baseClassName,
												 IO::IoObject* basePart) {
DOTRACE("IO::LegacyReader::readBaseClass");
  if (itsImpl->itsFlags & IO::BASES) {
	 DebugEvalNL(baseClassName);
	 IO::LRFlagJanitor jtr_(*this, itsImpl->itsFlags | IO::TYPENAME);
	 
	 // This test should always succeed since we just set flags to
	 // include IO::TYPENAME with the FlagJanitor
	 if (itsImpl->itsFlags & IO::TYPENAME) {
		IO::readTypename(itsImpl->itsInStream,
							  basePart->legacyValidTypenames().c_str(), true);
	 }

	 basePart->readFrom(this);
  }
  throwIfError(baseClassName);
}

IO::IoObject* IO::LegacyReader::readRoot(IO::IoObject* root) {
DOTRACE("IO::LegacyReader::readRoot");
  if (root == 0) {
	 return readObject("rootObject");
  }
  DebugEvalNL(root->ioTypename());
  if (itsImpl->itsFlags & IO::TYPENAME) {
	 IO::readTypename(itsImpl->itsInStream,
							root->legacyValidTypenames().c_str(), true);
  }
  root->readFrom(this);
  return root;
}


class IO::LegacyWriter::Impl {
public:
  Impl(STD_IO::ostream& os, IO::IOFlag flag) :
	 itsOutStream(os),
	 itsFlags(flag),
	 itsFSep()
  {
	 itsFSep[0] = IO::SEP;
	 itsFSep[1] = '\0';
  }

  STD_IO::ostream& itsOutStream;
  IO::IOFlag itsFlags;
  char itsFSep[8];				  // field separator
  LegacyStringMode itsStringMode;
};

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, IO::IOFlag flag) :
  itsImpl(new Impl(os, flag))
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

void IO::LegacyWriter::throwIfError(const char* type) {
DOTRACE("IO::LegacyWriter::throwIfError");
  if (itsImpl->itsOutStream.fail()) throw IO::OutputError(type);
}

void IO::LegacyWriter::setFieldSeparator(char sep) {
DOTRACE("IO::LegacyWriter::setFieldSeparator");
  itsImpl->itsFSep[0] = sep;
  itsImpl->itsFSep[1] = '\0';
}

void IO::LegacyWriter::setFieldSeparator(const char* sep) {
DOTRACE("IO::LegacyWriter::setFieldSeparator");
  char* ptr = &itsImpl->itsFSep[0];
  char* end = ptr+8;
  while (*ptr++ = *sep++)
	 if (ptr >= end) break;
  ptr[7] = '\0';
}

void IO::LegacyWriter::resetFieldSeparator() {
DOTRACE("IO::LegacyWriter::resetFieldSeparator");
  itsImpl->itsFSep[0] = IO::SEP;
  itsImpl->itsFSep[1] = '\0';
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
  throwIfError(name);
}

void IO::LegacyWriter::writeInt(const char* name, int val) {
DOTRACE("IO::LegacyWriter::writeInt");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  throwIfError(name);
}

void IO::LegacyWriter::writeBool(const char* name, bool val) {
DOTRACE("IO::LegacyWriter::writeBool");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  throwIfError(name);
}

void IO::LegacyWriter::writeDouble(const char* name, double val) {
DOTRACE("IO::LegacyWriter::writeDouble");
  itsImpl->itsOutStream << val << itsImpl->itsFSep;
  throwIfError(name);
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

  throwIfError(name);
}

void IO::LegacyWriter::writeValueObj(const char* name,
												 const Value& value) {
DOTRACE("IO::LegacyWriter::writeValueObj");
  value.printTo(itsImpl->itsOutStream);
  itsImpl->itsOutStream << itsImpl->itsFSep;
  throwIfError(name);
}

void IO::LegacyWriter::writeObject(const char* name, const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeObject");
  resetFieldSeparator();

  DebugEvalNL(itsImpl->itsFlags & IO::TYPENAME); 

  if (itsImpl->itsFlags & IO::TYPENAME) {
	 itsImpl->itsOutStream << obj->ioTypename() << itsImpl->itsFSep;
	 throwIfError(obj->ioTypename().c_str());
  }

  obj->writeTo(this); 
  resetFieldSeparator();
  throwIfError(name);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
													 const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeOwnedObject");
  resetFieldSeparator();

  DebugEvalNL(itsImpl->itsFlags & IO::TYPENAME); 

  if (itsImpl->itsFlags & IO::TYPENAME) {
	 itsImpl->itsOutStream << obj->ioTypename() << itsImpl->itsFSep;
	 throwIfError(obj->ioTypename().c_str());
  }

  obj->writeTo(this); 
  resetFieldSeparator();
  throwIfError(name);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
												  const IO::IoObject* basePart) {
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (itsImpl->itsFlags & IO::BASES) {
	 resetFieldSeparator();
	 IO::LWFlagJanitor jtr_(*this, itsImpl->itsFlags | IO::TYPENAME);


	 DebugEvalNL(itsImpl->itsFlags & IO::TYPENAME); 

	 if (itsImpl->itsFlags & IO::TYPENAME) {
		itsImpl->itsOutStream << basePart->ioTypename() << itsImpl->itsFSep;
		throwIfError(basePart->ioTypename().c_str());
	 }

	 basePart->writeTo(this);
	 resetFieldSeparator();
  }
  throwIfError(baseClassName);
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root) {
DOTRACE("IO::LegacyWriter::writeRoot");
  resetFieldSeparator();


  DebugEvalNL(itsImpl->itsFlags & IO::TYPENAME); 

  if (itsImpl->itsFlags & IO::TYPENAME) {
	 itsImpl->itsOutStream << root->ioTypename() << itsImpl->itsFSep;
	 throwIfError(root->ioTypename().c_str());
  }

  root->writeTo(this);
  resetFieldSeparator();
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
