///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Sun Aug 26 08:53:51 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RECT_H_DEFINED)
#include "gfx/rect.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TKWIDGET_H_DEFINED)
#include "tcl/tkwidget.h"
#endif

struct Togl;
struct Tcl_Interp;
struct Tcl_Obj;

class Toglet_Impl;

#ifndef _XLIB_H_
struct _XDisplay;
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

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
             bool pack=true,
             double dist=30.0, double unit_angle=2.05);

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

  virtual bool isVolatile() const { return true; }

  // accessors
  double getAspect() const { return double(getWidth())/getHeight(); }
  double getFixedScale() const;
  Gfx::Rect<double> getMinRect() const;
  int getHeight() const;
  unsigned int getFontListBase() const { return itsFontListBase; }
  Tcl_Interp* getInterp() const;
  int getWidth() const;
  const char* pathname() const;
  Color queryColor(unsigned int color_index) const
  {
    Color col;
    queryColor(color_index, col);
    return col;
  }
  void queryColor(unsigned int color_index, Color& color) const;
  bool usingFixedScale() const;

  Display* getX11Display() const;
  int getX11ScreenNumber() const;
  Window getX11Window() const;

  virtual Gfx::Canvas& getCanvas();

  // manipulators
  static void defaultParent(const char* pathname);

  void destroyWidget();
  void scaleRect(double factor);
  void setColor(const Color& color);
  void setFixedScale(double s);
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

  friend class Toglet_Impl;

  void reconfigure();

  Togl* const itsTogl;
  scoped_ptr<Gfx::Canvas> itsCanvas;
  double itsViewingDistance;     // inches
  bool itsFixedScaleFlag;
  double itsFixedScale;
  Gfx::Rect<double> itsMinRect;
  unsigned int itsFontListBase;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
