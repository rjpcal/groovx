///////////////////////////////////////////////////////////////////////
//
// nullresponsehdlr.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 21 18:54:36 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef NULLRESPONSEHDLR_CC_DEFINED
#define NULLRESPONSEHDLR_CC_DEFINED

#include "gfx/toglet.h"

#include "nub/ref.h"

#include "visx/nullresponsehdlr.h"

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

NullResponseHdlr::~NullResponseHdlr() throw()
{
DOTRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::readFrom(IO::Reader&)
{
DOTRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(IO::Writer&) const
{
DOTRACE("NullResponseHdlr::writeTo");
}

// actions
void NullResponseHdlr::rhBeginTrial(Nub::SoftRef<Toglet>, Trial&) const
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

void NullResponseHdlr::rhAllowResponses(Nub::SoftRef<Toglet>, Trial&) const
{
DOTRACE("NullResponseHdlr::rhAllowResponses");
}

void NullResponseHdlr::rhDenyResponses() const
{
DOTRACE("NullResponseHdlr::rhDenyResponses");
}

static const char vcid_nullresponsehdlr_cc[] = "$Header$";
#endif // !NULLRESPONSEHDLR_CC_DEFINED
