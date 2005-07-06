/** @file gfx/gxfactory.h factory class for platform-specific graphics
    objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jul  6 15:47:01 2005
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

#ifndef GROOVX_GFX_GXFACTORY_H_UTC20050706224701_DEFINED
#define GROOVX_GFX_GXFACTORY_H_UTC20050706224701_DEFINED

#include <tk.h>

class GlWindowInterface;
class GlxOpts;
class GxFont;
class GxRasterFont;

namespace rutz
{
  template <class T> class shared_ptr;
}

/// Provides factory functions to create platform-specific objects.
class GxFactory
{
public:
  /// Construct a platform-appropriate raster font from the given X11 font specification.
  static rutz::shared_ptr<GxRasterFont>
  makeRasterFont(const char* fontname);

  /// Create a platform-appropriate raster or vector font.
  static rutz::shared_ptr<GxFont> makeFont(const char* name);

  /// Make a concrete GlWindowInterface of a platform-dependent type.
  static rutz::shared_ptr<GlWindowInterface>
  makeWindowInterface(Display* dpy, GlxOpts& opts);
};

static const char vcid_groovx_gfx_gxfactory_h_utc20050706224701[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXFACTORY_H_UTC20050706224701DEFINED
