///////////////////////////////////////////////////////////////////////
//
// toglconfig.cc
// Rob Peters
// created: Wed Feb 24 10:18:17 1999
// written: Wed Sep 15 19:05:16 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_CC_DEFINED
#define TOGLCONFIG_CC_DEFINED

#include "toglconfig.h"

#include <GL/gl.h>
#include <tk.h>
#include <togl.h>
#include <cmath>
#include <limits>
#include <string>
#include <strstream.h>

#include "error.h"
#include "gfxattribs.h"
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
	 
	 TclEvalCmd cmd(buf);
	 
	 if ( cmd.invoke(Togl_Interp(togl)) != TCL_OK ) { throw ToglError(); }
  }
}

///////////////////////////////////////////////////////////////////////
//
// ToglConfig member definitions
//
///////////////////////////////////////////////////////////////////////

ToglConfig::ToglConfig(Togl* togl, double dist, double unit_angle) :
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

  GfxAttribs::setFlagsIf(GfxAttribs::RGBA_FLAG,
								 getIntParam("rgba"));
  GfxAttribs::setFlagsIf(GfxAttribs::PRIVATE_CMAP_FLAG,
								 getIntParam("privatecmap"));
  GfxAttribs::setFlagsIf(GfxAttribs::DOUBLE_BUFFER_FLAG,
								 getIntParam("double"));

  if (GfxAttribs::isTrue(GfxAttribs::RGBA_FLAG)) {
    glColor3f(1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }
  else { // not using rgba
    if (GfxAttribs::isTrue(GfxAttribs::PRIVATE_CMAP_FLAG)) {
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

  TclEvalCmd cmd(cmd_str);

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
  color.red   = double(col.red)   / numeric_limits<unsigned short>::max();
  color.green = double(col.green) / numeric_limits<unsigned short>::max();
  color.blue  = double(col.blue)  / numeric_limits<unsigned short>::max();
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

//////////////////
// manipulators //
//////////////////

void ToglConfig::destroyWidget() {
DOTRACE("ToglConfig::destroyWidget");
DebugPrintNL("ToglConfig::destroyWidget");
  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(Togl_Interp(itsWidget)) ) {
	 string destroy_cmd_str = string("destroy ") + pathname(itsWidget);
	 TclEvalCmd destroy_cmd(destroy_cmd_str);
	 if ( destroy_cmd.invoke(Togl_Interp(itsWidget)) != TCL_OK ) {
		Tcl_BackgroundError(Togl_Interp(itsWidget));
	 }
  }
}

void ToglConfig::scaleRect(double factor) {
DOTRACE("ToglConfig::scaleRect");
  try { if (factor <= 0.0) { throw ToglError("invalid scaling factor"); } }
  catch (ToglError&) { throw; }

  itsMinRect.widen(factor);
  itsMinRect.heighten(factor);

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
  itsFixedScale = int(screen_unit_dist * GfxAttribs::getScreenPpi());

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
  TclEvalCmd cmd(cmd_str);
  int result = cmd.invoke(Togl_Interp(itsWidget));
  try { if (result != TCL_OK) throw ToglError(); }
  catch (ToglError&) { throw; }
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
  GLuint newListBase =
	 Togl_LoadBitmapFont(itsWidget, reinterpret_cast<char*>(fontnumber));

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
      therect.widen(1/ratio_of_aspects); // so use some extra width
    }
    else {                        // the available space is too tall...
      therect.heighten(ratio_of_aspects); // and use some extra height
    }
    
    glOrtho(therect.l, therect.r, therect.b, therect.t, -1.0, 1.0);
    
	 DebugEval(itsMinRect.l);
	 DebugEval(itsMinRect.r);
	 DebugEval(itsMinRect.b);
	 DebugEvalNL(itsMinRect.t);
    DebugEval(itsMinRect.aspect());
    DebugEvalNL(getAspect());
#ifdef LOCAL_DEBUG
	 cerr << "glViewport(0, 0, " << Togl_Width(itsWidget) << ", "
			<< Togl_Height(itsWidget) << ")" << endl;
    cerr << "glOrtho(l=" << therect.l << ", r=" << therect.r
         << ", b=" << therect.b << ", t=" << therect.t << ", -1.0, 1.0)" << endl;
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
  TclEvalCmd cmd(string("focus -force ") + pathname(itsWidget));
  try {
	 if ( cmd.invoke(Togl_Interp(itsWidget)) != TCL_OK ) { throw ToglError(); }
  }
  catch (ToglError&) { throw; }
}

void ToglConfig::writeEpsFile(const char* filename) {
DOTRACE("ToglConfig::writeEpsFile");

  // save the old color indices for foreground and background
  GLint oldclear, oldindex;
  glGetIntegerv(GL_INDEX_CLEAR_VALUE, &oldclear);
  glGetIntegerv(GL_CURRENT_INDEX, &oldindex);

  // set color indices to maximum values for the purposes of the EPS rendering
  glClearIndex(255);
  glIndexi(0);

  // get a clear buffer
  glClear(GL_COLOR_BUFFER_BIT);
  swapBuffers();

  // do the EPS dump
  const int rgbFlag = 0;
  Togl_DumpToEpsFile(itsWidget, filename, rgbFlag, dummyEpsCallback);

  // restore the old color indices
  glClearIndex(oldclear);
  glIndexi(oldindex);

  // redisplay original image
  display();
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
  config->display();
}

void dummyEpsCallback(const Togl* togl) {
DOTRACE("dummyEpsCallback");
  ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
  DebugEvalNL((void*) config);
  config->display();
}
}

static const char vcid_toglconfig_cc[] = "$Header$";
#endif // !TOGLCONFIG_CC_DEFINED
