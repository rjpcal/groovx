///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Nov 21 14:41:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#include "tcl/tkwidget.h"

struct Tcl_Interp;

namespace Gfx
{
  class Canvas;
}

///////////////////////////////////////////////////////////////////////
//
// Togl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl : public Tcl::TkWidget
{
private:
  Togl(const Togl&);
  Togl& operator=(const Togl&);

public:
  Togl(Tcl_Interp* interp, const char* pathname);
  virtual ~Togl();

  virtual bool isNotShareable() const { return true; }

  /// Overridden from GWT::Widget.
  virtual Gfx::Canvas& getCanvas() const;

  /// Overridden from Tcl::TkWidget.
  virtual void displayCallback();

  void makeCurrent() const;

  /// Overridden from GWT::Widget.
  virtual void swapBuffers();

  /// Simple RGB color struct for use with Togl.
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
  Impl* const rep;
};

class TogletSizer;

// ####################################################################

/// Toglet is an OpenGL Tk widget implemented with the "Togl" library.

class Toglet : public Togl
{
protected:
  Toglet(bool pack=true);

public:

  static Toglet* make() { return new Toglet; }

  virtual ~Toglet();

  // accessors
  bool usingFixedScale() const;

  // manipulators
  static void defaultParent(const char* pathname);

  void scaleRect(double factor);
  void setPixelsPerUnit(double s);
  void setFixedRectLTRB(double L, double T, double R, double B);
  void setMinRectLTRB(double L, double T, double R, double B);
  void setUnitAngle(double deg);
  void setViewingDistIn(double in);
  void setPerspective(double fovy, double zNear, double zFar);

  /// Overridden from Tcl::TkWidget.
  virtual void reshapeCallback();

private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  TogletSizer* itsSizer;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
