///////////////////////////////////////////////////////////////////////
//
// subject.cc
// Rob Peters
// created: Dec-98
// written: Wed Sep 27 11:23:53 2000
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
	 ostream& os = lwriter->output();
	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 os << itsName << endl;
	 os << itsDirectory << endl;

	 if (os.fail()) throw IO::OutputError(ioTag.c_str());
  }
}

void Subject::legacyDesrlz(IO::Reader* reader) {
DOTRACE("Subject::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }

	 getline(is, itsName, '\n');
	 getline(is, itsDirectory, '\n');

	 if (is.fail()) throw IO::InputError(ioTag.c_str());
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
