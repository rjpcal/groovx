///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
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

namespace Nub
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

// ########################################################

/// Toglet is an OpenGL Tk widget.

class Toglet : public Tcl::TkWidget
{
protected:
  Toglet(bool pack=true, bool topLevel=false);

public:

  virtual ~Toglet() throw();

  /// Returns a new Toglet object.
  /** Note that the object will be "volatile" -- it manages its own
      lifetime. */
  static Toglet* make();

  /// Returns a new top-level Toglet object.
  /** Note that the object will be "volatile" -- it manages its own
      lifetime. */
  static Toglet* makeToplevel();

  /// Find the Toglet that most recently had makeCurrent() called for it.
  static Nub::SoftRef<Toglet> getCurrent();

  /// Equivalent to calling toglet->makeCurrent().
  static void setCurrent(Nub::SoftRef<Toglet> toglet);

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
  const Nub::Ref<GxCamera>& getCamera() const;
  void setCamera(const Nub::Ref<GxCamera>& cam);
  void setDrawable(const Nub::Ref<GxNode>& node);
  void animate(unsigned int framesPerSecond);


private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  class Impl;
  Impl* const rep;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
