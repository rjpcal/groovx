///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Mon Sep 16 19:40:32 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TOGL_H_DEFINED)
#include "togl/togl.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

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

  virtual Gfx::Canvas& getCanvas();

  // manipulators
  static void defaultParent(const char* pathname);

  void destroyWidget();
  void scaleRect(double factor);
  void setPixelsPerUnit(double s);
  void setFixedRectLTRB(double L, double T, double R, double B);
  void setMinRectLTRB(double L, double T, double R, double B);
  void setUnitAngle(double deg);
  void setViewingDistIn(double in);
  void setPerspective(double fovy, double zNear, double zFar);

  // widget functions
  virtual void bind(const char* event_sequence, const char* script);
  virtual void swapBuffers();
  virtual void takeFocus();

  // actions
  void writeEpsFile(const char* filename);

  virtual void displayCallback();
  virtual void reshapeCallback();

private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  TogletSizer* itsSizer;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
