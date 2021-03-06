/** @file gfx/glxwrapper.h GlWindowInterface implementation using the
    X11 GLX API. */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Aug  3 16:38:03 2002
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

#ifndef GROOVX_GFX_GLXWRAPPER_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GLXWRAPPER_H_UTC20050626084024_DEFINED

#include "gfx/glwindowinterface.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

struct GlxOpts;

/// Wraps the GLXContext mechanism.
class GlxWrapper : public GlWindowInterface
{
private:
  Display* itsDisplay;
  XVisualInfo* itsVisInfo;
  GLXContext itsContext;
  Window itsCurrentWin;

  GlxWrapper(const GlxWrapper&);
  GlxWrapper& operator=(const GlxWrapper&);

  /// Construct.
  GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

public:
  /// Factory function.
  static GlxWrapper* make(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

  /// Destructor.
  virtual ~GlxWrapper();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const override
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

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

  // GLX-specific functions:

  /// Bind to a given X window
  void bindWindow(Window win) { itsCurrentWin = win; }

  /// Get the associated X11 Display.
  Display* display() const { return itsDisplay; }

  /// Get the currently associated X11 Window.
  Window window() const { return itsCurrentWin; }

  /// Get the associated GLXContext.
  GLXContext context() const { return itsContext; }

  /// Get the associated X11 XVisualInfo.
  XVisualInfo* visInfo() const { return itsVisInfo; }
};

#endif // !GROOVX_GFX_GLXWRAPPER_H_UTC20050626084024_DEFINED
