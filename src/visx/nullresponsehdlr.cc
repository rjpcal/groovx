///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 18:54:36 1999
// written: Wed Feb 26 16:30:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "visx/nullresponsehdlr.h"

#include "tcl/toglet.h"

#include "util/ref.h"

#include "util/trace.h"

NullResponseHdlr* NullResponseHdlr::make()
{
DOTRACE("NullResponseHdlr::make");
  return new NullResponseHdlr;
}

NullResponseHdlr::NullResponseHdlr()
{
DOTRACE("NullResponseHdlr::NullResponseHdlr");
}

NullResponseHdlr::~NullResponseHdlr()
{
DOTRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::readFrom(IO::Reader*)
{
DOTRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(IO::Writer*) const
{
DOTRACE("NullResponseHdlr::writeTo");
}

// actions
void NullResponseHdlr::rhBeginTrial(Util::SoftRef<Toglet>, Trial&) const
{
DOTRACE("NullResponseHdlr::rhBeginTrial");
}

void NullResponseHdlr::rhAbortTrial()  const
{
DOTRACE("NullResponseHdlr::rhAbortTrial");
}

void NullResponseHdlr::rhEndTrial() const
{
DOTRACE("NullResponseHdlr::rhEndTrial");
}

void NullResponseHdlr::rhHaltExpt() const
{
DOTRACE("NullResponseHdlr::rhHaltExpt");
}

void NullResponseHdlr::rhAllowResponses(Util::SoftRef<Toglet>, Trial&) const
{
DOTRACE("NullResponseHdlr::rhAllowResponses");
}

void NullResponseHdlr::rhDenyResponses() const
{
DOTRACE("NullResponseHdlr::rhDenyResponses");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
