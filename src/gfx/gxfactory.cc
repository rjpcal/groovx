/** @file gfx/gxfactory.cc factory class for platform-specific graphics
    objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jul  6 15:49:13 2005
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

#ifndef GROOVX_GFX_GXFACTORY_CC_UTC20050706224913_DEFINED
#define GROOVX_GFX_GXFACTORY_CC_UTC20050706224913_DEFINED

#include "gfx/gxfactory.h"

#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"

#if defined(GVX_GL_PLATFORM_GLX)
#  include "gfx/glxrasterfont.h"
#elif defined(GVX_GL_PLATFORM_AGL)
#  include "gfx/aglrasterfont.h"
#else
#  error no GVX_GL_PLATFORM macro defined
#endif

#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <map>

#include "rutz/trace.h"

namespace
{
  typedef std::map<rutz::fstring, rutz::shared_ptr<GxFont> > MapType;
  MapType theFontMap;
}

rutz::shared_ptr<GxRasterFont>
GxFactory::makeRasterFont(const char* fontname)
{
GVX_TRACE("GxFactory::makeRasterFont");

#if defined(GVX_GL_PLATFORM_GLX)
  return rutz::make_shared(new GlxRasterFont(fontname));
#elif defined(GVX_GL_PLATFORM_AGL)
  return rutz::make_shared(new AglRasterFont(fontname));
#endif
}

rutz::shared_ptr<GxFont> GxFactory::makeFont(const char* name_cstr)
{
  rutz::fstring name(name_cstr);

  MapType::iterator itr = theFontMap.find(name);

  if (itr != theFontMap.end())
    return (*itr).second;

  if (name == "vector")
    {
      rutz::shared_ptr<GxFont> font(new GxVectorFont);
      theFontMap.insert(MapType::value_type(name, font));
      return font;
    }

  rutz::shared_ptr<GxFont> font(GxFactory::makeRasterFont(name_cstr));
  theFontMap.insert(MapType::value_type(name, font));
  return font;
}

static const char __attribute__((used)) vcid_groovx_gfx_gxfactory_cc_utc20050706224913[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXFACTORY_CC_UTC20050706224913DEFINED
