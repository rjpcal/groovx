///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 24 10:18:17 1999
// written: Fri Jun 22 09:06:55 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "toglet.h"

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

namespace {
  // the main window can be specified with either PARENT = "" or "."
  fixed_string PARENT = "";

  const char* widgetName(Util::UID id)
  {
    static dynamic_string buf;
    buf = PARENT;
    buf.append(".togl_private");
    buf.append(int(id));

    return buf.c_str();
  }
}

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

class Toglet_Impl {
public:
  static void dummyReshapeCallback(Togl* togl);
  static void dummyDisplayCallback(Togl* togl);
  static void dummyEpsCallback(const Togl* togl);
};

namespace {

  void toglDestroyCallback(Togl* togl) {
  DOTRACE("toglDestroyCallback");
    DebugEvalNL((void*)togl);
    Toglet* config = static_cast<Toglet*>(togl->getClientData());


    // We have to test that config is non-null, because if this
    // callback was triggered out of Toglet's destructor, then it
    // will have already set the client data for the Togl* to null.
    DebugEvalNL((void*)config);
    if (config)
      {
        config->decrRefCount();
      }
  }

  void setIntParam(Togl* togl, const char* param, int val) {
    dynamic_string builder = togl->pathname();
    builder.append(" configure -").append(param)
      .append(" ").append(val);

    fixed_string buf(builder.c_str());

    Tcl::TclEvalCmd cmd(buf.data(), Tcl::TclEvalCmd::THROW_EXCEPTION);
    cmd.invoke(togl->interp());
  }
}

///////////////////////////////////////////////////////////////////////
//
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(Tcl_Interp* interp,
               int config_argc, char** config_argv,
               bool pack,
               double dist, double unit_angle) :
  Tcl::TkWidget(),
  itsCanvas(new GLCanvas),
  itsTogl(new Togl(interp, widgetName(id()), config_argc, config_argv)),
  itsViewingDistance(dist),
  itsFixedScaleFlag(true),
  itsFixedScale(1.0),
  itsMinRect(),
  itsFontListBase(0)
{
DOTRACE("Toglet::Toglet");
  DebugEvalNL((void*) this);

  Assert(itsTogl != 0);

  itsTogl->setClientData(static_cast<ClientData>(this));

  itsTogl->setReshapeFunc(Toglet_Impl::dummyReshapeCallback);
  itsTogl->setDisplayFunc(Toglet_Impl::dummyDisplayCallback);
  itsTogl->setDestroyFunc(toglDestroyCallback);

  setUnitAngle(unit_angle);

  itsTogl->makeCurrent();

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
  XBmapRenderer::initClass(tkwin);

  if (pack) {
    dynamic_string pack_cmd_str = "pack ";
    pack_cmd_str += pathname();
    pack_cmd_str += " -expand 1 -fill both; update";
    Tcl::TclEvalCmd pack_cmd(pack_cmd_str.c_str(),
                             Tcl::TclEvalCmd::THROW_EXCEPTION);
    pack_cmd.invoke(interp);
  }

  incrRefCount();
}

Toglet::~Toglet() {
DOTRACE("Toglet::~Toglet");

  Assert(itsTogl != 0);

  itsTogl->setClientData(static_cast<ClientData>(0));
  itsTogl->setReshapeFunc(0);
  itsTogl->setDisplayFunc(0);
}

///////////////
// accessors //
///////////////

Tk_Window Toglet::tkWin() const {
DOTRACE("Toglet::tkWin");

  return itsTogl->tkWin();
}

double Toglet::getFixedScale() const {
DOTRACE("Toglet::getFixedScale");
  return itsFixedScale;
}

Rect<double> Toglet::getMinRect() const {
DOTRACE("Toglet::getMinRect");
  return itsMinRect;
}

Tcl_Interp* Toglet::getInterp() const {
DOTRACE("Toglet::getInterp");

  return itsTogl->interp();
}

int Toglet::getHeight() const {
DOTRACE("Toglet::getHeight");

  return itsTogl->height();
}

int Toglet::getWidth() const {
DOTRACE("Toglet::getWidth");

  return itsTogl->width();
}

const char* Toglet::pathname() const {
DOTRACE("Toglet::pathname");

  return itsTogl->pathname();
}

void Toglet::queryColor(unsigned int color_index, Color& color) const {
DOTRACE("Toglet::queryColor");

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

bool Toglet::usingFixedScale() const {
DOTRACE("Toglet::usingFixedScale");
  return itsFixedScaleFlag;
}

Display* Toglet::getX11Display() const {
DOTRACE("getX11Display");

  return itsTogl->display();
}

int Toglet::getX11ScreenNumber() const {
DOTRACE("getX11ScreenNumber");

  return itsTogl->screenNumber();
}

Window Toglet::getX11Window() const {
DOTRACE("getX11Window");

  return itsTogl->windowId();
}

GWT::Canvas& Toglet::getCanvas() {
DOTRACE("Toglet::getCanvas");

  itsTogl->makeCurrent();

  return *itsCanvas;
}

//////////////////
// manipulators //
//////////////////

void Toglet::defaultParent(const char* pathname) {
DOTRACE("Toglet::defaultParent");
  PARENT = pathname;
}

void Toglet::destroyWidget() {
DOTRACE("Toglet::destroyWidget");
DebugPrintNL("Toglet::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(itsTogl->interp()) ) {
    dynamic_string destroy_cmd_str = "destroy ";
    destroy_cmd_str += pathname();

    Tcl::TclEvalCmd destroy_cmd(destroy_cmd_str.c_str(),
                                Tcl::TclEvalCmd::BACKGROUND_ERROR);
    destroy_cmd.invoke(itsTogl->interp());
  }
}

void Toglet::scaleRect(double factor) {
DOTRACE("Toglet::scaleRect");
#ifdef ACC_COMPILER
  try {
#endif
    if (factor <= 0.0)
      throw ToglError("invalid scaling factor");
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

  itsMinRect.widenByFactor(factor);
  itsMinRect.heightenByFactor(factor);

  reconfigure();
}

void Toglet::setColor(const Color& color) {
DOTRACE("Toglet::setColor");

  static const char* const bad_val_msg = "RGB values must be in [0.0, 1.0]";
  static const char* const bad_index_msg = "color index must be in [0, 255]";

#ifdef ACC_COMPILER
  try {
#endif
    if (                     color.pixel > 255) { throw ToglError(bad_index_msg); }
    if (color.red   < 0.0 || color.red   > 1.0) { throw ToglError(bad_val_msg); }
    if (color.green < 0.0 || color.green > 1.0) { throw ToglError(bad_val_msg); }
    if (color.blue  < 0.0 || color.blue  > 1.0) { throw ToglError(bad_val_msg); }
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

  itsTogl->setColor(color.pixel, color.red, color.green, color.blue);
}

void Toglet::setFixedScale(double s) {
DOTRACE("Toglet::setFixedScale");
#ifdef ACC_COMPILER
  try {
#endif
    if (s <= 0.0)
      throw ToglError("invalid scaling factor");
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

  itsFixedScaleFlag = true;
  itsFixedScale = s;

  reconfigure();
}

void Toglet::setUnitAngle(double deg) {
DOTRACE("Toglet::setUnitAngle");

#ifdef ACC_COMPILER
  try {
#endif
    if (deg <= 0.0)
      throw ToglError("unit angle must be positive");
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

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

void Toglet::setViewingDistIn(double in) {
DOTRACE("Toglet::setViewingDistIn");
#ifdef ACC_COMPILER
  try {
#endif
    if (in <= 0.0)
      { throw ToglError("viewing distance must be positive (duh)"); }
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif


  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  double factor = in / itsViewingDistance;
  itsFixedScale *= factor;
  itsViewingDistance = in;

  reconfigure();
}

void Toglet::setMinRectLTRB(double L, double T, double R, double B) {
DOTRACE("Toglet::setMinRectLTRB");
  itsFixedScaleFlag = false;
  itsMinRect.setRectLTRB(L,T,R,B);

  reconfigure();
}

void Toglet::setHeight(int val) {
DOTRACE("Toglet::setHeight");

  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "height", val);
}

void Toglet::setWidth(int val) {
DOTRACE("Toglet::setWidth");

  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "width", val);
}

/////////////
// actions //
/////////////

void Toglet::bind(const char* event_sequence, const char* script) {
DOTRACE("Toglet::bind");

  dynamic_string cmd_str = "bind ";
  cmd_str.append( pathname() ).append(" ");
  cmd_str.append( event_sequence ).append(" ");
  cmd_str.append( script );

  Tcl::TclEvalCmd cmd(cmd_str.c_str(),
                      Tcl::TclEvalCmd::THROW_EXCEPTION);
#ifdef ACC_COMPILER
  try {
#endif
    cmd.invoke(itsTogl->interp());
#ifdef ACC_COMPILER
  }
  catch (Tcl::TclError&) { throw; }
#endif
}

void Toglet::loadDefaultFont() { loadFont(0); }

void Toglet::loadFont(const char* fontname) {
DOTRACE("Toglet::loadFont");

  GLuint newListBase = itsTogl->loadBitmapFont(fontname);

#ifdef ACC_COMPILER
  try {
#endif
    // Check if font loading succeeded...
    if (newListBase == 0) {
      DebugEval(fontname);
      ToglError err("unable to load font ");
      err.appendMsg(fontname);
      throw err;
    }
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
    itsTogl->unloadBitmapFont(itsFontListBase);
  }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

void Toglet::loadFonti(int fontnumber) {
DOTRACE("Toglet::loadFonti");

  GLuint newListBase = itsTogl->loadBitmapFonti(fontnumber);

  // Check if font loading succeeded...
#ifdef ACC_COMPILER
  try {
#endif
    if (newListBase == 0) {
      throw ToglError("unable to load font");
    }
#ifdef ACC_COMPILER
  }
  catch (ToglError&) { throw; }
#endif

  // ... otherwise unload the current font
  if (itsFontListBase > 0) {
    itsTogl->unloadBitmapFont(itsFontListBase);
  }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

void Toglet::reconfigure() {
DOTRACE("Toglet::reconfigure");

  itsTogl->makeCurrent();

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

void Toglet::swapBuffers() {
DOTRACE("Toglet::swapBuffers");

  itsTogl->swapBuffers();
}

void Toglet::takeFocus() {
DOTRACE("Toglet::takeFocus");

  dynamic_string cmd_str = "focus -force ";
  cmd_str.append( pathname() );

  Tcl::TclEvalCmd cmd(cmd_str.c_str(),
                      Tcl::TclEvalCmd::THROW_EXCEPTION);
#ifdef ACC_COMPILER
  try {
#endif
    cmd.invoke(itsTogl->interp());
#ifdef ACC_COMPILER
  }
  catch (Tcl::TclError&) { throw; }
#endif
}

void Toglet::makeCurrent() {
DOTRACE("Toglet::makeCurrent");

  itsTogl->makeCurrent();
}

void Toglet::writeEpsFile(const char* filename) {
DOTRACE("Toglet::writeEpsFile");

  itsTogl->makeCurrent();

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
                           Toglet_Impl::dummyEpsCallback);
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

void Toglet_Impl::dummyReshapeCallback(Togl* togl) {
DOTRACE("Toglet_Impl::dummyReshapeCallback");
  Toglet* config = static_cast<Toglet*>(togl->getClientData());
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

void Toglet_Impl::dummyDisplayCallback(Togl* togl) {
DOTRACE("Toglet_Impl::dummyDisplayCallback");

  Toglet* config = static_cast<Toglet*>(togl->getClientData());
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

void Toglet_Impl::dummyEpsCallback(const Togl* togl) {
DOTRACE("Toglet_Impl::dummyEpsCallback");
  Toglet* config = static_cast<Toglet*>(togl->getClientData());
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

static const char vcid_toglet_cc[] = "$Header$";
#endif // !TOGLET_CC_DEFINED
