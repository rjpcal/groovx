///////////////////////////////////////////////////////////////////////
//
// aglwrapper.h
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 20 10:18:14 2004
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

#ifndef AGLWRAPPER_H_DEFINED
#define AGLWRAPPER_H_DEFINED

#if defined(GL_PLATFORM_AGL)

#include "gfx/glwindowinterface.h"

#include <AGL/agl.h>

class GlxOpts;

/// Wraps the GLXContext mechanism.
class AglWrapper : public GlWindowInterface
{
private:
  Display* itsDisplay;
  AGLPixelFormat itsPixFormat;
  AGLContext itsContext;
  Window itsCurrentWin;

  AglWrapper(const AglWrapper&);
  AglWrapper& operator=(const AglWrapper&);

  /// Construct.
  AglWrapper(Display* dpy, GlxOpts& opts);

public:
  /// Factory function.
  static AglWrapper* make(Display* dpy, GlxOpts& opts);

  /// Destructor.
  virtual ~AglWrapper();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const;

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const;

  /// Bind the rendering context to the given window.
  virtual void makeCurrent(Window win);

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height);

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const;

  /// Instantiate an actual Aqua window for the given Tk_Window.
  virtual Window makeTkRealWindow(Tk_Window tkwin, Window parent,
                                  int width, int height) throw();
};

#endif // defined (GL_PLATFORM_AGL)

static const char vcid_aglwrapper_h[] = "$Header$";
#endif // !AGLWRAPPER_H_DEFINED
