///////////////////////////////////////////////////////////////////////
//
// subject.cc
// Rob Peters
// created: Dec-98
// written: Wed Sep 27 13:50:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECT_CC_DEFINED
#define SUBJECT_CC_DEFINED

#include "subject.h"

#include "io/iolegacy.h"
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

void Subject::legacySrlz(IO::Writer* writer) const {
DOTRACE("Subject::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 lwriter->writeTypename(ioTag.c_str());

	 ostream& os = lwriter->output();

	 os << itsName << endl;
	 os << itsDirectory << endl;

	 lwriter->throwIfError(ioTag.c_str());
  }
}

void Subject::legacyDesrlz(IO::Reader* reader) {
DOTRACE("Subject::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->readTypename(ioTag.c_str());

	 istream& is = lreader->input();

	 getline(is, itsName, '\n');
	 getline(is, itsDirectory, '\n');

	 lreader->throwIfError(ioTag.c_str());
  }
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
