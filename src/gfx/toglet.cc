///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GFX_TOGLET_CC_UTC20050626084024_DEFINED
#define GROOVX_GFX_TOGLET_CC_UTC20050626084024_DEFINED

#include "toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/glwindowinterface.h"
#include "gfx/glxopts.h"
#include "gfx/gxscene.h"
#include "gfx/rgbacolor.h"

#include "nub/ref.h"

#include "tcl/eventloop.h"

#include "rutz/fstring.h"

#include <tk.h>

#include <limits>

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
  shared_ptr<GlWindowInterface> const glx;
  nub::soft_ref<GLCanvas>        const canvas;
  GxScene*                      const scene;

  Impl(Toglet* p);
  ~Impl() throw()
  {
    if (scene != 0)       scene->destroy();
    if (canvas.is_valid()) canvas->destroy();
  }

  static Window cClassCreateProc(Tk_Window tkwin,
                                 Window parent,
                                 ClientData clientData) throw();

  static Tk_ClassProcs classProcs;
};

Tk_ClassProcs Toglet::Impl::classProcs =
  {
    sizeof(Tk_ClassProcs),
    static_cast<Tk_ClassWorldChangedProc*>(0),
    Toglet::Impl::cClassCreateProc,
    static_cast<Tk_ClassModalProc*>(0),
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
  glx(GlWindowInterface::make(Tk_Display(tkWin), *opts)),
  canvas(GLCanvas::make(opts, glx)),
  scene(new GxScene(canvas))
{
GVX_TRACE("Toglet::Impl::Impl");
}

Window Toglet::Impl::cClassCreateProc(Tk_Window tkwin,
                                      Window parent,
                                      ClientData clientData) throw()
{
GVX_TRACE("Toglet::Impl::cClassCreateProc");
  Toglet* const toglet = static_cast<Toglet*>(clientData);
  GlWindowInterface* const glx = toglet->rep->glx.get();

  GVX_ASSERT(glx != 0);

  return glx->makeTkRealWindow(tkwin, parent, DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
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

  const char* widgetName(nub::uid id)
  {
    static rutz::fstring buf;
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
GVX_TRACE("Toglet::Toglet");

  dbg_eval_nl(3, this);

  // Get the window mapped onscreen. NOTE -- we need to make these calls
  // here, rather than in Impl's constructor. Why? Because some of these
  // calls (e.g. Tk_MapWindow()) may trigger window-system callbacks (such
  // as ConfigureNotify), which will end up calling Toglet's own functions,
  // e.g. reshapeCallback(). Those functions require that "rep" be
  // initialized. If we do this window set up in Impl's constructor, then
  // it won't have returned yet, and so our "rep" variable won't be
  // pointing anywhere meaningful (will be either NULL or garbage).

  Tk_GeometryRequest(rep->tkWin, DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
#ifdef GVX_GL_PLATFORM_GLX
  Tk_SetClassProcs(rep->tkWin, &Impl::classProcs, static_cast<ClientData>(this));
#endif
  Tk_MakeWindowExist(rep->tkWin);
  Tk_MapWindow(rep->tkWin);

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

  if (pack) Tcl::TkWidget::pack();
}

Toglet::~Toglet() throw()
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
  if (theCurrentToglet.id() != this->id())
    {
      rep->glx->makeCurrent(Tk_WindowId(rep->tkWin));
      if (!rep->opts->doubleFlag && rep->canvas->isDoubleBuffered())
        {
          // We requested single buffering but had to accept a double buffered
          // visual.  Set the GL draw buffer to be the front buffer to
          // simulate single buffering.
          rep->canvas->drawBufferFront();
        }
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

  GVX_ASSERT(rep->glx.get() != 0);
  GVX_ASSERT(rep->scene != 0);

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

static const char vcid_groovx_gfx_toglet_cc_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_TOGLET_CC_UTC20050626084024_DEFINED
