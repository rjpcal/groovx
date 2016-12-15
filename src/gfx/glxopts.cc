/** @file gfx/glxopts.cc options for controlling the
    OpenGL-windowsystem interface */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sun Aug  4 16:29:23 2002
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

#include "glxopts.h"

#include "rutz/trace.h"

GlxOpts::GlxOpts() :
#if defined(GVX_NO_RGBA) && GVX_NO_RGBA != 0
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
#if defined(GVX_NO_DOUBLE_BUFFER) && GVX_NO_DOUBLE_BUFFER != 0
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
#if defined(GVX_NO_DIRECT_RENDER) && GVX_NO_DIRECT_RENDER != 0
  indirect( true ),
#else
  indirect( false ),
#endif
  level( 0 ),
  transparent( false )
{
GVX_TRACE("GlxOpts::GlxOpts");
}
