///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:54:36 1999
// written: Wed Jun  6 09:44:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "nullresponsehdlr.h"

#define NO_TRACE
#include "util/trace.h"

NullResponseHdlr* NullResponseHdlr::make() {
DOTRACE("NullResponseHdlr::make");
  return new NullResponseHdlr;
}

NullResponseHdlr::NullResponseHdlr() {
DOTRACE("NullResponseHdlr::NullResponseHdlr");
}

NullResponseHdlr::~NullResponseHdlr() {
DOTRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::readFrom(IO::Reader*) {
DOTRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(IO::Writer*) const {
DOTRACE("NullResponseHdlr::writeTo");
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

void NullResponseHdlr::rhAllowResponses(GWT::Widget&, TrialBase&) const {
DOTRACE("NullResponseHdlr::rhAllowResponses");
}

void NullResponseHdlr::rhDenyResponses() const {
DOTRACE("NullResponseHdlr::rhDenyResponses");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
