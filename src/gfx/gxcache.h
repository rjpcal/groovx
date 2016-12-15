/** @file gfx/gxcache.h GxBin subclass that can cache its child node
    in an OpenGL display list */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 13 14:03:23 2002
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
  GxCache(nub::soft_ref<GxNode> child);

  /// Virtual destructor.
  virtual ~GxCache() noexcept;

  virtual io::version_id class_version_id() const override;

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Draw using the current draw mode.
  virtual void draw(Gfx::Canvas& canvas) const override;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  /// Invalidate any current cache.
  void invalidate() noexcept;

  /// Get the current drawing mode.
  Mode getMode() const { return itsMode; }

  /// Set the current drawing mode.
  void setMode(Mode new_mode) noexcept;

private:
  Mode itsMode;

  mutable unsigned int itsDisplayList;
  mutable nub::soft_ref<GLCanvas> itsCanvas;
};

#endif // !GROOVX_GFX_GXCACHE_H_UTC20050626084023_DEFINED
