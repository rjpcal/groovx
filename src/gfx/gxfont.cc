///////////////////////////////////////////////////////////////////////
//
// gxfont.cc
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 12 18:35:02 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_CC_DEFINED
#define GXFONT_CC_DEFINED

#include "gxfont.h"

#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/strings.h"

#include <map>

namespace
{
  typedef std::map<fstring, shared_ptr<GxFont> > MapType;
  MapType theFontMap;
}

shared_ptr<GxFont> GxFont::make(const char* name_cstr)
{
  fstring name(name_cstr);

  MapType::iterator itr = theFontMap.find(name);

  if (itr != theFontMap.end())
    return (*itr).second;

  if (name == "vector")
    {
      shared_ptr<GxFont> font(new GxVectorFont);
      theFontMap.insert(MapType::value_type(name, font));
      return font;
    }

  shared_ptr<GxFont> font(new GxRasterFont(name_cstr));
  theFontMap.insert(MapType::value_type(name, font));
  return font;
}

GxFont::~GxFont() throw() {}

static const char vcid_gxfont_cc[] = "$Header$";
#endif // !GXFONT_CC_DEFINED
