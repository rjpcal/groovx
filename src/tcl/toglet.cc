///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Nov 21 14:58:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "toglet.h"

#include "gfx/glcanvas.h"

#include "gx/rect.h"
#include "gx/rgbacolor.h"

#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/ref.h"
#include "util/strings.h"

#include <cmath>
#include <tcl.h>
#include <tk.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Xlib.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Toglet::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Toglet::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Toglet* owner;
  const Tk_Window tkWin;
  Util::SoftRef<GLCanvas> canvas;

  Impl(Toglet* p);
  ~Impl() throw() {}

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData);
};


Tk_ClassProcs toglProcs =
  {
    sizeof(Tk_ClassProcs),
    (Tk_ClassWorldChangedProc*) 0,
    Toglet::Impl::cClassCreateProc,
    (Tk_ClassModalProc*) 0,
  };

//---------------------------------------------------------------------
//
// Toglet::Impl::Impl
//
//---------------------------------------------------------------------

Toglet::Impl::Impl(Toglet* p) :
  owner(p),
  tkWin(owner->tkWin()),
  canvas()
{
DOTRACE("Toglet::Impl::Impl");

  //
  // Get the window mapped onscreen
  //

  Tk_GeometryRequest(tkWin, owner->width(), owner->height());
  Tk_SetClassProcs(tkWin, &toglProcs, static_cast<ClientData>(this));
  Tk_MakeWindowExist(tkWin);
  Tk_MapWindow(tkWin);
}

Window Toglet::Impl::cClassCreateProc(Tk_Window tkwin,
                                    Window parent,
                                    ClientData clientData)
{
  Toglet::Impl* rep = static_cast<Toglet::Impl*>(clientData);

  Display* dpy = Tk_Display(tkwin);

  rep->canvas = Util::SoftRef<GLCanvas>(GLCanvas::make(dpy));

  Visual* visual = rep->canvas->visual();
  int screen = rep->canvas->screen();
  int depth = rep->canvas->bitsPerPixel();

  Colormap cmap =
    visual == DefaultVisual(dpy, screen)
    ? DefaultColormap(dpy, screen)
    : XCreateColormap(dpy,
                      RootWindow(dpy, screen),
                      visual, AllocNone);

  // Make sure Tk knows to switch to the new colormap when the cursor is over
  // this window when running in color index mode.
  Tk_SetWindowVisual(tkwin, visual, depth, cmap);

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  XSetWindowAttributes atts;

  atts.colormap = cmap;
  atts.border_pixel = 0;
  atts.event_mask = ALL_EVENTS_MASK;

  Window win = XCreateWindow(dpy,
                             parent,
                             0, 0,
                             rep->owner->width(),
                             rep->owner->height(),
                             0, depth,
                             InputOutput, visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin,
                            CWBorderPixel | CWColormap | CWEventMask,
                            &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

  // Bind the context to the window and make it the current context
  rep->canvas->makeCurrent(win);

  if (rep->canvas->isRgba())
    {
      DOTRACE("GlxWrapper::GlxWrapper::rgbaFlag");
      rep->canvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      rep->canvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      rep->canvas->setColorIndex(0);
      rep->canvas->setClearColorIndex(1);
    }

  return win;
}

///////////////////////////////////////////////////////////////////////
//
// Toglet member function definitions
//
///////////////////////////////////////////////////////////////////////

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

  itsRect.scaleX(factor);
  itsRect.scaleY(factor);
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
            port.scaleX(1/ratio_of_aspects); // so use some extra width
          }
        else                        // the available space is too tall...
          {
            port.scaleY(ratio_of_aspects); // so use some extra height
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
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(bool pack) :
  Tcl::TkWidget(Tcl::Main::interp(), "Toglet", widgetName(id())),
  rep(new Impl(this)),
  itsSizer(new TogletSizer)
{
DOTRACE("Toglet::Toglet");

  dbgEvalNL(3, (void*) this);

  const double default_unit_angle = 2.05;

  setUnitAngle(default_unit_angle);

  if (pack) Tcl::TkWidget::pack();
}

Toglet::~Toglet()
{
DOTRACE("Toglet::~Toglet");

  dbgEvalNL(3, (void*)this);
  dbgEvalNL(3, (void*)itsSizer);

  delete itsSizer;

  delete rep;
}

///////////////
// accessors //
///////////////

bool Toglet::usingFixedScale() const
{
DOTRACE("Toglet::usingFixedScale");
  return itsSizer->usingFixedScale();
}

Gfx::Canvas& Toglet::getCanvas() const
{
DOTRACE("Toglet::getCanvas");
  makeCurrent();
  return *(rep->canvas);
}

//////////////////
// manipulators //
//////////////////

void Toglet::defaultParent(const char* pathname)
{
DOTRACE("Toglet::defaultParent");
  PARENT = pathname;
}

void Toglet::scaleRect(double factor)
{
DOTRACE("Toglet::scaleRect");

  itsSizer->scaleRect(factor);

  reshapeCallback();
}

void Toglet::setPixelsPerUnit(double s)
{
DOTRACE("Toglet::setPixelsPerUnit");
  itsSizer->setPixelsPerUnit(s);
  reshapeCallback();
}

void Toglet::setUnitAngle(double deg)
{
DOTRACE("Toglet::setUnitAngle");
  itsSizer->setUnitAngle(deg, pixelsPerInch());
  reshapeCallback();
}

void Toglet::setViewingDistIn(double inches)
{
DOTRACE("Toglet::setViewingDistIn");
  itsSizer->setViewingDistIn(inches);
  reshapeCallback();
}

void Toglet::setPerspective(double fovy, double zNear, double zFar)
{
DOTRACE("Toglet::setPerspective");
  itsSizer->setPerspective(fovy, zNear, zFar);
  reshapeCallback();
}

void Toglet::setFixedRectLTRB(double L, double T, double R, double B)
{
DOTRACE("Toglet::setFixedRectLTRB");
  itsSizer->setFixedRectLTRB(L,T,R,B);
  reshapeCallback();
}

void Toglet::setMinRectLTRB(double L, double T, double R, double B)
{
DOTRACE("Toglet::setMinRectLTRB");
  itsSizer->setMinRectLTRB(L,T,R,B);
  reshapeCallback();
}

void Toglet::makeCurrent() const
{
  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
}

void Toglet::displayCallback()
{
DOTRACE("Toglet::displayCallback");

  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
  fullRender();
}

void Toglet::reshapeCallback()
{
DOTRACE("Toglet::reshapeCallback");

  makeCurrent();

  itsSizer->reconfigure(width(), height());

  requestRedisplay();
}

void Toglet::swapBuffers()
{
DOTRACE("Toglet::swapBuffers");
  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
  rep->canvas->glxFlush();
}

Toglet::Color Toglet::queryColor(unsigned int color_index) const
{
DOTRACE("Toglet::queryColor");

  XColor col;

  col.pixel = color_index;
  XQueryColor(Tk_Display(rep->tkWin), Tk_Colormap(rep->tkWin), &col);

  Toglet::Color color;

  color.pixel = (unsigned int)col.pixel;
#ifdef HAVE_LIMITS
  const unsigned short usmax = std::numeric_limits<unsigned short>::max();
#else
  const unsigned short usmax = USHRT_MAX;
#endif

  color.red   = double(col.red)   / usmax;
  color.green = double(col.green) / usmax;
  color.blue  = double(col.blue)  / usmax;

  return color;
}

static const char vcid_toglet_cc[] = "$Header$";
#endif // !TOGLET_CC_DEFINED
