///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Nov 21 17:58:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#include "tcl/tkwidget.h"

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

  /// Overridden from GWT::Widget.
  virtual Gfx::Canvas& getCanvas() const;

  void makeCurrent() const;

  /// Overridden from Tcl::TkWidget.
  virtual void displayCallback();

  /// Overridden from GWT::Widget.
  virtual void swapBuffers();

  /// Simple RGB color struct.
  struct Color
  {
    Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
      pixel(p), red(r), green(g), blue(b) {}
    unsigned int pixel;
    double red, green, blue;
  };

  Color queryColor(unsigned int color_index) const;

  class Impl;

private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  Impl* const rep;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
