///////////////////////////////////////////////////////////////////////
//
// togl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue May 23 13:11:59 2000
// written: Wed Nov 20 20:37:33 2002
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

#include "gfx/glcanvas.h"

#include "gx/rgbacolor.h"

#include "tcl/tclsafeinterp.h"

#include "util/error.h"
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
  Util::SoftRef<GLCanvas> itsCanvas;

  bool itsPrivateCmapFlag;

  Impl(Togl* owner);
  ~Impl() throw() {}

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData);

  void swapBuffers() const;

  Togl::Color queryColor(unsigned int color_index) const;
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
  itsCanvas(),

  itsPrivateCmapFlag(false)
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

  itsCanvas->glxFlush(Tk_WindowId(itsTkWin));
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

namespace
{
  Colormap findColormap(Display* dpy, Visual* visual,
                        int screen, bool privateCmap)
  {
    // For RGB colormap or shared color-index colormap
    if (!privateCmap)
      {
        int scrnum = DefaultScreen(dpy);

        if (visual == DefaultVisual(dpy, scrnum))
          {
            // Just share the default colormap
            return DefaultColormap(dpy, scrnum);
          }
        else
          {
            // Make a new read-only colormap
            return XCreateColormap(dpy,
                                   RootWindow(dpy, screen),
                                   visual, AllocNone);
          }
      }

    // For private color-index colormap
    else
      {
        // Use AllocAll to get a read/write colormap
        return XCreateColormap(dpy,
                               RootWindow(dpy, screen),
                               visual, AllocAll);
      }
  }
}

Window Togl::Impl::cClassCreateProc(Tk_Window tkwin,
                                    Window parent,
                                    ClientData clientData)
{
  Togl::Impl* rep = static_cast<Togl::Impl*>(clientData);

  Display* dpy = Tk_Display(tkwin);

  rep->itsCanvas = Util::SoftRef<GLCanvas>(GLCanvas::make(dpy));

  Visual* visual = rep->itsCanvas->visual();
  int screen = rep->itsCanvas->screen();
  int depth = rep->itsCanvas->bitsPerPixel();

  Colormap cmap = findColormap(dpy, visual, screen,
                               rep->itsPrivateCmapFlag);

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
                             rep->itsOwner->width(),
                             rep->itsOwner->height(),
                             0, depth,
                             InputOutput, visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin,
                            CWBorderPixel | CWColormap | CWEventMask,
                            &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

  // Bind the context to the window and make it the current context
  rep->itsCanvas->makeCurrent(win);

  if (rep->itsCanvas->isRgba())
    {
      DOTRACE("GlxWrapper::GlxWrapper::rgbaFlag");
      rep->itsCanvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      rep->itsCanvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      rep->itsCanvas->setColorIndex(0);
      rep->itsCanvas->setClearColorIndex(1);
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

  rep->itsCanvas->makeCurrent(Tk_WindowId(rep->itsTkWin));
  fullRender();
}

void Togl::makeCurrent() const          { rep->itsCanvas->makeCurrent(Tk_WindowId(rep->itsTkWin)); }
void Togl::swapBuffers()                { rep->swapBuffers(); }
bool Togl::hasPrivateCmap() const       { return rep->itsPrivateCmapFlag; }

Togl::Color Togl::queryColor(unsigned int color_index) const
  { return rep->queryColor(color_index); }

Gfx::Canvas& Togl::getCanvas() const
{
DOTRACE("Togl::getCanvas");
  makeCurrent();
  return *(rep->itsCanvas);
}

static const char vcid_togl_cc[] = "$Header$";
#endif // !TOGL_CC_DEFINED
