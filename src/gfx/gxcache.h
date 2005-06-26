///////////////////////////////////////////////////////////////////////
//
// gxcache.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 14:03:23 2002
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

#ifndef GROOVX_GFX_GXCACHE_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXCACHE_H_UTC20050626084023_DEFINED

#include "gfx/gxbin.h"

#include "nub/ref.h"

class GLCanvas;

/// A node for caching with OpenGL display lists.
class GxCache : public GxBin
{
public:

  ///////////////////////////////////////////////////////////
  //
  // Rendering modes
  //
  ///////////////////////////////////////////////////////////

  /** These symbolic constants control the way the object is drawn to
      and undrawn from the screen. The current modes can be get/set
      with \c getMode(), and \c setMode(). The default rendering mode
      is \c GLCOMPILE. **/
  typedef int Mode;

  /// In this mode, the object is rendered afresh each time it is drawn.
  static const Mode DIRECT = 1;

  /** In this mode, the object is rendered freshly on a draw request
      only if the object's state has changed since the last draw
      request. If it has not changed, then the object is instead
      rendered from either a cached OpenGL display list. The behavior
      of \c GLCOMPILE should be exactly the same as that of \c DIRECT,
      except that drawing should be faster if the object's state has
      not changed since the last draw. */
  static const Mode GLCOMPILE = 2;


  /// Construct with a given child object.
  GxCache(Nub::SoftRef<GxNode> child);

  /// Virtual destructor.
  virtual ~GxCache() throw();

  IO::VersionId serialVersionId() const;

  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Draw using the current draw mode.
  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Invalidate any current cache.
  void invalidate() throw();

  /// Get the current drawing mode.
  Mode getMode() const { return itsMode; }

  /// Set the current drawing mode.
  void setMode(Mode new_mode) throw();

private:
  Mode itsMode;

  mutable int itsDisplayList;
  mutable Nub::SoftRef<GLCanvas> itsCanvas;
};

static const char vcid_groovx_gfx_gxcache_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXCACHE_H_UTC20050626084023_DEFINED
