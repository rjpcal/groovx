/** @file visx/element.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Dec  4 15:40:25 2002
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

#ifndef GROOVX_VISX_ELEMENT_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_ELEMENT_CC_UTC20050626084016_DEFINED

#include "visx/element.h"

Element::~Element() throw() {}

void Element::vxEndTrialHook() { /* no-op */ }

static const char vcid_groovx_visx_element_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_ELEMENT_CC_UTC20050626084016_DEFINED
