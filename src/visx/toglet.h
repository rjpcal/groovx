///////////////////////////////////////////////////////////////////////
//
// toglet.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Wed Jun  6 20:23:41 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_H_DEFINED
#define TOGLET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RECT_H_DEFINED)
#include "rect.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WIDGET_H_DEFINED)
#include "gwt/widget.h"
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

class Toglet : public virtual Util::Object, public GWT::Widget {
protected:
  Toglet(Tcl_Interp* interp,
				 int config_argc=0, char** config_argv=0,
				 bool pack=true,
				 double dist=30.0, double unit_angle=2.05);

public:
  // types
  struct Color {
	 Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
		pixel(p), red(r), green(g), blue(b) {}
	 unsigned int pixel;
	 double red, green, blue;
  };

  static Toglet* make(Tcl_Interp* interp) { return new Toglet(interp); }

  virtual ~Toglet();

  void onWindowClose();

  // accessors
  double getAspect() const { return double(getWidth())/getHeight(); }
  double getFixedScale() const;
  Rect<double> getMinRect() const;
  int getHeight() const;
  unsigned int getFontListBase() const { return itsFontListBase; }
  Tcl_Interp* getInterp() const;
  int getWidth() const;
  Color queryColor(unsigned int color_index) const {
	 Color col;
	 queryColor(color_index, col);
	 return col;
  }
  void queryColor(unsigned int color_index, Color& color) const;
  bool usingFixedScale() const;

  Display* getX11Display() const;
  int getX11ScreenNumber() const;
  Window getX11Window() const;

  virtual GWT::Canvas* getCanvas();

  // manipulators
  void destroyWidget();
  void scaleRect(double factor);
  void setColor(const Color& color);
  void setFixedScale(double s);
  void setMinRectLTRB(double L, double T, double R, double B);
  void setHeight(int val);
  void setWidth(int val);
  void setUnitAngle(double deg);
  void setViewingDistIn(double in);

  class DestroyCallback {
  public:
	 virtual ~DestroyCallback();
	 virtual void onDestroy(Toglet* config) = 0;
  };

  void setDestroyCallback(DestroyCallback* callback);

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

  scoped_ptr<GWT::Canvas> itsCanvas;

  Togl* itsTogl;
  double itsViewingDistance;     // inches
  bool itsFixedScaleFlag;
  double itsFixedScale;
  Rect<double> itsMinRect;
  unsigned int itsFontListBase;

  scoped_ptr<DestroyCallback> itsDestroyCallback;
};

static const char vcid_toglet_h[] = "$Header$";
#endif // !TOGLET_H_DEFINED
