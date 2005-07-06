/** @file gfx/gxfont.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 12 18:35:02 2002
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

#ifndef GROOVX_GFX_GXFONT_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GXFONT_CC_UTC20050626084025_DEFINED

#include "gxfont.h"

#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <map>

namespace
{
  typedef std::map<rutz::fstring, rutz::shared_ptr<GxFont> > MapType;
  MapType theFontMap;
}

rutz::shared_ptr<GxFont> GxFont::make(const char* name_cstr)
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

  rutz::shared_ptr<GxFont> font(GxRasterFont::make(name_cstr));
  theFontMap.insert(MapType::value_type(name, font));
  return font;
}

GxFont::~GxFont() throw() {}

static const char vcid_groovx_gfx_gxfont_cc_utc20050626084025[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXFONT_CC_UTC20050626084025_DEFINED
