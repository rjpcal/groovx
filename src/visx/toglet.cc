///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 24 10:18:17 1999
// written: Mon Sep 16 18:50:28 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "visx/toglet.h"

#include "gfx/canvas.h"

#include "gx/rect.h"
#include "gx/rgbacolor.h"

#include "tcl/tclcode.h"
#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "togl/glutil.h"
#include "togl/togl.h"

#include "util/error.h"
#include "util/ref.h"
#include "util/strings.h"

#include "visx/xbmaprenderer.h"

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <tk.h>
#include <cmath>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  // the main window can be specified with either PARENT = "" or "."
  fstring PARENT = "";

  const char* widgetName(Util::UID id)
  {
    static fstring buf;
    buf = PARENT;
    buf.append(".togl_private");
    buf.append(int(id));

    return buf.c_str();
  }
}

///////////////////////////////////////////////////////////////////////
//
// TogletSizer
//
///////////////////////////////////////////////////////////////////////

class TogletSizer
{
public:
  enum ResizePolicy
    {
      FIXED_SCALE,
      FIXED_RECT,
      MIN_RECT,
      PERSPECTIVE,
    };

  TogletSizer(double dist = 30.0) :
    itsPolicy(FIXED_SCALE),
    itsViewingDistance(dist),
    itsPixelsPerUnit(1.0),
    itsRect()
  {}

  // For FIXED_SCALE mode:
  void setPixelsPerUnit(double s);
  void setUnitAngle(double deg, double screen_ppi);
  void setViewingDistIn(double inches);
  bool usingFixedScale() const { return itsPolicy == FIXED_SCALE; }

  // For FIXED_RECT mode:
  void setFixedRectLTRB(double L, double T, double R, double B);

  // For MIN_RECT mode:
  void setMinRectLTRB(double L, double T, double R, double B);

  // For FIXED_RECT or MIN_RECT modes:
  void scaleRect(double factor);

  // For PERSPECTIVE modes:
  void setPerspective(double fovy, double zNear, double zFar);

  void reconfigure(const int width, const int height);

private:
  void setRect(double L, double T, double R, double B)
  {
    // Test for valid rect: right > left && top > bottom. In
    // particular, we must not have right == left or top == bottom
    // since this collapses the space onto one dimension.
    if (R <= L || T <= B)
      {
        throw Util::Error("invalid rect");
      }

    itsRect.setRectLTRB(L,T,R,B);
  }

  ResizePolicy itsPolicy;
  double itsViewingDistance;    // inches
  double itsPixelsPerUnit;      // pixels per GLunit
  Gfx::Rect<double> itsRect;

  struct Perspective
  {
    double fovy;
    double zNear;
    double zFar;
  };

  Perspective itsPerspective;
};

void TogletSizer::setPixelsPerUnit(double s)
{
  if (s <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsPolicy = TogletSizer::FIXED_SCALE;
  itsPixelsPerUnit = s;
}

void TogletSizer::setUnitAngle(double deg_per_unit, double screen_ppi)
{
  if (deg_per_unit <= 0.0)
    throw Util::Error("unit angle must be positive");

  static const double deg_to_rad = 3.141593/180.0;

  // tan(deg_per_unit) == screen_inches_per_unit/viewing_dist;
  // screen_inches_per_unit == 1.0 * itsPixelsPerUnit / screepPpi;
  const double screen_inches_per_unit =
    tan(deg_per_unit*deg_to_rad) * itsViewingDistance;

  itsPixelsPerUnit = int(screen_inches_per_unit * screen_ppi);
  itsPolicy = TogletSizer::FIXED_SCALE;
}

void TogletSizer::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw Util::Error("viewing distance must be positive (duh)");

  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  const double factor = inches / itsViewingDistance;
  itsPixelsPerUnit *= factor;
  itsViewingDistance = inches;
}

void TogletSizer::setFixedRectLTRB(double L, double T, double R, double B)
{
  itsPolicy = TogletSizer::FIXED_RECT;
  setRect(L,T,R,B);
}

void TogletSizer::setMinRectLTRB(double L, double T, double R, double B)
{
  itsPolicy = TogletSizer::MIN_RECT;
  setRect(L,T,R,B);
}

void TogletSizer::scaleRect(double factor)
{
  if (factor <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsRect.widenByFactor(factor);
  itsRect.heightenByFactor(factor);
}

void TogletSizer::setPerspective(double fovy, double zNear, double zFar)
{
DOTRACE("TogletSizer::setPerspective");

  itsPolicy = PERSPECTIVE;

  itsPerspective.fovy = fovy;
  itsPerspective.zNear = zNear;
  itsPerspective.zFar = zFar;
}

void TogletSizer::reconfigure(const int width, const int height)
{
DOTRACE("TogletSizer::reconfigure");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, width, height);

  switch (itsPolicy)
    {
    case FIXED_SCALE:
      {
        const double l = -1 * (width  / 2.0) / itsPixelsPerUnit;
        const double r =      (width  / 2.0) / itsPixelsPerUnit;
        const double b = -1 * (height / 2.0) / itsPixelsPerUnit;
        const double t =      (height / 2.0) / itsPixelsPerUnit;
        glOrtho(l, r, b, t, -10.0, 10.0);
      }
      break;
    case FIXED_RECT:
      {
        glOrtho(itsRect.left(), itsRect.right(),
                itsRect.bottom(), itsRect.top(), -10.0, 10.0);
      }
      break;
    case MIN_RECT:
      {
        // the actual rect that we'll build:
        Gfx::Rect<double> port(itsRect);

        // the desired conditions are as follows:
        //    (1) port contains itsRect
        //    (2) port.aspect() == getAspect()
        //    (3) port is the smallest rectangle that meets (1) and (2)

        const double window_aspect = double(width) / double(height);

        const double ratio_of_aspects = itsRect.aspect() / window_aspect;

        if ( ratio_of_aspects < 1 ) // the available space is too wide...
          {
            port.widenByFactor(1/ratio_of_aspects); // so use some extra width
          }
        else                        // the available space is too tall...
          {
            port.heightenByFactor(ratio_of_aspects); // so use some extra height
          }

        glOrtho(port.left(), port.right(),
                port.bottom(), port.top(), -10.0, 10.0);
      }
      break;
    case PERSPECTIVE:
      {
        gluPerspective(itsPerspective.fovy,
                       double(width) / double(height),
                       itsPerspective.zNear,
                       itsPerspective.zFar);
      }
      break;
    default:
      Assert(0); // "can't happen"
    }
}

///////////////////////////////////////////////////////////////////////
//
// TogletImpl definition
//
///////////////////////////////////////////////////////////////////////

class TogletImpl
{
public:
  Toglet* const owner;
  scoped_ptr<TogletSizer> sizer;
  unsigned int fontListBase;

  TogletImpl(Toglet* p);

  ~TogletImpl();

  void loadFontList(GLuint newListBase);

  void reconfigure();
};

TogletImpl::TogletImpl(Toglet* p)
  :
  owner(p),
  sizer(new TogletSizer),
  fontListBase(0)
{}

TogletImpl::~TogletImpl()
{
DOTRACE("TogletImpl::~TogletImpl");
}

void TogletImpl::loadFontList(GLuint newListBase)
{
  // Check if font loading succeeded...
  if (newListBase == 0)
    {
      throw Util::Error("unable to load font");
    }

  // ... otherwise unload the current font
  if (fontListBase > 0)
    {
      owner->unloadBitmapFont(fontListBase);
    }

  // ... and point to the new font
  fontListBase = newListBase;
}

void TogletImpl::reconfigure()
{
DOTRACE("TogletImpl::reconfigure");

  owner->makeCurrent();

  sizer->reconfigure(owner->width(), owner->height());

  owner->requestRedisplay();
}

///////////////////////////////////////////////////////////////////////
//
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(bool pack) :
  Togl(Tcl::Main::interp(), widgetName(id())),
  Tcl::TkWidget(),
  rep(new TogletImpl(this))
{
DOTRACE("Toglet::Toglet");
  DebugEvalNL((void*) this);

  const double default_unit_angle = 2.05;

  setUnitAngle(default_unit_angle);

  Togl::makeCurrent();

  Tk_Window tkwin = Togl::tkWin();
  XBmapRenderer::initClass(tkwin);

  if (pack)
    {
      fstring pack_cmd_str = "pack ";
      pack_cmd_str.append( pathname() );
      pack_cmd_str.append( " -side left -expand 1 -fill both; update" );
      Tcl::Code pack_cmd(pack_cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);
      pack_cmd.invoke(Togl::interp());
    }

  TkWidget::setTkWin(tkwin);

  incrRefCount();
}

Toglet::~Toglet()
{
DOTRACE("Toglet::~Toglet");

  DebugEvalNL((void*)this);
  DebugEvalNL((void*)rep);

  delete rep;
}

///////////////
// accessors //
///////////////

Toglet::Color Toglet::queryColor(unsigned int color_index) const
{
  Color col;
  queryColor(color_index, col);
  return col;
}

void Toglet::queryColor(unsigned int color_index, Color& color) const
{
DOTRACE("Toglet::queryColor");

  Tk_Window tkwin = Togl::tkWin();
  Display* display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
  Colormap cmap = Togl::colormap();
  XColor col;

  col.pixel = color_index;
  XQueryColor(display, cmap, &col);

  color.pixel = (unsigned int)col.pixel;
#ifdef HAVE_LIMITS
  const unsigned short usmax = std::numeric_limits<unsigned short>::max();
#else
  const unsigned short usmax = USHRT_MAX;
#endif

  color.red   = double(col.red)   / usmax;
  color.green = double(col.green) / usmax;
  color.blue  = double(col.blue)  / usmax;
}

bool Toglet::usingFixedScale() const
{
DOTRACE("Toglet::usingFixedScale");
  return rep->sizer->usingFixedScale();
}

double Toglet::pixelsPerInch() const
{
  Screen* scr = Tk_Screen(reinterpret_cast<Tk_FakeWin*>(Togl::tkWin()));
  const int screen_pixel_width = XWidthOfScreen(scr);
  const int screen_mm_width = XWidthMMOfScreen(scr);

  const double screen_inch_width = screen_mm_width / 25.4;

  const double screen_ppi = screen_pixel_width / screen_inch_width;

  DebugEvalNL(screen_ppi);
  return screen_ppi;
}

Gfx::Canvas& Toglet::getCanvas()
{
DOTRACE("Toglet::getCanvas");
  return Togl::getCanvas();
}

//////////////////
// manipulators //
//////////////////

void Toglet::defaultParent(const char* pathname)
{
DOTRACE("Toglet::defaultParent");
  PARENT = pathname;
}

void Toglet::destroyWidget()
{
DOTRACE("Toglet::destroyWidget");
DebugPrintNL("Toglet::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(Togl::interp()) )
    {
      fstring destroy_cmd_str = "destroy ";
      destroy_cmd_str.append( pathname() );

      Tcl::Code destroy_cmd(destroy_cmd_str.c_str(),
                            Tcl::Code::BACKGROUND_ERROR);
      destroy_cmd.invoke(Togl::interp());
    }
}

void Toglet::scaleRect(double factor)
{
DOTRACE("Toglet::scaleRect");

  rep->sizer->scaleRect(factor);

  rep->reconfigure();
}

void Toglet::setColor(const Color& color)
{
DOTRACE("Toglet::setColor");

  static const char* const bad_val_msg = "RGB values must be in [0.0, 1.0]";
  static const char* const bad_index_msg = "color index must be in [0, 255]";

  if (                     color.pixel > 255) { throw Util::Error(bad_index_msg); }
  if (color.red   < 0.0 || color.red   > 1.0) { throw Util::Error(bad_val_msg); }
  if (color.green < 0.0 || color.green > 1.0) { throw Util::Error(bad_val_msg); }
  if (color.blue  < 0.0 || color.blue  > 1.0) { throw Util::Error(bad_val_msg); }

  Togl::setColor(color.pixel, color.red, color.green, color.blue);
}

void Toglet::setPixelsPerUnit(double s)
{
DOTRACE("Toglet::setPixelsPerUnit");
  rep->sizer->setPixelsPerUnit(s);
  rep->reconfigure();
}

void Toglet::setUnitAngle(double deg)
{
DOTRACE("Toglet::setUnitAngle");
  rep->sizer->setUnitAngle(deg, pixelsPerInch());
  rep->reconfigure();
}

void Toglet::setViewingDistIn(double inches)
{
DOTRACE("Toglet::setViewingDistIn");
  rep->sizer->setViewingDistIn(inches);
  rep->reconfigure();
}

void Toglet::setPerspective(double fovy, double zNear, double zFar)
{
DOTRACE("Toglet::setPerspective");
  rep->sizer->setPerspective(fovy, zNear, zFar);
  rep->reconfigure();
}

void Toglet::setFixedRectLTRB(double L, double T, double R, double B)
{
DOTRACE("Toglet::setFixedRectLTRB");
  rep->sizer->setFixedRectLTRB(L,T,R,B);
  rep->reconfigure();
}

void Toglet::setMinRectLTRB(double L, double T, double R, double B)
{
DOTRACE("Toglet::setMinRectLTRB");
  rep->sizer->setMinRectLTRB(L,T,R,B);
  rep->reconfigure();
}

/////////////
// actions //
/////////////

void Toglet::bind(const char* event_sequence, const char* script)
{
DOTRACE("Toglet::bind");

  fstring cmd_str("bind ", pathname(), " ");
  cmd_str.append( event_sequence, " ");
  cmd_str.append("{ ", script, " }");

  Tcl::Code cmd(cmd_str, Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(Togl::interp());
}

void Toglet::swapBuffers() { Togl::swapBuffers(); }

void Toglet::loadDefaultFont() { loadFont(0); }

void Toglet::loadFont(const char* fontname)
{
DOTRACE("Toglet::loadFont");

  rep->loadFontList(Togl::loadBitmapFont(fontname));
}

void Toglet::loadFonti(int fontnumber)
{
DOTRACE("Toglet::loadFonti");

  rep->loadFontList(Togl::loadBitmapFonti(fontnumber));
}

void Toglet::takeFocus()
{
DOTRACE("Toglet::takeFocus");
  fstring cmd_str = "focus -force ";
  cmd_str.append( pathname() );

  Tcl::Code cmd(cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(Togl::interp());
}

void Toglet::writeEpsFile(const char* filename)
{
DOTRACE("Toglet::writeEpsFile");
  Togl::makeCurrent();

  {
    Gfx::Canvas& canvas = getCanvas();

    Gfx::AttribSaver saver(canvas);

    // Set fore/background colors to extremes for the purposes of EPS
    // rendering
    if ( Togl::isRgba() )
      {
        canvas.setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
        canvas.setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
      }
    else
      {
        canvas.setColorIndex(0);
        canvas.setClearColorIndex(255);
      }

    // get a clear buffer
    canvas.clearColorBuffer();
    swapBuffers();

    fullRender();

    // do the EPS dump
    const bool inColor = false;
    GLUtil::generateEPS(filename, inColor, width(), height());
  }

  // redisplay original image
  fullRender();
}

void Toglet::displayCallback()
{
DOTRACE("Toglet::displayCallback");

  fullRender();
}

void Toglet::reshapeCallback()
{
DOTRACE("Toglet::reshapeCallback");

  rep->reconfigure();
}

static const char vcid_toglet_cc[] = "$Header$";
#endif // !TOGLET_CC_DEFINED
