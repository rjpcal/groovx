///////////////////////////////////////////////////////////////////////
//
// subject.cc
// Rob Peters
// created: Dec-98
// written: Thu Oct 21 18:47:52 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include <iostream.h>
#include <string>
#include <cstring>

#include "ioutils.h"
#include "reader.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "Subject";
}

///////////////////////////////////////////////////////////////////////
//
// Subject member funcitons
//
///////////////////////////////////////////////////////////////////////

Subject::Subject(const char* name, const char* dir) : 
  itsName(NULL), itsDirectory(NULL)
{
DOTRACE("Subject::Subject");
  if (name != NULL) {
    itsName = new char[strlen(name)+1];
    strcpy(itsName, name);
  }
  if (dir != NULL) {
    itsDirectory = new char[strlen(dir)+1];
    strcpy(itsDirectory, dir);
  }
}

Subject::~Subject() {
DOTRACE("Subject::~Subject");
  if (itsName != NULL)
    delete [] itsName;
  if (itsDirectory != NULL)
    delete [] itsDirectory;
}

Subject::Subject(istream &is, IOFlag flag) :
  itsName(NULL), itsDirectory(NULL)
{
DOTRACE("Subject::Subject");
  deserialize(is, flag);
}

void Subject::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Subject::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  serializeCstring(os, itsName, sep);
  serializeCstring(os, itsDirectory, sep);
  if (os.fail()) throw OutputError(ioTag);
}

void Subject::deserialize(istream &is, IOFlag flag) {
DOTRACE("Subject::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  deserializeCstring(is, itsName);
  deserializeCstring(is, itsDirectory);
  if (is.fail()) throw InputError(ioTag);
}

int Subject::charCount() const {
  return (ioTag.size() + 1
			 + 2 + strlen(itsName) + 1	// 2 chars for its length
			 + 2 + strlen(itsDirectory) + 1 // 2 chars for its length
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
  if (itsName != NULL)
    delete [] itsName;
  itsName = new char[strlen(name)+1];
  strcpy(itsName, name);
}

void Subject::setDirectory(const char* dir) {
  if (itsDirectory != NULL)
    delete [] itsDirectory;
  itsDirectory = new char[strlen(dir)+1];
  strcpy(itsDirectory, dir);
}

static const char vcid_subject_cc[] = "$Header$";
#endif // !SUBJECT_CC_DEFINED
