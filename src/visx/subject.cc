///////////////////////////////////////////////////////////////////////
//
// subject.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Fri Nov 10 17:27:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include "io/reader.h"
#include "io/writer.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

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
