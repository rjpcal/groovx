///////////////////////////////////////////////////////////////////////
//
// toglconfig.cc
// Rob Peters
// created: Wed Feb 24 10:18:17 1999
// written: Sun Jan 16 23:13:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_CC_DEFINED
#define TOGLCONFIG_CC_DEFINED

#include "toglconfig.h"

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <tk.h>
#include <togl.h>
#include <cmath>
#include <string>
#include <strstream.h>

#ifndef GCC_COMPILER
#  include <limits>
#else
#  include <climits>
#  define NO_CPP_LIMITS
#endif

#include "error.h"
#include "glcanvas.h"
#include "tclevalcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ToglError class definition
//
///////////////////////////////////////////////////////////////////////

class ToglError : public ErrorWithMsg {
public:
  ToglError(const string& msg="") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  void dummyReshapeCallback(Togl* togl);
  void dummyDisplayCallback(Togl* togl);
  void dummyEpsCallback(const Togl* togl);

  const char* pathname(Togl* togl) {
	 return Tk_PathName(reinterpret_cast<Tk_FakeWin*>(Togl_TkWin(togl)));
  }

  void setIntParam(Togl* togl, const char* param, int val) {
	 const int BUF_SIZE = 256;
	 char buf[BUF_SIZE];
	 ostrstream ost(buf, BUF_SIZE);
	 ost << pathname(togl);
	 ost << " configure -" << param << ' ' << val << '\0';
	 
	 Tcl::TclEvalCmd cmd(buf, Tcl::TclEvalCmd::THROW_EXCEPTION);
	 cmd.invoke(Togl_Interp(togl));
  }
}

///////////////////////////////////////////////////////////////////////
//
// ToglConfig member definitions
//
///////////////////////////////////////////////////////////////////////

ToglConfig::ToglConfig(Togl* togl, double dist, double unit_angle) :
  itsCanvas(new GLCanvas),
  itsWidget(togl),
  itsViewingDistance(dist), 
  itsFixedScaleFlag(true),
  itsFontListBase(0)
{
DOTRACE("ToglConfig::ToglConfig"); 
  DebugEvalNL((void*) this);

  Togl_SetClientData(itsWidget, static_cast<ClientData>(this));
  Togl_SetReshapeFunc(itsWidget, dummyReshapeCallback);
  Togl_SetDisplayFunc(itsWidget, dummyDisplayCallback);
  
  setUnitAngle(unit_angle);

  itsUsingRgba = (getIntParam("rgba") != 0);
  itsHasPrivateCmap = (getIntParam("privatecmap") != 0);
  itsIsDoubleBuffered = (getIntParam("double") != 0);

  if ( itsUsingRgba ) {
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }
  else { // not using rgba
    if ( itsHasPrivateCmap ) {
      glClearIndex(0);
      glIndexi(1);
    }
    else {
      glClearIndex(Togl_AllocColor(togl, 0.0, 0.0, 0.0));
      glIndexi(Togl_AllocColor(togl, 1.0, 1.0, 1.0));
    }
  }
}

ToglConfig::~ToglConfig() {
DOTRACE("ToglConfig::~ToglConfig");
DebugPrintNL("ToglConfig::~ToglConfig");
  Togl_SetClientData(itsWidget, static_cast<ClientData>(0));
  Togl_SetReshapeFunc(itsWidget, 0);
  Togl_SetDisplayFunc(itsWidget, 0);
  itsWidget = 0;
}

///////////////
// accessors //
///////////////

double ToglConfig::getFixedScale() const {
DOTRACE("ToglConfig::getFixedScale"); 
  return itsFixedScale; 
}

Rect<double> ToglConfig::getMinRect() const { 
DOTRACE("ToglConfig::getMinRect");
  return itsMinRect;
}

Tcl_Obj* ToglConfig::getParamValue(const char* param) const {
DOTRACE("ToglConfig::getParamValue");
  // Build a Tcl command string to fetch the value of param
  // command will look like:
  //     pathname configure -param
  string cmd_str = pathname(itsWidget);
  cmd_str += " configure -";
  cmd_str += param;

  Tcl::TclEvalCmd cmd(cmd_str, Tcl::TclEvalCmd::BACKGROUND_ERROR);

  // Execute the command
  Tcl_Interp* interp = Togl_Interp(itsWidget);
  if ( cmd.invoke(interp) != TCL_OK ) return NULL;

  // The resulting list will look like:
  //          -rgba rgba Rgba true 0
  // (i.e.)   -cmd_name all_lower with_upper default_val current_val
  // So...
  // get the current value, the fifth object from the resulting list 
  Tcl_Obj* list = Tcl_GetObjResult(interp);
  Tcl_Obj* obj = NULL;
  if ( Tcl_ListObjIndex(interp, list, 4, &obj) != TCL_OK ) return NULL;
  Tcl_IncrRefCount(obj);

  // Restore the previous result value
  Tcl_ResetResult(interp);
  return obj;
}

Tcl_Interp* ToglConfig::getInterp() const {
DOTRACE("ToglConfig::getInterp");
  return Togl_Interp(itsWidget); 
}

int ToglConfig::getIntParam(const char* param) const {
DOTRACE("ToglConfig::getIntParam");
  Tcl_Obj* obj = getParamValue(param);
  int value;
  if ( Tcl_GetIntFromObj(Togl_Interp(itsWidget), obj, &value) != TCL_OK ) {
	 Tcl_DecrRefCount(obj);
	 throw ToglError();
  }
  Tcl_DecrRefCount(obj);
  return value;
}

Togl* ToglConfig::getTogl() const {
DOTRACE("ToglConfig::getTogl");
  return itsWidget; 
}

int ToglConfig::getHeight() const {
DOTRACE("ToglConfig::getHeight");
  return Togl_Height(itsWidget);
}

int ToglConfig::getWidth() const {
DOTRACE("ToglConfig::getWidth");
  return Togl_Width(itsWidget);
}

void ToglConfig::queryColor(unsigned int color_index, Color& color) const {
DOTRACE("ToglConfig::queryColor");
  Tk_Window tkwin = Togl_TkWin(itsWidget);
  Display* display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
  Colormap cmap = Togl_Colormap(itsWidget);
  XColor col;
  
  col.pixel = color_index;
  XQueryColor(display, cmap, &col);

  color.pixel = (unsigned int)col.pixel;
#ifndef NO_CPP_LIMITS
  color.red   = double(col.red)   / numeric_limits<unsigned short>::max();
  color.green = double(col.green) / numeric_limits<unsigned short>::max();
  color.blue  = double(col.blue)  / numeric_limits<unsigned short>::max();
#else
  color.red   = double(col.red)   / USHRT_MAX;
  color.green = double(col.green) / USHRT_MAX;
  color.blue  = double(col.blue)  / USHRT_MAX;
#endif
}

bool ToglConfig::usingFixedScale() const {
DOTRACE("ToglConfig::usingFixedScale"); 
  return itsFixedScaleFlag; 
}

Display* ToglConfig::getX11Display() const {
DOTRACE("getX11Display");
  return Togl_Display(itsWidget);
}

int ToglConfig::getX11ScreenNumber() const {
DOTRACE("getX11ScreenNumber");
  return Togl_ScreenNumber(itsWidget); 
}

Window ToglConfig::getX11Window() const {
DOTRACE("getX11Window");
  return Togl_Window(itsWidget);
}

Canvas* ToglConfig::getCanvas() {
DOTRACE("ToglConfig::getCanvas");
  return itsCanvas.get();
}

//////////////////
// manipulators //
//////////////////

void ToglConfig::destroyWidget() {
DOTRACE("ToglConfig::destroyWidget");
DebugPrintNL("ToglConfig::destroyWidget");
  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(Togl_Interp(itsWidget)) ) {
	 string destroy_cmd_str = string("destroy ") + pathname(itsWidget);
	 Tcl::TclEvalCmd destroy_cmd(destroy_cmd_str, Tcl::TclEvalCmd::BACKGROUND_ERROR);
	 destroy_cmd.invoke(Togl_Interp(itsWidget));
  }
}

void ToglConfig::scaleRect(double factor) {
DOTRACE("ToglConfig::scaleRect");
  try { if (factor <= 0.0) { throw ToglError("invalid scaling factor"); } }
  catch (ToglError&) { throw; }

  itsMinRect.widenByFactor(factor);
  itsMinRect.heightenByFactor(factor);

  reconfigure();
}

void ToglConfig::setColor(const Color& color) {
DOTRACE("ToglConfig::setColor");
  static const char* const bad_val_msg = "RGB values must be in [0.0, 1.0]";
  static const char* const bad_index_msg = "color index must be in [0, 255]";

  try {
	 if (color.pixel < 0   || color.pixel > 255) { throw ToglError(bad_index_msg); }
	 if (color.red   < 0.0 || color.red   > 1.0) { throw ToglError(bad_val_msg); }
	 if (color.green < 0.0 || color.green > 1.0) { throw ToglError(bad_val_msg); }
	 if (color.blue  < 0.0 || color.blue  > 1.0) { throw ToglError(bad_val_msg); }
  }
  catch (ToglError&) { throw; }

  Togl_SetColor(itsWidget, color.pixel, color.red, color.green, color.blue);
}

void ToglConfig::setFixedScale(double s) {
DOTRACE("ToglConfig::setFixedScale"); 
  try { if (s <= 0.0) { throw ToglError("invalid scaling factor"); } }
  catch (ToglError&) { throw; }

  itsFixedScaleFlag = true;
  itsFixedScale = s;

  reconfigure();
}

void ToglConfig::setUnitAngle(double deg) {
DOTRACE("ToglConfig::setUnitAngle");
  try { if (deg <= 0.0) { throw ToglError("unit angle must be positive"); } }
  catch (ToglError&) { throw; }

  static const double deg_to_rad = 3.141593/180.0;
  itsFixedScaleFlag = true;
  // tan(deg) == screen_unit_dist/viewing_dist;
  // screen_unit_dist == 1.0 * itsFixedScale / screepPpi;
  double screen_unit_dist = tan(deg*deg_to_rad) * itsViewingDistance;

  Screen* scr = Tk_Screen(reinterpret_cast<Tk_FakeWin*>(Togl_TkWin(itsWidget)));
  int screen_pixel_width = XWidthOfScreen(scr);
  int screen_mm_width = XWidthMMOfScreen(scr);
  double screen_inch_width = screen_mm_width / 25.4;

  double screen_ppi = screen_pixel_width / screen_inch_width; 
  DebugEvalNL(screen_ppi);
  itsFixedScale = int(screen_unit_dist * screen_ppi);

  reconfigure();
}

void ToglConfig::setViewingDistIn(double in) {
DOTRACE("ToglConfig::setViewingDistIn"); 
  try {
	 if (in <= 0.0) 
		{ throw ToglError("viewing distance must be positive (duh)"); }
  }
  catch (ToglError&) { throw; }


  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  double factor = in / itsViewingDistance;
  itsFixedScale *= factor;
  itsViewingDistance = in; 

  reconfigure();
}

void ToglConfig::setMinRectLTRB(double L, double T, double R, double B) {
DOTRACE("ToglConfig::setMinRectLTRB"); 
  itsFixedScaleFlag = false;
  itsMinRect.setRectLTRB(L,T,R,B);

  reconfigure();
}

void ToglConfig::setHeight(int val) {
DOTRACE("ToglConfig::setHeight");
  setIntParam(itsWidget, "height", val);
  reconfigure();
}

void ToglConfig::setWidth(int val) {
DOTRACE("ToglConfig::setWidth");
  setIntParam(itsWidget, "width", val);
  reconfigure();
}

/////////////
// actions //
/////////////

void ToglConfig::bind(const char* event_sequence, const char* script) {
DOTRACE("ToglConfig::bind");
  string cmd_str = string("bind ") + pathname(itsWidget) + " "
	 + event_sequence + " " + script;
  Tcl::TclEvalCmd cmd(cmd_str, Tcl::TclEvalCmd::THROW_EXCEPTION);
  try { cmd.invoke(Togl_Interp(itsWidget)); }
  catch (Tcl::TclError&) { throw; }
}

void ToglConfig::loadFont(const char* fontname) {
DOTRACE("ToglConfig::loadFont");

  GLuint newListBase = Togl_LoadBitmapFont(itsWidget, fontname);

  try {
	 // Check if font loading succeeded...
	 if (newListBase == 0) {
		DebugEval(fontname);
		throw ToglError(string("unable to load font ") + fontname);
	 }
  }
  catch (ToglError&) { throw; }

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
	 Togl_UnloadBitmapFont(itsWidget, itsFontListBase);
  }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

void ToglConfig::loadFonti(int fontnumber) {
DOTRACE("ToglConfig::loadFonti");

  // This function core dumps on irix6 using g++, so we throw an error
  // for now.
#ifndef IRIX6
  GLuint newListBase =
	 Togl_LoadBitmapFont(itsWidget, reinterpret_cast<char*>(fontnumber));
#else
  GLuint newListBase = 0;		  // this will force an exception to be thrown
#endif
  

  // Check if font loading succeeded...
  try {
	 if (newListBase == 0) {
		throw ToglError(string("unable to load font"));
	 }
  }
  catch (ToglError&) { throw; }

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
	 Togl_UnloadBitmapFont(itsWidget, itsFontListBase);
  }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

void ToglConfig::reconfigure() {
DOTRACE("ToglConfig::reconfigure");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, getWidth(), getHeight());

  if (usingFixedScale()) {
    double l = -1 * (getWidth()  / 2.0) / itsFixedScale;
    double r =      (getWidth()  / 2.0) / itsFixedScale;
    double b = -1 * (getHeight() / 2.0) / itsFixedScale;
    double t =      (getHeight() / 2.0) / itsFixedScale;
    glOrtho(l, r, b, t, -1.0, 1.0);
  }

  else { // not usingFixedScale (i.e. minRect instead)

    Rect<double> therect(itsMinRect);        // the actual Rect that we'll build
  
    // the desired conditions are as follows:
    //    (1) therect contains itsMinRect
    //    (2) therect.aspect() == getAspect()
    //    (3) therect is the smallest rectangle that meets (1) and (2)
    
    double ratio_of_aspects = itsMinRect.aspect() / getAspect();
    
    if ( ratio_of_aspects < 1 ) { // the available space is too wide...
      therect.widenByFactor(1/ratio_of_aspects); // so use some extra width
    }
    else {                        // the available space is too tall...
      therect.heightenByFactor(ratio_of_aspects); // and use some extra height
    }
    
    glOrtho(therect.left(), therect.right(),
				therect.bottom(), therect.top(), -1.0, 1.0);
    
	 DebugEval(itsMinRect.left());
	 DebugEval(itsMinRect.right());
	 DebugEval(itsMinRect.bottom());
	 DebugEvalNL(itsMinRect.top());
    DebugEval(itsMinRect.aspect());
    DebugEvalNL(getAspect());
#ifdef LOCAL_DEBUG
	 cerr << "glViewport(0, 0, " << Togl_Width(itsWidget) << ", "
			<< Togl_Height(itsWidget) << ")" << endl;
    cerr << "glOrtho(l=" << therect.left() << ", r=" << therect.right()
         << ", b=" << therect.bottom() << ", t=" << therect.top() << ", -1.0, 1.0)" << endl;
#endif
  } // end not usingFixedScale

  Togl_PostRedisplay(itsWidget);
}

void ToglConfig::swapBuffers() {
DOTRACE("ToglConfig::swapBuffers");
  Togl_SwapBuffers(itsWidget);
}

void ToglConfig::takeFocus() {
DOTRACE("ToglConfig::takeFocus");
  Tcl::TclEvalCmd cmd(string("focus -force ") + pathname(itsWidget),
					  Tcl::TclEvalCmd::THROW_EXCEPTION);
  try { cmd.invoke(Togl_Interp(itsWidget)); }
  catch (Tcl::TclError&) { throw; }
}

void ToglConfig::writeEpsFile(const char* filename) {
DOTRACE("ToglConfig::writeEpsFile");

  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 // Set fore/background colors to extremes for the purposes of EPS
	 // rendering
	 if ( itsUsingRgba ) {
		glColor4d(0.0, 0.0, 0.0, 1.0);
		glClearColor(1.0, 1.0, 1.0, 1.0);
	 }
	 else {
		glIndexi(0);
		glClearIndex(255);
	 }

	 // get a clear buffer
	 glClear(GL_COLOR_BUFFER_BIT);
	 swapBuffers();
	 
	 // do the EPS dump
	 const int rgbFlag = 0;
	 Togl_DumpToEpsFile(itsWidget, filename, rgbFlag, dummyEpsCallback);
  }
  glPopAttrib();

  // redisplay original image
  refresh();
}

///////////////////////////////////////////////////////////////////////
//
// Dummy callbacks
//
///////////////////////////////////////////////////////////////////////

namespace {
void dummyReshapeCallback(Togl* togl) {
DOTRACE("dummyReshapeCallback");
  ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
  DebugEvalNL((void*) config);
  config->reconfigure();
}

void dummyDisplayCallback(Togl* togl) {
DOTRACE("dummyDisplayCallback");
  ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
  DebugEvalNL((void*) config);
  config->refresh();
}

void dummyEpsCallback(const Togl* togl) {
DOTRACE("dummyEpsCallback");
  ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
  DebugEvalNL((void*) config);
  config->refresh();
}
}

static const char vcid_toglconfig_cc[] = "$Header$";
#endif // !TOGLCONFIG_CC_DEFINED
