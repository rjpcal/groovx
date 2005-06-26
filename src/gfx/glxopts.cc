///////////////////////////////////////////////////////////////////////
//
// glxopts.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Aug  4 16:29:23 2002
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

#ifndef GROOVX_GFX_GLXOPTS_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GLXOPTS_CC_UTC20050626084023_DEFINED

#include "glxopts.h"

#include "rutz/trace.h"

GlxOpts::GlxOpts() :
#if defined(NO_RGBA) && NO_RGBA != 0
  rgbaFlag( false ),
#else
  rgbaFlag( true ),
#endif
  rgbaRed( 1 ),
  rgbaGreen( 1 ),
  rgbaBlue( 1 ),
  alphaFlag( false ),
  alphaSize( 1 ),
  colorIndexSize( 8 ),
#if defined(NO_DOUBLE_BUFFER) && NO_DOUBLE_BUFFER != 0
  doubleFlag( false ),
#else
  doubleFlag( true ),
#endif
  depthFlag( true ),
  depthSize( 8 ),
  accumFlag( false ),
  accumRed( 1 ),
  accumGreen( 1 ),
  accumBlue( 1 ),
  accumAlpha( 1 ),
  stencilFlag( false ),
  stencilSize( 1 ),
  auxNumber( 0 ),
#if defined(NO_DIRECT_RENDER) && NO_DIRECT_RENDER != 0
  indirect( true ),
#else
  indirect( false ),
#endif
  level( 0 ),
  transparent( false )
{
GVX_TRACE("GlxOpts::GlxOpts");
}

static const char vcid_groovx_gfx_glxopts_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GLXOPTS_CC_UTC20050626084023_DEFINED
