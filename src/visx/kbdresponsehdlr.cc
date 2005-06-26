///////////////////////////////////////////////////////////////////////
//
// kbdresponsehdlr.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun 21 18:09:12 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_VISX_KBDRESPONSEHDLR_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_KBDRESPONSEHDLR_CC_UTC20050626084016_DEFINED

#include "visx/kbdresponsehdlr.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"

///////////////////////////////////////////////////////////////////////
//
// KbdResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

KbdResponseHdlr* KbdResponseHdlr::make()
{
GVX_TRACE("KbdResponseHdlr::make");
  return new KbdResponseHdlr;
}

KbdResponseHdlr::KbdResponseHdlr() :
  EventResponseHdlr()
{
  setEventSequence("<KeyPress>");
  setBindingSubstitution("%K");
}

KbdResponseHdlr::~KbdResponseHdlr() throw() {}

static const char vcid_groovx_visx_kbdresponsehdlr_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_KBDRESPONSEHDLR_CC_UTC20050626084016_DEFINED
