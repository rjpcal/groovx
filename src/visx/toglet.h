///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Tue Apr  2 16:29:08 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TKWIDGET_H_DEFINED)
#include "visx/tkwidget.h"
#endif

struct Togl;
struct Tcl_Interp;

class Toglet_Impl;

///////////////////////////////////////////////////////////////////////
//
// Toglet class definition
//
///////////////////////////////////////////////////////////////////////

class Toglet : public Tcl::TkWidget
{
protected:
  Toglet(Tcl_Interp* interp,
         int config_argc=0, char** config_argv=0,
         bool pack=true);

public:
  // types
  struct Color
  {
    Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
      pixel(p), red(r), green(g), blue(b) {}
    unsigned int pixel;
    double red, green, blue;
  };

  static Toglet* make(Tcl_Interp* interp) { return new Toglet(interp); }

  virtual ~Toglet();

  virtual bool isNotShareable() const { return true; }

  // accessors
  int getHeight() const;
  Tcl_Interp* getInterp() const;
  int getWidth() const;
  const char* pathname() const;
  Color queryColor(unsigned int color_index) const;
  void queryColor(unsigned int color_index, Color& color) const;
  bool usingFixedScale() const;
  double pixelsPerInch() const;

  virtual Gfx::Canvas& getCanvas();

  // manipulators
  static void defaultParent(const char* pathname);

  void destroyWidget();
  void scaleRect(double factor);
  void setColor(const Color& color);
  void setPixelsPerUnit(double s);
  void setMinRectLTRB(double L, double T, double R, double B);
  void setHeight(int val);
  void setWidth(int val);
  void setUnitAngle(double deg);
  void setViewingDistIn(double in);

  // widget functions
  virtual void bind(const char* event_sequence, const char* script);
  virtual void swapBuffers();
  virtual void takeFocus();

  void makeCurrent();

  // actions
  void loadDefaultFont();
  void loadFont(const char* fontname);
  void loadFonti(int fontnumber);
  void writeEpsFile(const char* filename);

private:
  Toglet(const Toglet&); // no copy constructor
  Toglet& operator=(const Toglet&); // no assignment operator

  unsigned int getFontListBase() const { return itsFontListBase; }
  void reconfigure();

  friend class Toglet_Impl;

  class Sizer;
  friend class Sizer;

  Togl* const itsTogl;
  scoped_ptr<Gfx::Canvas> itsCanvas;
  scoped_ptr<Sizer> itsSizer;
  unsigned int itsFontListBase;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
