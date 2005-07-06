/** @file gfx/glwindowinterface.cc abstract base class that wraps the
    interface between OpenGL and a particular windowing system */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jul 19 18:42:28 2004
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

#ifndef GROOVX_GFX_GLWINDOWINTERFACE_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GLWINDOWINTERFACE_CC_UTC20050626084023_DEFINED

#include "glwindowinterface.h"

#include "rutz/trace.h"

GlWindowInterface::~GlWindowInterface()
{
GVX_TRACE("GlWindowInterface::~GlWindowInterface");
}

static const char vcid_groovx_gfx_glwindowinterface_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GLWINDOWINTERFACE_CC_UTC20050626084023_DEFINED
