///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:36 1999
// written: Wed Dec  1 14:37:40 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "nullresponsehdlr.h"

#define NO_TRACE
#include "trace.h"

namespace {
  const string ioTag = "NullResponseHdlr";
}

NullResponseHdlr::NullResponseHdlr() {
DOTRACE("NullResponseHdlr::NullResponseHdlr");
}

NullResponseHdlr::~NullResponseHdlr() {
DOTRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("NullResponseHdlr::serialize");

  if (flag & TYPENAME) { os << ioTag << ' '; }
}

void NullResponseHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("NullResponseHdlr::deserialize");

  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }
}

int NullResponseHdlr::charCount() const {
DOTRACE("NullResponseHdlr::charCount");
  return ioTag.length() + 1;
}

void NullResponseHdlr::readFrom(Reader* reader) {
DOTRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(Writer* writer) const {
DOTRACE("NullResponseHdlr::writeTo");
}

// manipulators/accessors
void NullResponseHdlr::setInterp(Tcl_Interp*) {
  // do nothing, since we don't need a Tcl_Interp* for this class
}

// actions
void NullResponseHdlr::rhBeginTrial(Experiment*) const {
DOTRACE("NullResponseHdlr::rhBeginTrial");
}

void NullResponseHdlr::rhAbortTrial(Experiment*) const {
DOTRACE("NullResponseHdlr::rhAbortTrial");
}

void NullResponseHdlr::rhEndTrial(Experiment*) const {
DOTRACE("NullResponseHdlr::rhEndTrial");
}

void NullResponseHdlr::rhHaltExpt(Experiment*) const {
DOTRACE("NullResponseHdlr::rhHaltExpt");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
