///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Aug  3 16:38:03 2002
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

#ifndef GLXWRAPPER_H_DEFINED
#define GLXWRAPPER_H_DEFINED

#if defined(GL_PLATFORM_GLX)

#include "gfx/glwindowinterface.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

class GlxOpts;

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
  virtual bool isDirect() const
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const;

  /// Bind the rendering context to the given window.
  virtual void makeCurrent(Window win);

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height) = 0;

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const;

  /// Instantiate an actual X11 window for the given Tk_Window.
  virtual Window makeTkRealWindow(Tk_Window tkwin, Window parent,
                                  int width, int height) throw();

  // GLX-specific functions:

  /// Get the associated X11 Display.
  Display* display() const { return itsDisplay; }

  /// Get the currently associated X11 Window.
  Window window() const { return itsCurrentWin; }

  /// Get the associated GLXContext.
  GLXContext context() const { return itsContext; }

  /// Get the associated X11 XVisualInfo.
  XVisualInfo* visInfo() const { return itsVisInfo; }
};

#endif // defined(GL_PLATFORM_GLX)

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
