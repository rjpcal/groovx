/** @file visx/nullresponsehdlr.cc response handler that ignores all
    user responses */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 21 18:54:36 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "visx/nullresponsehdlr.h"

#include "nub/ref.h"

#include "tcl-gfx/toglet.h"

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

NullResponseHdlr::~NullResponseHdlr() noexcept
{
GVX_TRACE("NullResponseHdlr::~NullResponseHdlr");
}

void NullResponseHdlr::read_from(io::reader&)
{
GVX_TRACE("NullResponseHdlr::read_from");
}

void NullResponseHdlr::write_to(io::writer&) const
{
GVX_TRACE("NullResponseHdlr::write_to");
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
