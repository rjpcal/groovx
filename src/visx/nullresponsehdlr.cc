///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 18:54:36 1999
// written: Sat Jul  3 16:34:45 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "nullresponsehdlr.h"

#include "exptdriver.h"

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

// manipulators/accessors
void NullResponseHdlr::setInterp(Tcl_Interp* interp) {
  itsInterp = interp;
}

// actions
void NullResponseHdlr::rhBeginTrial() const {
DOTRACE("NullResponseHdlr::rhBeginTrial");
}

void NullResponseHdlr::rhAbortTrial() const {
DOTRACE("NullResponseHdlr::rhAbortTrial");
}

void NullResponseHdlr::rhHaltExpt() const {
DOTRACE("NullResponseHdlr::rhHaltExpt");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
