///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Sat Nov 23 17:46:38 2002
// $Id$
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

  static Toglet* make() { return new Toglet; }

  virtual ~Toglet();

  virtual bool isNotShareable() const { return true; }

  /// Specify which Tk window should be the parent of new Toglet's.
  static void defaultParent(const char* pathname);

  /// Return the associated canvas.
  Gfx::Canvas& getCanvas() const;

  void makeCurrent() const;

  /// Overridden from Tcl::TkWidget.
  virtual void displayCallback();

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


  /// Overridden from Tcl::TkWidget.
  virtual void reshapeCallback(int width, int height);


  void undraw();

  /** "Bare-bones rendering": only render the current object; the caller is
      expected to take care of clearing the color buffer first and flushing
      the graphics stream afterwards. */
  void render();

  /** "Full-featured rendering": first clears the color buffer, then renders
      the current object, then flushes the graphics stream and swaps buffers
      if necessary. */
  void fullRender();

  /** "Bare-bones clearscreen": clear the color buffer and set the current
      object to empty, but don't flush the graphics stream. */
  void clearscreen();

  /** "Full-featured clearscreen": clear the color buffer, set the current
      object to empty, and flush the graphics stream. */
  void fullClearscreen();

  // Change the global visibility, which determines whether anything
  // will be displayed by a "redraw" command, or by remap events sent
  // to the screen window.
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
  GxScene* const itsScene;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
