///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Sat Nov  9 21:18:00 2002
// $Id$
//
// This is a modified version of the Togl widget by Brian Paul and Ben
// Bederson; see the original copyright notice and log info below.
//
// Win32 support has been removed from this version, and the code has
// been C++-ified.
//
// *** Original Copyright Notice ***
//
// Togl - a Tk OpenGL widget
// Version 1.5
// Copyright (C) 1996-1997  Brian Paul and Ben Bederson
// See the LICENSE file for copyright details.
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGL_CC_DEFINED
#define TOGL_CC_DEFINED

#include "togl/togl.h"

#include "gfx/canvas.h"

#include "gx/rgbacolor.h"

#include "tcl/tclsafeinterp.h"

#include "togl/glutil.h"
#include "togl/glxopts.h"
#include "togl/glxwrapper.h"
#include "togl/x11util.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"

#include <X11/Xlib.h>
#include <tcl.h>
#include <tk.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Togl::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Togl::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Togl* itsOwner;
  const Tk_Window itsTkWin;
  shared_ptr<GlxOpts> itsOpts;
  shared_ptr<GlxWrapper> itsGlx;

  bool itsPrivateCmapFlag;
  int itsFontListBase;

  Impl(Togl* owner);
  ~Impl() throw() {}

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData);

  void swapBuffers() const;

  Togl::Color queryColor(unsigned int color_index) const;

  void loadFontList(GLuint newListBase);
};


Tk_ClassProcs toglProcs =
  {
    sizeof(Tk_ClassProcs),
    (Tk_ClassWorldChangedProc*) 0,
    Togl::Impl::cClassCreateProc,
    (Tk_ClassModalProc*) 0,
  };

//---------------------------------------------------------------------
//
// Togl::Impl::Impl
//
//---------------------------------------------------------------------

Togl::Impl::Impl(Togl* owner) :
  itsOwner(owner),
  itsTkWin(owner->tkWin()),
  itsOpts(new GlxOpts),
  itsGlx(0),

  itsPrivateCmapFlag(false),
  itsFontListBase(0)
{
DOTRACE("Togl::Impl::Impl");

  //
  // Get the window mapped onscreen
  //

  Tk_GeometryRequest(itsTkWin, itsOwner->width(), itsOwner->height());

  Tk_SetClassProcs(itsTkWin, &toglProcs, static_cast<ClientData>(this));

  Tk_MakeWindowExist(itsTkWin);

  Tk_MapWindow(itsTkWin);
}

void Togl::Impl::swapBuffers() const
{
DOTRACE("Togl::Impl::swapBuffers");

  itsGlx->flush(Tk_WindowId(itsTkWin));
}

Togl::Color Togl::Impl::queryColor(unsigned int color_index) const
{
  XColor col;

  col.pixel = color_index;
  XQueryColor(Tk_Display(itsTkWin), Tk_Colormap(itsTkWin), &col);

  Togl::Color color;

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

void Togl::Impl::loadFontList(GLuint newListBase)
{
DOTRACE("Togl::Impl::loadFontList");
  // Check if font loading succeeded...
  if (newListBase == 0)
    {
      throw Util::Error("unable to load font");
    }

  // ... otherwise unload the current font
  if (itsFontListBase > 0)
    {
      GLUtil::unloadBitmapFont(itsFontListBase);
    }

  // ... and point to the new font
  itsFontListBase = newListBase;
}

Window Togl::Impl::cClassCreateProc(Tk_Window tkwin,
                                    Window parent,
                                    ClientData clientData)
{
  Togl::Impl* rep = static_cast<Togl::Impl*>(clientData);

  Display* dpy = Tk_Display(tkwin);

  rep->itsGlx.reset(GlxWrapper::make(dpy, *(rep->itsOpts)));

  XVisualInfo* visInfo = rep->itsGlx->visInfo();

  Colormap cmap = X11Util::findColormap(dpy, visInfo,
                                        rep->itsPrivateCmapFlag);

  // Make sure Tk knows to switch to the new colormap when the cursor is over
  // this window when running in color index mode.
  Tk_SetWindowVisual(tkwin, visInfo->visual, visInfo->depth, cmap);

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
                             rep->itsOwner->width(),
                             rep->itsOwner->height(),
                             0, visInfo->depth,
                             InputOutput, visInfo->visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin,
                            CWBorderPixel | CWColormap | CWEventMask,
                            &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

  // Bind the context to the window and make it the current context
  rep->itsGlx->makeCurrent(win);

  if (rep->itsOpts->rgbaFlag)
    {
      DOTRACE("GlxWrapper::GlxWrapper::rgbaFlag");
      rep->itsGlx->canvas().setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      rep->itsGlx->canvas().setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      rep->itsGlx->canvas().setColorIndex(0);
      rep->itsGlx->canvas().setClearColorIndex(1);
    }

  // Check for a single/double buffering snafu
  if (rep->itsOpts->doubleFlag == 0 && rep->itsGlx->isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double buffered
      // visual.  Set the GL draw buffer to be the front buffer to
      // simulate single buffering.
      glDrawBuffer(GL_FRONT);
    }

  return win;
}

///////////////////////////////////////////////////////////////////////
//
// Togl member function definitions
//
///////////////////////////////////////////////////////////////////////

Togl::Togl(Tcl_Interp* interp, const char* pathname) :
  Tcl::TkWidget(interp, "Togl", pathname),
  rep(new Impl(this))
{
DOTRACE("Togl::Togl");
}

Togl::~Togl()
{
DOTRACE("Togl::~Togl");
  delete rep;
}

void Togl::displayCallback()
{
DOTRACE("Togl::displayCallback");

  rep->itsGlx->makeCurrent(Tk_WindowId(rep->itsTkWin));
  fullRender();
}

void Togl::makeCurrent() const          { rep->itsGlx->makeCurrent(Tk_WindowId(rep->itsTkWin)); }
void Togl::swapBuffers()                { rep->swapBuffers(); }
bool Togl::isRgba() const               { return rep->itsOpts->rgbaFlag; }
bool Togl::isDoubleBuffered() const     { return rep->itsOpts->doubleFlag; }
unsigned int Togl::bitsPerPixel() const { return rep->itsGlx->visInfo()->depth; }
bool Togl::hasPrivateCmap() const       { return rep->itsPrivateCmapFlag; }

Togl::Color Togl::queryColor(unsigned int color_index) const
  { return rep->queryColor(color_index); }

void Togl::loadDefaultFont() const
{
  loadBitmapFont(0);
}

void Togl::loadBitmapFont(const char* fontname) const
{
DOTRACE("Togl::loadBitmapFont");
  rep->loadFontList(GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin), fontname));
}

void Togl::loadBitmapFonti(int fontnumber) const
{
DOTRACE("Togl::loadBitmapFonti");
  rep->loadFontList(GLUtil::loadBitmapFont(Tk_Display(rep->itsTkWin),
                                           GLUtil::NamedFont(fontnumber)));
}

Gfx::Canvas& Togl::getCanvas() const
{
DOTRACE("Togl::getCanvas");
  makeCurrent();
  return rep->itsGlx->canvas();
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
