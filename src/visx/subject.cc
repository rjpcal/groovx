///////////////////////////////////////////////////////////////////////
//
// subject.cc
// Rob Peters
// created: Dec-98
// written: Tue Sep 26 19:07:33 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include <iostream.h>

#include "io/reader.h"
#include "io/writer.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string_literal ioTag("Subject");
}

///////////////////////////////////////////////////////////////////////
//
// Subject member funcitons
//
///////////////////////////////////////////////////////////////////////

Subject::Subject(const char* name, const char* dir) : 
  itsName(name ? name : ""), itsDirectory(dir ? dir : "")
{
DOTRACE("Subject::Subject");
}

Subject::~Subject() {
DOTRACE("Subject::~Subject");
}
#ifdef LEGACY
Subject::Subject(STD_IO::istream &is, IO::IOFlag flag) :
  itsName(""), itsDirectory("")
{
DOTRACE("Subject::Subject");
  legacyDesrlz(is, flag);
}
#endif
void Subject::legacySrlz(IO::Writer* writer, STD_IO::ostream &os, IO::IOFlag flag) const {
DOTRACE("Subject::legacySrlz");
  if (flag & IO::BASES) { /* there are no bases to legacyDesrlz */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  os << itsName << endl;
  os << itsDirectory << endl;

  if (os.fail()) throw IO::OutputError(ioTag.c_str());
}

void Subject::legacyDesrlz(IO::Reader* reader, STD_IO::istream &is, IO::IOFlag flag) {
DOTRACE("Subject::legacyDesrlz");
  if (flag & IO::BASES) { /* there are no bases to legacyDesrlz */ }
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }

  getline(is, itsName, '\n');
  getline(is, itsDirectory, '\n');

  if (is.fail()) throw IO::InputError(ioTag.c_str());
}

int Subject::legacyCharCount() const {
  return (ioTag.length() + 1
			 + itsName.length() + 1
			 + itsDirectory.length() + 1
			 + 5);// fudge factor
}

void Subject::readFrom(IO::Reader* reader) {
DOTRACE("Subject::readFrom");

  reader->readValue("name", itsName);
  reader->readValue("directory", itsDirectory);
}

void Subject::writeTo(IO::Writer* writer) const {
DOTRACE("Subject::writeTo");

  writer->writeValue("name", itsName);
  writer->writeValue("directory", itsDirectory);
}

void Subject::setName(const char* name) {
  itsName = name;
}

void Subject::setDirectory(const char* dir) {
  itsDirectory = dir;
}

static const char vcid_subject_cc[] = "$Header$";
#endif // !SUBJECT_CC_DEFINED
