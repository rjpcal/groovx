///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:36 1999
// written: Thu May 11 20:00:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "nullresponsehdlr.h"

#include <cstring>
#include <iostream.h>

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

void NullResponseHdlr::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("NullResponseHdlr::serialize");

  if (flag & IO::TYPENAME) { os << ioTag << ' '; }
}

void NullResponseHdlr::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("NullResponseHdlr::deserialize");

  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }
}

int NullResponseHdlr::charCount() const {
DOTRACE("NullResponseHdlr::charCount");
  return strlen(ioTag) + 1;
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
