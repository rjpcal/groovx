///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:36 1999
// written: Wed Sep 27 13:50:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "nullresponsehdlr.h"

#include "io/iolegacy.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"

namespace {
  const char* ioTag = "NullResponseHdlr";
}

NullResponseHdlr::NullResponseHdlr() {
DOTRACE("NullResponseHdlr::NullResponseHdlr");
}

NullResponseHdlr::~NullResponseHdlr() {
DOTRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::legacySrlz(IO::Writer* writer) const {
DOTRACE("NullResponseHdlr::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 lwriter->writeTypename(ioTag);
  }
}

void NullResponseHdlr::legacyDesrlz(IO::Reader* reader) {
DOTRACE("NullResponseHdlr::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->readTypename(ioTag);
  }
}

void NullResponseHdlr::readFrom(IO::Reader*) {
DOTRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(IO::Writer*) const {
DOTRACE("NullResponseHdlr::writeTo");
}

// manipulators/accessors
void NullResponseHdlr::setInterp(Tcl_Interp*) {
  // do nothing, since we don't need a Tcl_Interp* for this class
}

// actions
void NullResponseHdlr::rhBeginTrial(GWT::Widget&, TrialBase&) const {
DOTRACE("NullResponseHdlr::rhBeginTrial");
}

void NullResponseHdlr::rhAbortTrial()  const {
DOTRACE("NullResponseHdlr::rhAbortTrial");
}

void NullResponseHdlr::rhEndTrial() const {
DOTRACE("NullResponseHdlr::rhEndTrial");
}

void NullResponseHdlr::rhHaltExpt() const {
DOTRACE("NullResponseHdlr::rhHaltExpt");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
