///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 16:45:32 2002
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

#ifndef GXRASTERFONT_CC_DEFINED
#define GXRASTERFONT_CC_DEFINED

#include "gxrasterfont.h"

#include "util/pointers.h"

#include "util/trace.h"

GxRasterFont::~GxRasterFont() throw()
{
DOTRACE("GxRasterFont::~GxRasterFont");
}

#if defined(GL_PLATFORM_GLX)

#include "gfx/glxrasterfont.h"

shared_ptr<GxRasterFont> GxRasterFont::make(const char* fontname)
{
DOTRACE("GxRasterFont::make[glx]");
  return shared_ptr<GxRasterFont>(new GlxRasterFont(fontname));
}

#elif defined(GL_PLATFORM_AGL)

#include "gfx/aglrasterfont.h"

shared_ptr<GxRasterFont> GxRasterFont::make(const char* fontname)
{
DOTRACE("GxRasterFont::make[agl]");
  return shared_ptr<GxRasterFont>(new AglRasterFont(fontname));
}

#else
#  error no GL_PLATFORM macro defined
#endif

static const char vcid_gxrasterfont_cc[] = "$Header$";
#endif // !GXRASTERFONT_CC_DEFINED
