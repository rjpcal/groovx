///////////////////////////////////////////////////////////////////////
// subject.cc
// Rob Peters
// created: Dec-98
// written: Fri Mar 12 11:25:30 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include <iostream.h>
#include <cstring>

#include "ioutils.h"

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
  deserializeCstring(is, itsName, "Subject");
  deserializeCstring(is, itsDirectory, "Subject");
}

IOResult Subject::serialize(ostream &os, IOFlag flag) const {
  char sep = '\t';
  if (serializeCstring(os, itsName, sep) != IO_OK)
    return IO_ERROR;
  if (serializeCstring(os, itsDirectory, sep) != IO_OK)
    return IO_ERROR;
  return IO_OK;
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

#endif // !SUBJECT_CC_DEFINED
