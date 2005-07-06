/** @file gfx/glwindowinterface.h abstract base class that wraps the
    interface between OpenGL and a particular windowing system */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jul 19 18:42:10 2004
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

#ifndef GROOVX_GFX_GLWINDOWINTERFACE_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GLWINDOWINTERFACE_H_UTC20050626084024_DEFINED

#include <tk.h>

class GlxOpts;

namespace rutz
{
  template <class T> class shared_ptr;
}

/// Abstract base class that wraps the interface between OpenGL and a particular windowing system.
class GlWindowInterface
{
public:
  /// Virtual destructor.
  virtual ~GlWindowInterface();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const = 0;

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const = 0;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const = 0;

  /// Bind the rendering context to the given window.
  virtual void makeCurrent(Window win) = 0;

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height) = 0;

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const = 0;

  /// Instantiate an actual window-system window for the given Tk_Window.
  virtual Window makeTkRealWindow(Tk_Window tkwin, Window parent,
                                  int width, int height) throw() = 0;

  /// Make a concrete GlWindowInterface of a platform-dependent type.
  static rutz::shared_ptr<GlWindowInterface> make(Display* dpy,
                                                  GlxOpts& opts);
};

static const char vcid_groovx_gfx_glwindowinterface_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GLWINDOWINTERFACE_H_UTC20050626084024_DEFINED
