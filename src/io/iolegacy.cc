///////////////////////////////////////////////////////////////////////
//
// iolegacy.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 27 08:40:04 2000
// written: Wed Sep 27 17:21:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOLEGACY_CC_DEFINED
#define IOLEGACY_CC_DEFINED

#include "io/iolegacy.h"

#include <cctype>

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// LegacyReader member definitions
//
///////////////////////////////////////////////////////////////////////

IO::LegacyReader::LegacyReader(STD_IO::istream& is, IO::IOFlag flag) :
  itsInStream(is),
  itsFlags(flag)
{
DOTRACE("IO::LegacyReader::LegacyReader");
}

IO::LegacyReader::~LegacyReader() {
DOTRACE("IO::LegacyReader::~LegacyReader");
}

void IO::LegacyReader::throwIfError(const char* type) {
DOTRACE("IO::LegacyReader::throwIfError");
  if (itsInStream.fail()) throw IO::InputError(type);
}

void IO::LegacyReader::readTypename(const char* correctNames,
												bool doCheck) {
DOTRACE("IO::LegacyReader::readTypename");
  DebugEvalNL(itsFlags & IO::TYPENAME); 
  if (itsFlags & IO::TYPENAME) {
	 IO::IoObject::readTypename(itsInStream, correctNames, doCheck);
  }
}

int IO::LegacyReader::eatWhitespace() {
DOTRACE("IO::LegacyReader::eatWhitespace");
  int c=0;
  while ( isspace(itsInStream.peek()) )
	 { itsInStream.get(); ++c; }
  return c;
}

IO::VersionId IO::LegacyReader::readSerialVersionId() {
DOTRACE("IO::LegacyReader::readSerialVersionId");
  throw IO::InputError("LegacyReader--not implemented");
}

char IO::LegacyReader::readChar(const char* name) {
DOTRACE("IO::LegacyReader::readChar");
  char temp;
  itsInStream >> temp;
  throwIfError(name);
  return temp;
}

int IO::LegacyReader::readInt(const char* name) {
DOTRACE("IO::LegacyReader::readInt");
  int temp;
  itsInStream >> temp;
  throwIfError(name);
  return temp;
}

bool IO::LegacyReader::readBool(const char* name) {
DOTRACE("IO::LegacyReader::readBool");
  int temp;
  itsInStream >> temp;
  throwIfError(name);
  return bool(temp);
}

double IO::LegacyReader::readDouble(const char* name) {
DOTRACE("IO::LegacyReader::readDouble");
  double temp;
  itsInStream >> temp;
  throwIfError(name);
  return temp;
}

char* IO::LegacyReader::readCstring(const char* name) {
DOTRACE("IO::LegacyReader::readCstring");
  throw IO::InputError("LegacyReader--not implemented");
  throwIfError(name);
}

void IO::LegacyReader::readValueObj(const char* name, Value& value) {
DOTRACE("IO::LegacyReader::readValueObj");
  throw IO::InputError("LegacyReader--not implemented");
  throwIfError(name);
}

IO::IoObject* IO::LegacyReader::readObject(const char* name) {
DOTRACE("IO::LegacyReader::readObject");
  throw IO::InputError("LegacyReader--not implemented");
  throwIfError(name);
}

void IO::LegacyReader::readOwnedObject(const char* name,
													IO::IoObject* obj) {
DOTRACE("IO::LegacyReader::readOwnedObject");
  obj->legacyDesrlz(this); 
  throwIfError(name);
}

void IO::LegacyReader::readBaseClass(const char* baseClassName,
												 IO::IoObject* basePart) {
DOTRACE("IO::LegacyReader::readBaseClass");
  if (itsFlags & IO::BASES) {
	 IO::LRFlagJanitor jtr_(*this, itsFlags | IO::TYPENAME);
	 basePart->legacyDesrlz(this);
  }
  throwIfError(baseClassName);
}

IO::IoObject* IO::LegacyReader::readRoot(IO::IoObject* root) {
DOTRACE("IO::LegacyReader::readRoot");
  if (root == 0) {
	 throw IO::InputError("LegacyReader can't handle a null root");
  }
  root->legacyDesrlz(this);
  return root;
}

///////////////////////////////////////////////////////////////////////
//
// LegacyWriter member definitions
//
///////////////////////////////////////////////////////////////////////


IO::LegacyWriter::LegacyWriter(STD_IO::ostream& os, IO::IOFlag flag) :
  itsOutStream(os),
  itsFlags(flag),
  itsFieldSeparator(IO::SEP)
{
DOTRACE("IO::LegacyWriter::LegacyWriter");
}

IO::LegacyWriter::~LegacyWriter() {
DOTRACE("IO::LegacyWriter::~LegacyWriter");
}

void IO::LegacyWriter::throwIfError(const char* type) {
DOTRACE("IO::LegacyWriter::throwIfError");
  if (itsOutStream.fail()) throw IO::OutputError(type);
}

void IO::LegacyWriter::writeTypename(const char* type) {
DOTRACE("IO::LegacyWriter::writeTypename");

  DebugEvalNL(itsFlags & IO::TYPENAME); 

  if (itsFlags & IO::TYPENAME) {
	 itsOutStream << type << IO::SEP;
	 throwIfError(type);
  }
}

void IO::LegacyWriter::setFieldSeparator(char sep) {
DOTRACE("IO::LegacyWriter::setFieldSeparator");
  itsFieldSeparator = sep;
}

void IO::LegacyWriter::resetFieldSeparator() {
DOTRACE("IO::LegacyWriter::resetFieldSeparator");
  itsFieldSeparator = IO::SEP;
}

void IO::LegacyWriter::writeChar(const char* name, char val) {
DOTRACE("IO::LegacyWriter::writeChar");
  itsOutStream << val << itsFieldSeparator;
  throwIfError(name);
}

void IO::LegacyWriter::writeInt(const char* name, int val) {
DOTRACE("IO::LegacyWriter::writeInt");
  itsOutStream << val << itsFieldSeparator;
  throwIfError(name);
}

void IO::LegacyWriter::writeBool(const char* name, bool val) {
DOTRACE("IO::LegacyWriter::writeBool");
  itsOutStream << val << itsFieldSeparator;
  throwIfError(name);
}

void IO::LegacyWriter::writeDouble(const char* name, double val) {
DOTRACE("IO::LegacyWriter::writeDouble");
  itsOutStream << val << itsFieldSeparator;
  throwIfError(name);
}

void IO::LegacyWriter::writeCstring(const char* name, const char* val) {
DOTRACE("IO::LegacyWriter::writeCstring");
  throw IO::OutputError("LegacyWriter--not implemented");
  throwIfError(name);
}

void IO::LegacyWriter::writeValueObj(const char* name,
												 const Value& value) {
DOTRACE("IO::LegacyWriter::writeValueObj");
  throw IO::OutputError("LegacyWriter--not implemented");
  throwIfError(name);
}

void IO::LegacyWriter::writeObject(const char* name, const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeObject");
  throw IO::OutputError("LegacyWriter--not implemented");
  resetFieldSeparator();
  throwIfError(name);
}

void IO::LegacyWriter::writeOwnedObject(const char* name,
													 const IO::IoObject* obj) {
DOTRACE("IO::LegacyWriter::writeOwnedObject");
  resetFieldSeparator();
  obj->legacySrlz(this); 
  resetFieldSeparator();
  throwIfError(name);
}

void IO::LegacyWriter::writeBaseClass(const char* baseClassName,
												  const IO::IoObject* basePart) {
DOTRACE("IO::LegacyWriter::writeBaseClass");
  if (itsFlags & IO::BASES) {
	 resetFieldSeparator();
	 IO::LWFlagJanitor jtr_(*this, itsFlags | IO::TYPENAME);
	 basePart->legacySrlz(this);
	 resetFieldSeparator();
  }
  throwIfError(baseClassName);
}

void IO::LegacyWriter::writeRoot(const IO::IoObject* root) {
DOTRACE("IO::LegacyWriter::writeRoot");
  resetFieldSeparator();
  root->legacySrlz(this);
  resetFieldSeparator();
}

static const char vcid_iolegacy_cc[] = "$Header$";
#endif // !IOLEGACY_CC_DEFINED
