///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Tue Dec 10 13:25:42 2002
// $Id$
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
                                      ClientData clientData) throw()
{
  Toglet::Impl* rep = static_cast<Toglet::Impl*>(clientData);

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
// Toglet member definitions
//
///////////////////////////////////////////////////////////////////////

Toglet::Toglet(bool pack) :
  Tcl::TkWidget(Tcl::Main::interp(), "Toglet", widgetName(id())),
  rep(new Impl(this))
{
DOTRACE("Toglet::Toglet");

  dbgEvalNL(3, (void*) this);

  if (pack) Tcl::TkWidget::pack();
}

Toglet::~Toglet()
{
DOTRACE("Toglet::~Toglet");

  dbgEvalNL(3, (void*)this);

  delete rep;
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
  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
}

void Toglet::displayCallback()
{
DOTRACE("Toglet::displayCallback");
  fullRender();
}

void Toglet::reshapeCallback(int width, int height)
{
DOTRACE("Toglet::reshapeCallback");
  rep->scene->reshape(width, height);
}

void Toglet::swapBuffers()
{
DOTRACE("Toglet::swapBuffers");
  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
  rep->canvas->flushOutput();
}

GxScene& Toglet::scene()
{
DOTRACE("Toglet::scene");
  return *(rep->scene);
}

void Toglet::render()
{
  rep->scene->render();
}

void Toglet::fullRender()
{
  rep->scene->fullRender();
}

void Toglet::clearscreen()
{
  rep->scene->clearscreen();
}

void Toglet::fullClearscreen()
{
  rep->scene->fullClearscreen();
}

void Toglet::undraw()
{
  rep->scene->undraw();
}

void Toglet::setVisibility(bool vis)
{
  rep->scene->setVisibility(vis);
}

void Toglet::setHold(bool hold_on)
{
  rep->scene->setHold(hold_on);
}

void Toglet::allowRefresh(bool allow)
{
  rep->scene->allowRefresh(allow);
}

const Util::Ref<GxCamera>& Toglet::getCamera() const
{
  return rep->scene->getCamera();
}

void Toglet::setCamera(const Util::Ref<GxCamera>& cam)
{
  rep->scene->setCamera(cam);
}

void Toglet::setDrawable(const Ref<GxNode>& node)
{
  rep->scene->setDrawable(node);
}

void Toglet::animate(unsigned int framesPerSecond)
{
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
