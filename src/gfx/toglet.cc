///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan  4 08:00:00 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/glwindowinterface.h"
#include "gfx/glxopts.h"
#include "gfx/gxscene.h"
#include "gfx/rgbacolor.h"

#include "nub/ref.h"

#include "tcl/tclmain.h"

#include "util/fstring.h"

#include <tk.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::shared_ptr;

namespace
{
  Nub::SoftRef<Toglet> theCurrentToglet;

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
  Nub::SoftRef<GLCanvas>        const canvas;
  GxScene*                      const scene;

  Impl(Toglet* p);
  ~Impl() throw()
  {
    if (scene != 0)       scene->destroy();
    if (canvas.isValid()) canvas->destroy();
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
DOTRACE("Toglet::Impl::Impl");
}

Window Toglet::Impl::cClassCreateProc(Tk_Window tkwin,
                                      Window parent,
                                      ClientData clientData) throw()
{
DOTRACE("Toglet::Impl::cClassCreateProc");
  Toglet* const toglet = static_cast<Toglet*>(clientData);
  GlWindowInterface* const glx = toglet->rep->glx.get();

  ASSERT(glx != 0);

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

  const char* widgetName(Nub::UID id)
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
DOTRACE("Toglet::Toglet");

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
#ifdef GL_PLATFORM_GLX
  Tk_SetClassProcs(rep->tkWin, &Impl::classProcs, static_cast<ClientData>(this));
#endif
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

  dbg_eval_nl(3, this);

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

Nub::SoftRef<Toglet> Toglet::getCurrent()
{
DOTRACE("Toglet::getCurrent");
  return theCurrentToglet;
}

void Toglet::setCurrent(Nub::SoftRef<Toglet> toglet)
{
DOTRACE("Toglet::setCurrent");
  dbg_eval(1, toglet.id());
  toglet->makeCurrent();
}

void Toglet::defaultParent(const char* pathname)
{
DOTRACE("Toglet::defaultParent");
  PARENT = pathname;
}

Nub::SoftRef<Gfx::Canvas> Toglet::getCanvas() const
{
DOTRACE("Toglet::getCanvas");
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
      theCurrentToglet = Nub::SoftRef<Toglet>(const_cast<Toglet*>(this));
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

  ASSERT(rep->glx.get() != 0);
  ASSERT(rep->scene != 0);

  makeCurrent();
  rep->glx->onReshape(width, height);
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

const Nub::Ref<GxCamera>& Toglet::getCamera() const
{
  makeCurrent();
  return rep->scene->getCamera();
}

void Toglet::setCamera(const Nub::Ref<GxCamera>& cam)
{
  makeCurrent();
  rep->scene->setCamera(cam);
}

void Toglet::setDrawable(const Nub::Ref<GxNode>& node)
{
  makeCurrent();
  rep->scene->setDrawable(node);
}

void Toglet::animate(unsigned int framesPerSecond)
{
  makeCurrent();
  rep->scene->animate(framesPerSecond);
}

static const char vcid_toglet_cc[] = "$Id$ $URL$";
#endif // !TOGLET_CC_DEFINED
