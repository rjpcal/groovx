///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/gxscene.h"

#include "gx/rgbacolor.h"

#include "tcl/tclmain.h"

#include "util/ref.h"
#include "util/strings.h"

#include <tk.h>
#include <X11/Xlib.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

namespace
{
  Util::SoftRef<Toglet> theCurrentToglet;

  const int DEFAULT_SIZE_X = 400;
  const int DEFAULT_SIZE_Y = 400;
}

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
  GxScene* scene;

  Impl(Toglet* p);
  ~Impl() throw()
  {
    scene->destroy();
    canvas->destroy();
  }

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData) throw();
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
  canvas(GLCanvas::make(Tk_Display(tkWin))),
  scene(new GxScene(canvas))
{
DOTRACE("Toglet::Impl::Impl");
}

Window Toglet::Impl::cClassCreateProc(Tk_Window tkwin,
                                      Window parent,
                                      ClientData clientData) throw()
{
DOTRACE("Toglet::Impl::cClassCreateProc");
  Toglet* toglet = static_cast<Toglet*>(clientData);
  Toglet::Impl* rep = toglet->rep;

  Display* dpy = Tk_Display(tkwin);

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

  unsigned long valuemask = CWBorderPixel | CWColormap | CWEventMask;

  if (Tk_IsTopLevel(tkwin))
    {
      atts.override_redirect = true;
      valuemask |= CWOverrideRedirect;
    }

  Window win = XCreateWindow(dpy,
                             parent,
                             0, 0,
                             DEFAULT_SIZE_X,
                             DEFAULT_SIZE_Y,
                             0, depth,
                             InputOutput, visual,
                             valuemask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin, valuemask, &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

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
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(bool pack, bool topLevel) :
  Tcl::TkWidget(Tcl::Main::interp(), "Toglet", widgetName(id()), topLevel),
  rep(new Impl(this))
{
DOTRACE("Toglet::Toglet");

  dbgEvalNL(3, this);

  // Get the window mapped onscreen. NOTE -- we need to make these calls
  // here, rather than in Impl's constructor. Why? Because some of these
  // calls (e.g. Tk_MapWindow()) may trigger window-system callbacks (such
  // as ConfigureNotify), which will end up calling Toglet's own functions,
  // e.g. reshapeCallback(). Those functions require that "rep" be
  // initialized. If we do this window set up in Impl's constructor, then
  // it won't have returned yet, and so our "rep" variable won't be
  // pointing anywhere meaningful (will be either NULL or garbage).

  Tk_GeometryRequest(rep->tkWin, DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
  Tk_SetClassProcs(rep->tkWin, &toglProcs, static_cast<ClientData>(this));
  Tk_MakeWindowExist(rep->tkWin);
  Tk_MapWindow(rep->tkWin);

  // Bind the context to the window and make it the current context
  this->makeCurrent();

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

  if (pack) Tcl::TkWidget::pack();
}

Toglet::~Toglet() throw()
{
DOTRACE("Toglet::~Toglet");

  dbgEvalNL(3, (void*)this);

  delete rep;
}

Toglet* Toglet::make()
{
DOTRACE("Toglet::make");

  Toglet* p = new Toglet;

  return p;
}

Toglet* Toglet::makeToplevel()
{
DOTRACE("Toglet::makeToplevel");
  Toglet* p = new Toglet(true, true);

  return p;
}

Util::SoftRef<Toglet> Toglet::getCurrent()
{
DOTRACE("Toglet::getCurrent");
  return theCurrentToglet;
}

void Toglet::setCurrent(Util::SoftRef<Toglet> toglet)
{
DOTRACE("Toglet::setCurrent");
  dbgEval(1, toglet.id());
  toglet->makeCurrent();
}

void Toglet::defaultParent(const char* pathname)
{
DOTRACE("Toglet::defaultParent");
  PARENT = pathname;
}

Gfx::Canvas& Toglet::getCanvas() const
{
DOTRACE("Toglet::getCanvas");
  makeCurrent();
  return *(rep->canvas);
}

void Toglet::makeCurrent() const
{
  if (theCurrentToglet.id() != this->id())
    {
      rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
      theCurrentToglet = Util::SoftRef<Toglet>(const_cast<Toglet*>(this));
    }
}

void Toglet::displayCallback()
{
DOTRACE("Toglet::displayCallback");
  fullRender();
}

void Toglet::reshapeCallback(int width, int height)
{
DOTRACE("Toglet::reshapeCallback");
  makeCurrent();
  rep->scene->reshape(width, height);
}

void Toglet::swapBuffers()
{
DOTRACE("Toglet::swapBuffers");
  makeCurrent();
  rep->canvas->flushOutput();
}

GxScene& Toglet::scene()
{
DOTRACE("Toglet::scene");
  return *(rep->scene);
}

void Toglet::render()
{
  makeCurrent();
  rep->scene->render();
}

void Toglet::fullRender()
{
  makeCurrent();
  rep->scene->fullRender();
}

void Toglet::clearscreen()
{
  makeCurrent();
  rep->scene->clearscreen();
}

void Toglet::fullClearscreen()
{
  makeCurrent();
  rep->scene->fullClearscreen();
}

void Toglet::undraw()
{
  makeCurrent();
  rep->scene->undraw();
}

void Toglet::setVisibility(bool vis)
{
  makeCurrent();
  rep->scene->setVisibility(vis);
}

void Toglet::setHold(bool hold_on)
{
  makeCurrent();
  rep->scene->setHold(hold_on);
}

void Toglet::allowRefresh(bool allow)
{
  makeCurrent();
  rep->scene->allowRefresh(allow);
}

const Util::Ref<GxCamera>& Toglet::getCamera() const
{
  makeCurrent();
  return rep->scene->getCamera();
}

void Toglet::setCamera(const Util::Ref<GxCamera>& cam)
{
  makeCurrent();
  rep->scene->setCamera(cam);
}

void Toglet::setDrawable(const Util::Ref<GxNode>& node)
{
  makeCurrent();
  rep->scene->setDrawable(node);
}

void Toglet::animate(unsigned int framesPerSecond)
{
  makeCurrent();
  rep->scene->animate(framesPerSecond);
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
