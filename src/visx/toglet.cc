///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 24 10:18:17 1999
// written: Tue Apr  2 16:31:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "visx/toglet.h"

#include "visx/xbmaprenderer.h"

#include "gfx/glcanvas.h"

#include "gx/rect.h"
#include "gx/rgbacolor.h"

#include "tcl/tclcode.h"
#include "tcl/tclsafeinterp.h"

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

  void setIntParam(Togl* togl, const char* param, int val)
  {
    fstring cmd_str(togl->pathname(), " configure -", param);
    cmd_str.append(" ", val);

    Tcl::Code cmd(cmd_str, Tcl::Code::THROW_EXCEPTION);
    cmd.invoke(togl->interp());
  }
}

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

class Toglet_Impl
{
public:
  static void reshapeCallback(Togl* togl);
  static void displayCallback(Togl* togl);
  static void epsCallback(const Togl* togl);

  static void destroyCallback(Togl* togl)
  {
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

  static void loadFontList(Toglet* self, GLuint newListBase)
  {
    // Check if font loading succeeded...
    if (newListBase == 0)
      {
        throw Util::Error("unable to load font");
      }

    // ... otherwise unload the current font
    if (self->itsFontListBase > 0)
      {
        self->itsTogl->unloadBitmapFont(self->itsFontListBase);
      }

    // ... and point to the new font
    self->itsFontListBase = newListBase;
  }
};

///////////////////////////////////////////////////////////////////////
//
// Toglet::Sizer
//
///////////////////////////////////////////////////////////////////////

class Toglet::Sizer
{
public:
  enum ResizePolicy
    {
      FIXED_SCALE,
      MIN_RECT
    };

  Sizer(double dist = 30.0) :
    itsPolicy(FIXED_SCALE),
    itsViewingDistance(dist),
    itsPixelsPerUnit(1.0),
    itsMinRect()
  {}

  // For FIXED_SCALE mode:
  void setPixelsPerUnit(double s);
  void setUnitAngle(double deg, double screen_ppi);
  void setViewingDistIn(double inches);
  bool usingFixedScale() const { return itsPolicy == FIXED_SCALE; }

  // For MIN_RECT mode:
  void scaleRect(double factor);
  void setMinRectLTRB(double L, double T, double R, double B);

  void reconfigure(const int width, const int height);

private:
  ResizePolicy itsPolicy;
  double itsViewingDistance;    // inches
  double itsPixelsPerUnit;         // pixels per GLunit
  Gfx::Rect<double> itsMinRect;
};

void Toglet::Sizer::setPixelsPerUnit(double s)
{
  if (s <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsPolicy = Sizer::FIXED_SCALE;
  itsPixelsPerUnit = s;
}

void Toglet::Sizer::setUnitAngle(double deg_per_unit, double screen_ppi)
{
  if (deg_per_unit <= 0.0)
    throw Util::Error("unit angle must be positive");

  static const double deg_to_rad = 3.141593/180.0;

  // tan(deg_per_unit) == screen_inches_per_unit/viewing_dist;
  // screen_inches_per_unit == 1.0 * itsPixelsPerUnit / screepPpi;
  const double screen_inches_per_unit =
    tan(deg_per_unit*deg_to_rad) * itsViewingDistance;

  itsPixelsPerUnit = int(screen_inches_per_unit * screen_ppi);
  itsPolicy = Sizer::FIXED_SCALE;
}

void Toglet::Sizer::setViewingDistIn(double inches)
{
  if (inches <= 0.0)
    throw Util::Error("viewing distance must be positive (duh)");

  // according to similar triangles,
  //   new_dist / old_dist == new_scale / old_scale;
  const double factor = inches / itsViewingDistance;
  itsPixelsPerUnit *= factor;
  itsViewingDistance = inches;
}

void Toglet::Sizer::scaleRect(double factor)
{
  if (factor <= 0.0)
    throw Util::Error("invalid scaling factor");

  itsMinRect.widenByFactor(factor);
  itsMinRect.heightenByFactor(factor);
}

void Toglet::Sizer::setMinRectLTRB(double L, double T, double R, double B)
{
  itsPolicy = Sizer::MIN_RECT;
  itsMinRect.setRectLTRB(L,T,R,B);
}

void Toglet::Sizer::reconfigure(const int width, const int height)
{
DOTRACE("Toglet::Sizer::reconfigure");

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
        glOrtho(l, r, b, t, -1.0, 1.0);
      }
      break;
    case MIN_RECT:
      {
        // the actual rect that we'll build:
        Gfx::Rect<double> port(itsMinRect);

        // the desired conditions are as follows:
        //    (1) port contains itsMinRect
        //    (2) port.aspect() == getAspect()
        //    (3) port is the smallest rectangle that meets (1) and (2)

        const double window_aspect = double(width) / double(height);

        const double ratio_of_aspects = itsMinRect.aspect() / window_aspect;

        if ( ratio_of_aspects < 1 ) // the available space is too wide...
          {
            port.widenByFactor(1/ratio_of_aspects); // so use some extra width
          }
        else                        // the available space is too tall...
          {
            port.heightenByFactor(ratio_of_aspects); // so use some extra height
          }

        glOrtho(port.left(), port.right(),
                port.bottom(), port.top(), -1.0, 1.0);
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

Toglet::Toglet(Tcl_Interp* interp,
               int config_argc, char** config_argv,
               bool pack) :
  Tcl::TkWidget(),
  itsTogl(new Togl(interp, widgetName(id()), config_argc, config_argv)),
  itsCanvas(new GLCanvas(itsTogl->bitsPerPixel(),
                         itsTogl->isRgba(), itsTogl->isDoubleBuffered())),
  itsSizer(new Sizer),
  itsFontListBase(0)
{
DOTRACE("Toglet::Toglet");
  DebugEvalNL((void*) this);

  Assert(itsTogl != 0);

  itsTogl->setClientData(static_cast<ClientData>(this));

  itsTogl->setReshapeFunc(Toglet_Impl::reshapeCallback);
  itsTogl->setDisplayFunc(Toglet_Impl::displayCallback);
  itsTogl->setDestroyFunc(Toglet_Impl::destroyCallback);

  const double default_unit_angle = 2.05;

  setUnitAngle(default_unit_angle);

  itsTogl->makeCurrent();

  if ( itsTogl->isRgba() )
    {
      itsCanvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      itsCanvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    { // not using rgba
      if ( itsTogl->hasPrivateCmap() )
        {
          itsCanvas->setColorIndex(0);
          itsCanvas->setClearColorIndex(1);
        }
      else
        {
          itsCanvas->setColorIndex(itsTogl->allocColor(0.0, 0.0, 0.0));
          itsCanvas->setClearColorIndex(itsTogl->allocColor(1.0, 1.0, 1.0));
        }
    }

  Tk_Window tkwin = itsTogl->tkWin();
  XBmapRenderer::initClass(tkwin);

  if (pack)
    {
      fstring pack_cmd_str = "pack ";
      pack_cmd_str.append( pathname() );
      pack_cmd_str.append( " -side left -expand 1 -fill both; update" );
      Tcl::Code pack_cmd(pack_cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);
      pack_cmd.invoke(interp);
    }

  TkWidget::setTkWin(tkwin);

  incrRefCount();
}

Toglet::~Toglet()
{
DOTRACE("Toglet::~Toglet");

  Assert(itsTogl != 0);

  itsTogl->setClientData(static_cast<ClientData>(0));
  itsTogl->setReshapeFunc(0);
  itsTogl->setDisplayFunc(0);
}

///////////////
// accessors //
///////////////

Tcl_Interp* Toglet::getInterp() const
{
DOTRACE("Toglet::getInterp");
  return itsTogl->interp();
}

int Toglet::getHeight() const
{
DOTRACE("Toglet::getHeight");
  return itsTogl->height();
}

int Toglet::getWidth() const
{
DOTRACE("Toglet::getWidth");
  return itsTogl->width();
}

const char* Toglet::pathname() const
{
DOTRACE("Toglet::pathname");
  return itsTogl->pathname();
}

Toglet::Color Toglet::queryColor(unsigned int color_index) const
{
  Color col;
  queryColor(color_index, col);
  return col;
}

void Toglet::queryColor(unsigned int color_index, Color& color) const
{
DOTRACE("Toglet::queryColor");

  Tk_Window tkwin = itsTogl->tkWin();
  Display* display = Tk_Display(reinterpret_cast<Tk_FakeWin *>(tkwin));
  Colormap cmap = itsTogl->colormap();
  XColor col;

  col.pixel = color_index;
  XQueryColor(display, cmap, &col);

  color.pixel = (unsigned int)col.pixel;
#ifndef NO_CPP_LIMITS
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
  return itsSizer->usingFixedScale();
}

double Toglet::pixelsPerInch() const
{
  Screen* scr = Tk_Screen(reinterpret_cast<Tk_FakeWin*>(itsTogl->tkWin()));
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
  itsTogl->makeCurrent();
  return *itsCanvas;
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
  if ( !Tcl_InterpDeleted(itsTogl->interp()) )
    {
      fstring destroy_cmd_str = "destroy ";
      destroy_cmd_str.append( pathname() );

      Tcl::Code destroy_cmd(destroy_cmd_str.c_str(),
                            Tcl::Code::BACKGROUND_ERROR);
      destroy_cmd.invoke(itsTogl->interp());
    }
}

void Toglet::scaleRect(double factor)
{
DOTRACE("Toglet::scaleRect");

  itsSizer->scaleRect(factor);

  reconfigure();
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

  itsTogl->setColor(color.pixel, color.red, color.green, color.blue);
}

void Toglet::setPixelsPerUnit(double s)
{
DOTRACE("Toglet::setPixelsPerUnit");
  itsSizer->setPixelsPerUnit(s);
  reconfigure();
}

void Toglet::setUnitAngle(double deg)
{
DOTRACE("Toglet::setUnitAngle");
  itsSizer->setUnitAngle(deg, pixelsPerInch());
  reconfigure();
}

void Toglet::setViewingDistIn(double inches)
{
DOTRACE("Toglet::setViewingDistIn");
  itsSizer->setViewingDistIn(inches);
  reconfigure();
}

void Toglet::setMinRectLTRB(double L, double T, double R, double B)
{
DOTRACE("Toglet::setMinRectLTRB");
  itsSizer->setMinRectLTRB(L,T,R,B);
  reconfigure();
}

void Toglet::setHeight(int val)
{
DOTRACE("Toglet::setHeight");

  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "height", val);
}

void Toglet::setWidth(int val)
{
DOTRACE("Toglet::setWidth");

  // This automatically triggers a ConfigureNotify/Expose event pair
  // through the Togl/Tk machinery
  setIntParam(itsTogl, "width", val);
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

  cmd.invoke(itsTogl->interp());
}

void Toglet::loadDefaultFont() { loadFont(0); }

void Toglet::loadFont(const char* fontname)
{
DOTRACE("Toglet::loadFont");

  Toglet_Impl::loadFontList(this, itsTogl->loadBitmapFont(fontname));
}

void Toglet::loadFonti(int fontnumber)
{
DOTRACE("Toglet::loadFonti");

  Toglet_Impl::loadFontList(this, itsTogl->loadBitmapFonti(fontnumber));
}

void Toglet::reconfigure()
{
DOTRACE("Toglet::reconfigure");

  itsTogl->makeCurrent();

  itsSizer->reconfigure(getWidth(), getHeight());

  itsTogl->postRedisplay();
}

void Toglet::swapBuffers()
{
DOTRACE("Toglet::swapBuffers");
  itsTogl->swapBuffers();
}

void Toglet::takeFocus()
{
DOTRACE("Toglet::takeFocus");
  fstring cmd_str = "focus -force ";
  cmd_str.append( pathname() );

  Tcl::Code cmd(cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(itsTogl->interp());
}

void Toglet::makeCurrent()
{
DOTRACE("Toglet::makeCurrent");
  itsTogl->makeCurrent();
}

void Toglet::writeEpsFile(const char* filename)
{
DOTRACE("Toglet::writeEpsFile");
  itsTogl->makeCurrent();

  {
    Gfx::AttribSaver saver(*itsCanvas);

    // Set fore/background colors to extremes for the purposes of EPS
    // rendering
    if ( itsTogl->isRgba() )
      {
        itsCanvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
        itsCanvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
      }
    else
      {
        itsCanvas->setColorIndex(0);
        itsCanvas->setClearColorIndex(255);
      }

    // get a clear buffer
    itsCanvas->clearColorBuffer();
    swapBuffers();

    // do the EPS dump
    const int rgbFlag = 0;
    itsTogl->dumpToEpsFile(filename, rgbFlag, Toglet_Impl::epsCallback);
  }

  // redisplay original image
  fullRender();
}

///////////////////////////////////////////////////////////////////////
//
// Callbacks
//
///////////////////////////////////////////////////////////////////////

void Toglet_Impl::reshapeCallback(Togl* togl)
{
DOTRACE("Toglet_Impl::reshapeCallback");
  Toglet* config = static_cast<Toglet*>(togl->getClientData());
  DebugEvalNL((void*) config);

  try
    {
      config->reconfigure();
    }
  catch (...)
    {
      Tcl::Interp(togl->interp()).handleLiveException("reshapeCallback", true);
    }
}

void Toglet_Impl::displayCallback(Togl* togl)
{
DOTRACE("Toglet_Impl::displayCallback");

  Toglet* toglet = static_cast<Toglet*>(togl->getClientData());
  DebugEvalNL((void*) toglet);

  try
    {
      toglet->fullRender();
    }
  catch (...)
    {
      Tcl::Interp(togl->interp()).handleLiveException("displayCallback", true);
    }
}

void Toglet_Impl::epsCallback(const Togl* togl)
{
  displayCallback(const_cast<Togl*>(togl));
}

static const char vcid_toglet_cc[] = "$Header$";
#endif // !TOGLET_CC_DEFINED
