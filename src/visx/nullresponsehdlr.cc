/** @file visx/nullresponsehdlr.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun 21 18:54:36 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_NULLRESPONSEHDLR_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_NULLRESPONSEHDLR_CC_UTC20050626084016_DEFINED

#include "gfx/toglet.h"

#include "nub/ref.h"

#include "visx/nullresponsehdlr.h"

#include "rutz/trace.h"

NullResponseHdlr* NullResponseHdlr::make()
{
GVX_TRACE("NullResponseHdlr::make");
  return new NullResponseHdlr;
}

NullResponseHdlr::NullResponseHdlr()
{
GVX_TRACE("NullResponseHdlr::NullResponseHdlr");
}

NullResponseHdlr::~NullResponseHdlr() throw()
{
GVX_TRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::readFrom(IO::Reader&)
{
GVX_TRACE("NullResponseHdlr::readFrom");
}

void NullResponseHdlr::writeTo(IO::Writer&) const
{
GVX_TRACE("NullResponseHdlr::writeTo");
}

// actions
void NullResponseHdlr::rhBeginTrial(nub::soft_ref<Toglet>, Trial&) const
{
GVX_TRACE("NullResponseHdlr::rhBeginTrial");
}

void NullResponseHdlr::rhAbortTrial()  const
{
GVX_TRACE("NullResponseHdlr::rhAbortTrial");
}

void NullResponseHdlr::rhEndTrial() const
{
GVX_TRACE("NullResponseHdlr::rhEndTrial");
}

void NullResponseHdlr::rhHaltExpt() const
{
GVX_TRACE("NullResponseHdlr::rhHaltExpt");
}

void NullResponseHdlr::rhAllowResponses(nub::soft_ref<Toglet>, Trial&) const
{
GVX_TRACE("NullResponseHdlr::rhAllowResponses");
}

void NullResponseHdlr::rhDenyResponses() const
{
GVX_TRACE("NullResponseHdlr::rhDenyResponses");
}

static const char vcid_groovx_visx_nullresponsehdlr_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_NULLRESPONSEHDLR_CC_UTC20050626084016_DEFINED
