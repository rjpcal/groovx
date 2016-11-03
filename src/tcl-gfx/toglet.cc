/** @file gfx/toglet.cc an OpenGL widget for use with tcl/tk */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan  4 08:00:00 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/glwindowinterface.h"
#include "gfx/glxopts.h"
#include "gfx/gxfactory.h"
#include "gfx/gxscene.h"
#include "gfx/rgbacolor.h"

#if defined (GVX_GL_PLATFORM_GLX)
#  include "gfx/glxwrapper.h"
#elif defined(GVX_GL_PLATFORM_AGL)
#  include "gfx/aglwrapper.h"
#endif

#include "nub/ref.h"

#include "tcl/eventloop.h"
#include "tcl/timerscheduler.h"

#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <limits>
#include <tk.h>

#ifdef GVX_GL_PLATFORM_AGL
#  define HAVE_LIMITS_H
#  include <tkInt.h>
#  include <tkMacOSX.h>
#endif

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::shared_ptr;

namespace
{
  nub::soft_ref<Toglet> theCurrentToglet;

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

  Toglet*                       const owner;
  Tk_Window                     const tkWin;
  shared_ptr<GlxOpts>           const opts;
#if defined(GVX_GL_PLATFORM_GLX)
  shared_ptr<GlxWrapper>        const glx;
#elif defined(GVX_GL_PLATFORM_AGL)
  shared_ptr<AglWrapper>        const glx;
#endif
  nub::soft_ref<GLCanvas>       const canvas;
  GxScene*                      const scene;

  Impl(Toglet* p);
  ~Impl() noexcept
  {
    if (scene != nullptr)  scene->destroy();
    if (canvas.is_valid()) canvas->destroy();
  }
};


//---------------------------------------------------------------------
//
// Toglet::Impl::Impl
//
//---------------------------------------------------------------------

Toglet::Impl::Impl(Toglet* p) :
  owner(p),
  tkWin(owner->tkWin()),
  opts(new GlxOpts),
#if defined(GVX_GL_PLATFORM_GLX)
  glx(GlxWrapper::make(Tk_Display(tkWin), *opts,
                       (GlxWrapper*)0 /*shared context*/)),
#elif defined(GVX_GL_PLATFORM_AGL)
  glx(AglWrapper::make(*opts)),
#endif
  canvas(GLCanvas::make(opts, glx)),
  scene(new GxScene(canvas, rutz::make_shared(new tcl::timer_scheduler)))
{
GVX_TRACE("Toglet::Impl::Impl");
}

///////////////////////////////////////////////////////////////////////
//
// Toglet member function definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  // the main window can be specified with either PARENT = "" or "."
  rutz::fstring PARENT = "";

#ifdef GVX_GL_PLATFORM_GLX
  void configureGlxWindow(Tk_Window tkWin, const GlxWrapper* glx)
  {
    Display* dpy = Tk_Display(tkWin);

    Visual* visual = glx->visInfo()->visual;
    const int screen = glx->visInfo()->screen;

    Colormap cmap =
      visual == DefaultVisual(dpy, screen)
      ? DefaultColormap(dpy, screen)
      : XCreateColormap(dpy,
                        RootWindow(dpy, screen),
                        visual, AllocNone);

    // Make sure Tk knows to switch to the new colormap when the cursor is over
    // this window when running in color index mode.
    Tk_SetWindowVisual(tkWin, visual, glx->visInfo()->depth, cmap);

    XSetWindowAttributes atts;

    atts.colormap = cmap;
    atts.border_pixel = 0;
    atts.event_mask =
      KeyPressMask|KeyReleaseMask|
      ButtonPressMask|ButtonReleaseMask|
      EnterWindowMask|LeaveWindowMask|
      PointerMotionMask|ExposureMask|
      VisibilityChangeMask|FocusChangeMask|
      PropertyChangeMask|ColormapChangeMask;

    unsigned long valuemask = CWBorderPixel | CWColormap | CWEventMask;

    if (Tk_IsTopLevel(tkWin))
      {
        atts.override_redirect = true;
        valuemask |= CWOverrideRedirect;
      }

    Tk_ChangeWindowAttributes(tkWin, valuemask, &atts);
  }
#endif
}

///////////////////////////////////////////////////////////////////////
//
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(bool pack, bool topLevel) :
  tcl::TkWidget(tcl::event_loop::interp(), "Toglet",
                rutz::sfmt("%s.togl_private%lu",
                           PARENT.c_str(), id()).c_str(),
                topLevel),
  rep(new Impl(this))
{
GVX_TRACE("Toglet::Toglet");

  dbg_eval_nl(3, this);

  // Get the window mapped onscreen. NOTE -- we need to make these
  // calls here, rather than in Impl's constructor. Why? Because some
  // of these calls (e.g. Tk_MapWindow()) may trigger window-system
  // callbacks (such as ConfigureNotify), which will end up calling
  // Toglet's own functions, e.g. reshapeCallback(). Those functions
  // require that "rep" be initialized. If we do this window set up in
  // Impl's constructor, then it won't have returned yet, and so our
  // "rep" variable won't be pointing anywhere meaningful (will be
  // either nullptr or garbage).

  Tk_GeometryRequest(rep->tkWin, DEFAULT_SIZE_X, DEFAULT_SIZE_Y);

#ifdef GVX_GL_PLATFORM_GLX
  configureGlxWindow(rep->tkWin, rep->glx.get());
#endif

  Tk_MakeWindowExist(rep->tkWin);
  Tk_MapWindow(rep->tkWin);

#if defined(GVX_GL_PLATFORM_GLX)
  rep->glx->bindWindow(Tk_WindowId(rep->tkWin));
#elif defined(GVX_GL_PLATFORM_AGL)
  rep->glx->bindDrawable((CGrafPtr)TkMacOSXGetDrawablePort(Tk_WindowId(rep->tkWin)));
#endif

  // Bind the context to the window and make it the current context
  this->makeCurrent();

  if (rep->canvas->isRgba())
    {
      GVX_TRACE("GlxWrapper::GlxWrapper::rgbaFlag");
      rep->canvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      rep->canvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      rep->canvas->setColorIndex(0);
      rep->canvas->setClearColorIndex(1);
    }

  if (pack) tcl::TkWidget::pack();
}

Toglet::~Toglet() noexcept
{
GVX_TRACE("Toglet::~Toglet");

  dbg_eval_nl(3, this);

  delete rep;
}

Toglet* Toglet::make()
{
GVX_TRACE("Toglet::make");

  Toglet* p = new Toglet;

  return p;
}

Toglet* Toglet::makeToplevel()
{
GVX_TRACE("Toglet::makeToplevel");
  Toglet* p = new Toglet(true, true);

  return p;
}

nub::soft_ref<Toglet> Toglet::getCurrent()
{
GVX_TRACE("Toglet::getCurrent");
  return theCurrentToglet;
}

void Toglet::setCurrent(nub::soft_ref<Toglet> toglet)
{
GVX_TRACE("Toglet::setCurrent");
  dbg_eval(1, toglet.id());
  toglet->makeCurrent();
}

void Toglet::defaultParent(const char* pathname)
{
GVX_TRACE("Toglet::defaultParent");
  PARENT = pathname;
}

nub::soft_ref<Gfx::Canvas> Toglet::getCanvas() const
{
GVX_TRACE("Toglet::getCanvas");
  makeCurrent();
  return rep->canvas;
}

void Toglet::makeCurrent() const
{
  if (theCurrentToglet.id() != this->id() ||
      GLCanvas::getCurrent().id() != rep->canvas->id())
    {
      rep->canvas->makeCurrent();
      theCurrentToglet = nub::soft_ref<Toglet>(const_cast<Toglet*>(this));
    }
}

void Toglet::displayCallback()
{
GVX_TRACE("Toglet::displayCallback");
  fullRender();
}

void Toglet::reshapeCallback(int width, int height)
{
GVX_TRACE("Toglet::reshapeCallback");

  GVX_ASSERT(rep->glx.get() != nullptr);
  GVX_ASSERT(rep->scene != nullptr);

  makeCurrent();
  rep->glx->onReshape(width, height);
  rep->scene->reshape(width, height);
}

void Toglet::swapBuffers()
{
GVX_TRACE("Toglet::swapBuffers");
  makeCurrent();
  rep->canvas->flushOutput();
}

GxScene& Toglet::scene()
{
GVX_TRACE("Toglet::scene");
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

const nub::ref<GxCamera>& Toglet::getCamera() const
{
  makeCurrent();
  return rep->scene->getCamera();
}

void Toglet::setCamera(const nub::ref<GxCamera>& cam)
{
  makeCurrent();
  rep->scene->setCamera(cam);
}

void Toglet::setDrawable(const nub::ref<GxNode>& node)
{
  makeCurrent();
  rep->scene->setDrawable(node);
}

void Toglet::animate(unsigned int framesPerSecond)
{
  makeCurrent();
  rep->scene->animate(framesPerSecond);
}
