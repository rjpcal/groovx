///////////////////////////////////////////////////////////////////////
//
// toglconfig.h
// Rob Peters 
// created: Jan-99
// written: Fri Dec  3 15:25:14 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_H_DEFINED
#define TOGLCONFIG_H_DEFINED

#ifndef RECT_H_DEFINED
#include "rect.h"
#endif

#ifndef WIDGET_H_DEFINED
#include "widget.h"
#endif

struct Togl;
struct Tcl_Interp;
struct Tcl_Obj;

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

class ToglConfig : public Widget {
public:
  // types
  struct Color {
	 Color(unsigned int p=0, double r=0.0, double g=0.0, double b=0.0):
		pixel(p), red(r), green(g), blue(b) {}
	 unsigned int pixel;
	 double red, green, blue;
  };

  ToglConfig(Togl* togl, double dist, double unit_angle);
  virtual ~ToglConfig();

  // accessors
  double getAspect() const { return double(getWidth())/getHeight(); }
  double getFixedScale() const;
  Rect<double> getMinRect() const;
  int getHeight() const;
  unsigned int getFontListBase() const { return itsFontListBase; }
  Tcl_Interp* getInterp() const;
  int getIntParam(const char* param) const;
  Tcl_Obj* getParamValue(const char* param) const;
  Togl* getTogl() const;
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

  // widget functions
  virtual void bind(const char* event_sequence, const char* script);
  virtual void swapBuffers();
  virtual void takeFocus();

  // actions
  void loadFont(const char* fontname);
  void loadFonti(int fontnumber);
  void writeEpsFile(const char* filename);

  virtual void reconfigure();
  virtual void display() = 0;
  virtual void clearscreen() = 0;
  virtual void refresh() = 0;
  virtual void undraw() = 0;

private:
  ToglConfig(const ToglConfig&); // no copy constructor
  ToglConfig& operator=(const ToglConfig&); // no assignment operator

  Togl* itsWidget;
  double itsViewingDistance;     // inches
  bool itsFixedScaleFlag;
  double itsFixedScale;
  Rect<double> itsMinRect;
  unsigned int itsFontListBase;
};

static const char vcid_toglconfig_h[] = "$Header$";
#endif // !TOGLCONFIG_H_DEFINED
