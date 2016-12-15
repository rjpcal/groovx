/** @file gfx/gxfactory.cc factory class for platform-specific graphics
    objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jul  6 15:49:13 2005
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

#include "gfx/gxfactory.h"

#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"

#if defined(GVX_GL_PLATFORM_GLX)
#  include "gfx/glxrasterfont.h"
#elif defined(GVX_GL_PLATFORM_AGL)
// #  define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_3
// #  include "gfx/aglrasterfont.h"
#else
#  error no GVX_GL_PLATFORM macro defined
#endif

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <map>
#include <memory>

#include "rutz/trace.h"

namespace
{
  typedef std::map<rutz::fstring, std::shared_ptr<GxFont> > MapType;
  MapType theFontMap;
}

std::shared_ptr<GxRasterFont>
GxFactory::makeRasterFont(const char* fontname)
{
GVX_TRACE("GxFactory::makeRasterFont");

#if defined(GVX_GL_PLATFORM_GLX)
  return std::make_shared<GlxRasterFont>(fontname);
#elif defined(GVX_GL_PLATFORM_AGL)
//   return rutz::make_shared(new AglRasterFont(fontname));
  throw rutz::error(rutz::sfmt("couldn't create font %s: AglRasterFont not supported",
                               fontname), SRC_POS);
#endif
}

std::shared_ptr<GxFont> GxFactory::makeFont(const char* name_cstr)
{
  rutz::fstring name(name_cstr);

  MapType::iterator itr = theFontMap.find(name);

  if (itr != theFontMap.end())
    return (*itr).second;

  if (name == "vector")
    {
      std::shared_ptr<GxFont> font(new GxVectorFont);
      theFontMap.insert(MapType::value_type(name, font));
      return font;
    }

  std::shared_ptr<GxFont> font(GxFactory::makeRasterFont(name_cstr));
  theFontMap.insert(MapType::value_type(name, font));
  return font;
}
