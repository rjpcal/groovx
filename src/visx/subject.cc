///////////////////////////////////////////////////////////////////////
//
// subject.cc
// Rob Peters
// created: Dec-98
// written: Wed Mar 15 10:17:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include <iostream.h>

#include "ioutils.h"
#include "reader.h"
#include "writer.h"

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

Subject::Subject(istream &is, IOFlag flag) :
  itsName(""), itsDirectory("")
{
DOTRACE("Subject::Subject");
  deserialize(is, flag);
}

void Subject::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Subject::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsName << endl;
  os << itsDirectory << endl;

  if (os.fail()) throw OutputError(ioTag.c_str());
}

void Subject::deserialize(istream &is, IOFlag flag) {
DOTRACE("Subject::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag.c_str()); }

  getline(is, itsName, '\n');
  getline(is, itsDirectory, '\n');

  if (is.fail()) throw InputError(ioTag.c_str());
}

int Subject::charCount() const {
  return (ioTag.length() + 1
			 + itsName.length() + 1
			 + itsDirectory.length() + 1
			 + 5);// fudge factor
}

void Subject::readFrom(Reader* reader) {
DOTRACE("Subject::readFrom");

  reader->readValue("name", itsName);
  reader->readValue("directory", itsDirectory);
}

void Subject::writeTo(Writer* writer) const {
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
