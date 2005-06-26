///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 16:45:32 2002
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

#ifndef GROOVX_GFX_GXRASTERFONT_CC_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXRASTERFONT_CC_UTC20050626084023_DEFINED

#include "gxrasterfont.h"

#include "rutz/sharedptr.h"

#include "rutz/trace.h"

GxRasterFont::~GxRasterFont() throw()
{
GVX_TRACE("GxRasterFont::~GxRasterFont");
}

#if defined(GVX_GL_PLATFORM_GLX)

#include "gfx/glxrasterfont.h"

rutz::shared_ptr<GxRasterFont> GxRasterFont::make(const char* fontname)
{
GVX_TRACE("GxRasterFont::make[glx]");
  return rutz::make_shared(new GlxRasterFont(fontname));
}

#elif defined(GVX_GL_PLATFORM_AGL)

#include "gfx/aglrasterfont.h"

rutz::shared_ptr<GxRasterFont> GxRasterFont::make(const char* fontname)
{
GVX_TRACE("GxRasterFont::make[agl]");
  return rutz::make_shared(new AglRasterFont(fontname));
}

#else
#  error no GVX_GL_PLATFORM macro defined
#endif

static const char vcid_groovx_gfx_gxrasterfont_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXRASTERFONT_CC_UTC20050626084023_DEFINED
