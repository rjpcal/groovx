///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Sep 25 19:03:37 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#include "togl/togl.h"

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
