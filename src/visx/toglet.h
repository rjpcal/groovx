///////////////////////////////////////////////////////////////////////
//
// toglconfig.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Sat Dec  2 09:49:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

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

class ToglConfig_Impl;

#ifndef _XLIB_H_
struct _XDisplay;
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

///////////////////////////////////////////////////////////////////////
//
// ToglConfig class definition
//
///////////////////////////////////////////////////////////////////////

class ToglConfig : public GWT::Widget {
public:
  // types
  struct Color {
	 Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
		pixel(p), red(r), green(g), blue(b) {}
	 unsigned int pixel;
	 double red, green, blue;
  };

  ToglConfig(Tcl_Interp* interp, const char* pathname,
				 int config_argc, char** config_argv,
				 double dist, double unit_angle);
  virtual ~ToglConfig();

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
	 virtual void onDestroy(ToglConfig* config) = 0;
  };

  void setDestroyCallback(DestroyCallback* callback);

  // widget functions
  virtual void bind(const char* event_sequence, const char* script);
  virtual void swapBuffers();
  virtual void takeFocus();

  // actions
  void loadFont(const char* fontname);
  void loadFonti(int fontnumber);
  void writeEpsFile(const char* filename);

private:
  ToglConfig(const ToglConfig&); // no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  friend class ToglConfig_Impl;

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

static const char vcid_toglconfig_h[] = "$Header$";
#endif // !TOGLCONFIG_H_DEFINED
