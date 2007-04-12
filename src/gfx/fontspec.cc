/** @file gfx/fontspec.cc Parse fontspec strings */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Nov 11 14:56:22 2005
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

#ifndef GROOVX_GFX_FONTSPEC_CC_UTC20051111225622_DEFINED
#define GROOVX_GFX_FONTSPEC_CC_UTC20051111225622_DEFINED

#include "gfx/fontspec.h"

#include "rutz/fstring.h"

#include <vector>

void parseFontSpec(const rutz::fstring& spec,
                   rutz::fstring* family,
                   rutz::fstring* pxlsize,
                   rutz::fstring* mods)
{
  unsigned int i = 0;

  std::vector<char> vfamily;
  std::vector<char> vpxlsize;
  std::vector<char> vmods;

  for ( ; i < spec.length(); ++i)
    {
      if (spec[i] == ':')
        { ++i; break; }
      else
        vfamily.push_back(spec[i]);
    }

  for ( ; i < spec.length(); ++i)
    {
      if (spec[i] == ':')
        { ++i; break; }
      else
        vpxlsize.push_back(spec[i]);
    }

  for ( ; i < spec.length(); ++i)
    {
      if (spec[i] == ':')
        { ++i; break; }
      else
        vmods.push_back(spec[i]);
    }

  vfamily.push_back('\0');
  vpxlsize.push_back('\0');
  vmods.push_back('\0');

  *family = rutz::fstring(&vfamily[0]);
  *pxlsize = rutz::fstring(&vpxlsize[0]);
  *mods = rutz::fstring(&vmods[0]);
}

static const char __attribute__((used)) vcid_groovx_gfx_fontspec_cc_utc20051111225622[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_FONTSPEC_CC_UTC20051111225622DEFINED
