///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu May 15 16:14:42 2003
// $Id$
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

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#include "tcl/tkwidget.h"

class GxCamera;
class GxNode;
class GxScene;

namespace Gfx
{
  class Canvas;
}

namespace Util
{
  template <class T> class Ref;
}

// ####################################################################

/// Toglet is an OpenGL Tk widget.

class Toglet : public Tcl::TkWidget
{
protected:
  Toglet(bool pack=true);

public:

  /// Returns a new Toglet object.
  /** Note that the object will be "volatile" -- it manages its own
      lifetime. */
  static Toglet* make();

  virtual ~Toglet();

  virtual bool isNotShareable() const { return true; }

  /// Specify which Tk window should be the parent of new Toglet's.
  static void defaultParent(const char* pathname);

  /// Return the associated canvas.
  Gfx::Canvas& getCanvas() const;

  void makeCurrent() const;

  /// Overridden from Tcl::TkWidget.
  virtual void displayCallback();

  /// Overridden from Tcl::TkWidget.
  virtual void reshapeCallback(int width, int height);

  /// Swap buffers in a double-buffered GL context.
  void swapBuffers();

  /// Simple RGB color struct.
  struct Color
  {
    Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
      pixel(p), red(r), green(g), blue(b) {}
    unsigned int pixel;
    double red, green, blue;
  };

  Color queryColor(unsigned int color_index) const;

  // Access to GxScene:

  GxScene& scene();

  void render();
  void fullRender();
  void clearscreen();
  void fullClearscreen();
  void undraw();
  void setVisibility(bool vis);
  void setHold(bool hold_on);
  void allowRefresh(bool allow);
  const Util::Ref<GxCamera>& getCamera() const;
  void setCamera(const Util::Ref<GxCamera>& cam);
  void setDrawable(const Util::Ref<GxNode>& node);
  void animate(unsigned int framesPerSecond);


  class Impl;

private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  Impl* const rep;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
