///////////////////////////////////////////////////////////////////////
// subject.cc
// Rob Peters
// created: Dec-98
// written: Tue Mar 16 19:32:51 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <cstring>

#include "ioutils.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
// Subject member funcitons
///////////////////////////////////////////////////////////////////////

Subject::Subject(const char *name, const char* dir) : 
  itsName(NULL), itsDirectory(NULL) {
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
  if (itsName != NULL)
    delete [] itsName;
  if (itsDirectory != NULL)
    delete [] itsDirectory;
}

Subject::Subject(istream &is) {
  deserialize(is);
  if (checkStream(is) != IO_OK) fatalInputError("Subject");
}

IOResult Subject::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Subject::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Subject).name() << sep; }

  serializeCstring(os, itsName, sep);
  serializeCstring(os, itsDirectory, sep);
  return checkStream(os);
}

IOResult Subject::deserialize(istream &is, IOFlag flag) {
DOTRACE("Subject::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != string(typeid(Subject).name())) { return IO_ERROR; }
  }

  deserializeCstring(is, itsName);
  deserializeCstring(is, itsDirectory);
  return checkStream(is);
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
