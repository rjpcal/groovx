/** @file gfx/aglwrapper.h GlWindowInterface implementation using
    Apple's AGL API */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jul 20 10:18:14 2004
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

#ifndef GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED

#include "gfx/glwindowinterface.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <AGL/agl.h>

struct GlxOpts;

/// Wraps the AGLContext mechanism.
class AglWrapper : public GlWindowInterface
{
private:
  AGLPixelFormat itsPixFormat;
  AGLContext itsContext;
  CGrafPtr itsDrawable; // AGLDrawable is a typedef for CGrafPtr

  AglWrapper(const AglWrapper&);
  AglWrapper& operator=(const AglWrapper&);

  /// Construct.
  AglWrapper(GlxOpts& opts);

public:
  /// Factory function.
  static AglWrapper* make(GlxOpts& opts);

  /// Destructor.
  virtual ~AglWrapper();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const override;

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const override;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const override;

  /// Make our rendering context the current active one.
  virtual void makeCurrent() override;

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height) override;

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const override;

  // AGL-specific functions

  void bindDrawable(CGrafPtr drawable) { itsDrawable = drawable; }
};

#pragma clang diagnostic pop

#endif // !GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED
