///////////////////////////////////////////////////////////////////////
//
// toglconfig.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 24 10:18:17 1999
// written: Sat Dec  2 09:50:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLCONFIG_CC_DEFINED
#define TOGLCONFIG_CC_DEFINED

#include "toglconfig.h"

#include "glcanvas.h"
#include "xbmaprenderer.h"

#include "tcl/tclevalcmd.h"

#include "togl/togl.h"

#include "util/error.h"
#include "util/strings.h"

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <tk.h>
#include <cmath>
#include <strstream.h>

#ifndef GCC_COMPILER
#  include <limits>
#else
#  include <climits>
#  define NO_CPP_LIMITS
#endif

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ToglError class definition
//
///////////////////////////////////////////////////////////////////////

class ToglError : public ErrorWithMsg {
public:
  ToglError(const char* msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

class ToglConfig_Impl {
  static void dummyReshapeCallback(Togl* togl);
  static void dummyDisplayCallback(Togl* togl);
  static void dummyEpsCallback(const Togl* togl);
};

namespace {

  void toglDestroyCallback(Togl* togl) {
  DOTRACE("toglDestroyCallback");
	 DebugEvalNL((void*)togl);
	 ToglConfig* config = static_cast<ToglConfig*>(togl->getClientData());
	 DebugEvalNL((void*)config);
	 delete config;
  }

  void dummyEventProc(ClientData clientData, XEvent* eventPtr) {
  DOTRACE("dummyEventProc");
    DebugEvalNL(clientData);
  }

  const char* pathname(Togl* togl) {
	 return Tk_PathName(reinterpret_cast<Tk_FakeWin*>(togl->tkWin()));
  }

  void setIntParam(Togl* togl, const char* param, int val) {
	 const int BUF_SIZE = 256;
	 char buf[BUF_SIZE];
	 ostrstream ost(buf, BUF_SIZE);
	 ost << pathname(togl);
	 ost << " configure -" << param << ' ' << val << '\0';
	 
	 Tcl::TclEvalCmd cmd(buf, Tcl::TclEvalCmd::THROW_EXCEPTION);
	 cmd.invoke(togl->interp());
  }
}

///////////////////////////////////////////////////////////////////////
//
// ToglConfig member definitions
//
///////////////////////////////////////////////////////////////////////

ToglConfig::ToglConfig(Tcl_Interp* interp, const char* pathname,
							  int config_argc, char** config_argv,
							  double dist, double unit_angle) :
  GWT::Widget(),
  itsCanvas(new GLCanvas),
  itsTogl(new Togl(interp, pathname, config_argc, config_argv)),
  itsViewingDistance(dist), 
  itsFixedScaleFlag(true),
  itsFixedScale(1.0),
  itsMinRect(),
  itsFontListBase(0),
  itsDestroyCallback(0)
{
DOTRACE("ToglConfig::ToglConfig"); 
  DebugEvalNL((void*) this);

  itsTogl->setClientData(static_cast<ClientData>(this));

  itsTogl->setReshapeFunc(ToglConfig_Impl::dummyReshapeCallback);
  itsTogl->setDisplayFunc(ToglConfig_Impl::dummyDisplayCallback);
  itsTogl->setDestroyFunc(toglDestroyCallback);
  
  setUnitAngle(unit_angle);

  if ( itsTogl->usesRgba() ) {
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }
  else { // not using rgba
    if ( itsTogl->hasPrivateCmap() ) {
      glClearIndex(0);
      glIndexi(1);
    }
    else {
      glClearIndex(itsTogl->allocColor(0.0, 0.0, 0.0));
      glIndexi(itsTogl->allocColor(1.0, 1.0, 1.0));
    }
  }

  Tk_Window tkwin = itsTogl->tkWin();
  Tk_CreateEventHandler(tkwin, ButtonPressMask, dummyEventProc,
								static_cast<void*>(this));

  XBmapRenderer::initClass(tkwin);
}

ToglConfig::~ToglConfig() {
DOTRACE("ToglConfig::~ToglConfig");
  if (itsDestroyCallback.get() != 0)
	 itsDestroyCallback->onDestroy(this);

  itsTogl->setClientData(static_cast<ClientData>(0));
  itsTogl->setReshapeFunc(0);
  itsTogl->setDisplayFunc(0);

  Tk_Window tkwin = itsTogl->tkWin();
  if (tkwin != 0) {
	 Tk_DeleteEventHandler(tkwin, ButtonPressMask, dummyEventProc,
								  static_cast<void*>(this));
  }

  itsTogl = 0;
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

Tcl_Interp* ToglConfig::getInterp() const {
DOTRACE("ToglConfig::getInterp");
  return itsTogl->interp();
}

int ToglConfig::getHeight() const {
DOTRACE("ToglConfig::getHeight");
  return itsTogl->height();
}

int ToglConfig::getWidth() const {
DOTRACE("ToglConfig::getWidth");
  return itsTogl->width();
}

void ToglConfig::queryColor(unsigned int color_index, Color& color) const {
DOTRACE("ToglConfig::queryColor");
  Tk_Window tkwin = itsTogl->tkWin();
  Display* display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
  Colormap cmap = itsTogl->colormap();
  XColor col;
  
  col.pixel = color_index;
  XQueryColor(display, cmap, &col);

  color.pixel = (unsigned int)col.pixel;
#ifndef NO_CPP_LIMITS
  color.red   = double(col.red)   / std::numeric_limits<unsigned short>::max();
  color.green = double(col.green) / std::numeric_limits<unsigned short>::max();
  color.blue  = double(col.blue)  / std::numeric_limits<unsigned short>::max();
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
  return itsTogl->display();
}

int ToglConfig::getX11ScreenNumber() const {
DOTRACE("getX11ScreenNumber");
  return itsTogl->screenNumber(); 
}

Window ToglConfig::getX11Window() const {
DOTRACE("getX11Window");
  return itsTogl->windowId();
}

GWT::Canvas* ToglConfig::getCanvas() {
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
  if ( !Tcl_InterpDeleted(itsTogl->interp()) ) {
	 dynamic_string destroy_cmd_str = "destroy ";
	 destroy_cmd_str += pathname(itsTogl);

	 Tcl::TclEvalCmd destroy_cmd(destroy_cmd_str.c_str(),
										  Tcl::TclEvalCmd::BACKGROUND_ERROR);
	 destroy_cmd.invoke(itsTogl->interp());
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
	 if (                     color.pixel > 255) { throw ToglError(bad_index_msg); }
	 if (color.red   < 0.0 || color.red   > 1.0) { throw ToglError(bad_val_msg); }
	 if (color.green < 0.0 || color.green > 1.0) { throw ToglError(bad_val_msg); }
	 if (color.blue  < 0.0 || color.blue  > 1.0) { throw ToglError(bad_val_msg); }
  }
  catch (ToglError&) { throw; }

  itsTogl->setColor(color.pixel, color.red, color.green, color.blue);
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

  Screen* scr = Tk_Screen(reinterpret_cast<Tk_FakeWin*>(itsTogl->tkWin()));
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
  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "height", val);
}

void ToglConfig::setWidth(int val) {
DOTRACE("ToglConfig::setWidth");
  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "width", val);
}

ToglConfig::DestroyCallback::~DestroyCallback() {}

void ToglConfig::setDestroyCallback(DestroyCallback* callback) {
DOTRACE("ToglConfig::setDestroyCallback");
  itsDestroyCallback.reset(callback); 
}

/////////////
// actions //
/////////////

void ToglConfig::bind(const char* event_sequence, const char* script) {
DOTRACE("ToglConfig::bind");
  dynamic_string cmd_str = "bind ";
  cmd_str += pathname(itsTogl); cmd_str += " ";
  cmd_str += event_sequence;      cmd_str += " ";
  cmd_str += script;

  Tcl::TclEvalCmd cmd(cmd_str.c_str(),
							 Tcl::TclEvalCmd::THROW_EXCEPTION);
  try { cmd.invoke(itsTogl->interp()); }
  catch (Tcl::TclError&) { throw; }
}

void ToglConfig::loadFont(const char* fontname) {
DOTRACE("ToglConfig::loadFont");

  GLuint newListBase = itsTogl->loadBitmapFont(fontname);

  try {
	 // Check if font loading succeeded...
	 if (newListBase == 0) {
		DebugEval(fontname);
		ToglError err("unable to load font ");
		err.appendMsg(fontname);
		throw err;
	 }
  }
  catch (ToglError&) { throw; }

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
	 itsTogl->unloadBitmapFont(itsFontListBase);
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
	 itsTogl->loadBitmapFont(reinterpret_cast<char*>(fontnumber));
#else
  GLuint newListBase = 0*fontnumber; // this will force an exception to be thrown
#endif
  

  // Check if font loading succeeded...
  try {
	 if (newListBase == 0) {
		throw ToglError("unable to load font");
	 }
  }
  catch (ToglError&) { throw; }

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
	 itsTogl->unloadBitmapFont(itsFontListBase);
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
	 cerr << "glViewport(0, 0, " << itsTogl->width() << ", "
			<< itsTogl->height() << ")" << endl;
    cerr << "glOrtho(l=" << therect.left() << ", r=" << therect.right()
         << ", b=" << therect.bottom() << ", t=" << therect.top() << ", -1.0, 1.0)" << endl;
#endif
  } // end not usingFixedScale

  itsTogl->postRedisplay();
}

void ToglConfig::swapBuffers() {
DOTRACE("ToglConfig::swapBuffers");
  itsTogl->swapBuffers();
}

void ToglConfig::takeFocus() {
DOTRACE("ToglConfig::takeFocus");
  dynamic_string cmd_str = "focus -force ";
  cmd_str += pathname(itsTogl);

  Tcl::TclEvalCmd cmd(cmd_str.c_str(),
							 Tcl::TclEvalCmd::THROW_EXCEPTION);
  try { cmd.invoke(itsTogl->interp()); }
  catch (Tcl::TclError&) { throw; }
}

void ToglConfig::writeEpsFile(const char* filename) {
DOTRACE("ToglConfig::writeEpsFile");

  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 // Set fore/background colors to extremes for the purposes of EPS
	 // rendering
	 if ( itsTogl->usesRgba() ) {
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
	 itsTogl->dumpToEpsFile(filename, rgbFlag,
									  ToglConfig_Impl::dummyEpsCallback);
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

void ToglConfig_Impl::dummyReshapeCallback(Togl* togl) {
DOTRACE("ToglConfig_Impl::dummyReshapeCallback");
  ToglConfig* config = static_cast<ToglConfig*>(togl->getClientData());
  DebugEvalNL((void*) config);

  try {
	 config->reconfigure();
  }
  catch (ErrorWithMsg& err) {
	 Tcl_AppendResult(togl->interp(), err.msg_cstr(), (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
  catch (...) {
	 Tcl_AppendResult(togl->interp(), "an error of unknown type occurred "
							"in dummyReshapeCallback", (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
}

void ToglConfig_Impl::dummyDisplayCallback(Togl* togl) {
DOTRACE("ToglConfig_Impl::dummyDisplayCallback");

  ToglConfig* config = static_cast<ToglConfig*>(togl->getClientData());
  DebugEvalNL((void*) config);

  try {
	 config->refresh();
  }
  catch (ErrorWithMsg& err) {
	 Tcl_AppendResult(togl->interp(), err.msg_cstr(), (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
  catch (...) {
	 Tcl_AppendResult(togl->interp(), "an error of unknown type occurred "
							"in dummyDisplayCallback", (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
}

void ToglConfig_Impl::dummyEpsCallback(const Togl* togl) {
DOTRACE("ToglConfig_Impl::dummyEpsCallback");
  ToglConfig* config = static_cast<ToglConfig*>(togl->getClientData());
  DebugEvalNL((void*) config);

  try {
	 config->refresh();
  }
  catch (ErrorWithMsg& err) {
	 Tcl_AppendResult(togl->interp(), err.msg_cstr(), (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
  catch (...) {
	 Tcl_AppendResult(togl->interp(), "an error of unknown type occurred "
							"in dummyEpsCallback", (char*) 0);
	 Tcl_BackgroundError(togl->interp());
  }
}

static const char vcid_toglconfig_cc[] = "$Header$";
#endif // !TOGLCONFIG_CC_DEFINED
