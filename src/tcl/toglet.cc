///////////////////////////////////////////////////////////////////////
//
// toglet.cc
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Sat Nov 23 14:41:46 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLET_CC_DEFINED
#define TOGLET_CC_DEFINED

#include "toglet.h"

#include "gfx/glcanvas.h"
#include "gfx/gxcamera.h"
#include "gfx/gxemptynode.h"
#include "gfx/gxnode.h"

#include "gx/rgbacolor.h"

#include "tcl/tclmain.h"
#include "tcl/tcltimer.h"

#include "util/ref.h"
#include "util/strings.h"
#include "util/volatileobject.h"

#include <tk.h>
#include <X11/Xlib.h>

#ifdef HAVE_LIMITS
#  include <limits>
#else
#  include <climits>
#endif

#include "util/trace.h"
#include "util/debug.h"

class Scene : public Util::VolatileObject
{
public:
  Scene(Toglet* owner) :
    itsDrawNode(GxEmptyNode::make()),
    itsUndrawNode(GxEmptyNode::make()),
    isItVisible(false),
    itsCamera(new GxFixedScaleCamera()),
    isItHolding(false),
    isItRefreshing(true),
    isItRefreshed(false),
    itsTimer(100, true),
    slotNodeChanged(Util::Slot::make(owner, &Toglet::onNodeChange))
  {
    itsTimer.sigTimeOut.connect(owner, &Toglet::fullRender);
    itsCamera->sigNodeChanged.connect(slotNodeChanged);
  }

  static Scene* make(Toglet* owner) { return new Scene(owner); }

  void undraw(Gfx::Canvas& canvas);

  void render(Gfx::Canvas& canvas, int width, int height);

  void fullRender(Gfx::Canvas& canvas, int width, int height);

  void clearscreen(Gfx::Canvas& canvas)
  {
    canvas.clearColorBuffer();
    setDrawable(Util::Ref<GxNode>(GxEmptyNode::make()));
    itsUndrawNode = Util::Ref<GxNode>(GxEmptyNode::make());
    isItVisible = false;
  }

  void fullClearscreen(Gfx::Canvas& canvas)
  {
    clearscreen(canvas);
    doFlush(canvas);
  }

  void setVisibility(Gfx::Canvas& canvas, bool val)
  {
    isItVisible = val;
    if ( !isItVisible )
      {
        fullClearscreen(canvas);
      }
  }

  void setCamera(const Ref<GxCamera>& cam)
  {
    itsCamera->sigNodeChanged.disconnect(slotNodeChanged);

    itsCamera = cam;

    itsCamera->sigNodeChanged.connect(slotNodeChanged);
  }

  void setDrawable(const Ref<GxNode>& node)
  {
    itsDrawNode->sigNodeChanged.disconnect(slotNodeChanged);

    itsDrawNode = node;

    itsDrawNode->sigNodeChanged.connect(slotNodeChanged);
  }

  void flushChanges(Gfx::Canvas& canvas, int width, int height)
  {
    if (isItRefreshing && !isItRefreshed)
      fullRender(canvas, width, height);
  }

  void onNodeChange(Gfx::Canvas& canvas, int width, int height)
  {
    isItRefreshed = false;
    flushChanges(canvas, width, height);
  }

private:
  void doFlush(Gfx::Canvas& canvas)
  {
    canvas.flushOutput();
  }

  Scene(const Scene&);
  Scene& operator=(const Scene&);

  Util::Ref<GxNode> itsDrawNode;
  Util::Ref<GxNode> itsUndrawNode;
  bool isItVisible;

public:
  Util::Ref<GxCamera> itsCamera;
  bool isItHolding;
  bool isItRefreshing;
  bool isItRefreshed;

  Tcl::Timer itsTimer;

  Util::Ref<Util::Slot> slotNodeChanged;
};

///////////////////////////////////////////////////////////////////////
//
// Scene member definitions
//
///////////////////////////////////////////////////////////////////////

void Scene::render(Gfx::Canvas& canvas, int width, int height)
{
DOTRACE("Scene::render");

  try
    {
      Gfx::MatrixSaver msaver(canvas);
      Gfx::AttribSaver asaver(canvas);

      itsCamera->reshape(width, height);
      itsCamera->draw(canvas);
      itsDrawNode->draw(canvas);
      itsUndrawNode = itsDrawNode;

      isItRefreshed = true;
    }
  catch (...)
    {
      // Here, something failed during rendering, so just go invisible
      setVisibility(canvas, false);
    }
}

void Scene::fullRender(Gfx::Canvas& canvas, int width, int height)
{
DOTRACE("Scene::fullRender");

  // (1) Clear the screen (but only if we are not "holding")
  if( !isItHolding )
    {
      canvas.clearColorBuffer();
    }

  // (2) Render the current object
  if ( isItVisible )
    {
      render(canvas, width, height);
    }

  // (3) Flush the graphics stream
  doFlush(canvas);
}

void Scene::undraw(Gfx::Canvas& canvas)
{
DOTRACE("Scene::undraw");
  itsUndrawNode->undraw(canvas);
  doFlush(canvas);
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

  Impl(Toglet* p);
  ~Impl() throw() { canvas->destroy(); }

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
  canvas(GLCanvas::make(Tk_Display(tkWin)))
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
  rep(new Impl(this)),
  itsScene(Scene::make(this))
{
DOTRACE("Toglet::Toglet");

  dbgEvalNL(3, (void*) this);

  if (pack) Tcl::TkWidget::pack();
}

Toglet::~Toglet()
{
DOTRACE("Toglet::~Toglet");

  dbgEvalNL(3, (void*)this);

  itsScene->destroy();
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

void Toglet::swapBuffers()
{
DOTRACE("Toglet::swapBuffers");
  rep->canvas->makeCurrent(Tk_WindowId(rep->tkWin));
  rep->canvas->flushOutput();
}

void Toglet::render()
{
  itsScene->render(getCanvas(), width(), height());
}

void Toglet::fullRender()
{
  itsScene->fullRender(getCanvas(), width(), height());
}

void Toglet::clearscreen()
{
  itsScene->clearscreen(getCanvas());
}

void Toglet::fullClearscreen()
{
  itsScene->fullClearscreen(getCanvas());
}

void Toglet::undraw()
{
  itsScene->undraw(getCanvas());
}

void Toglet::setVisibility(bool vis)
{
DOTRACE("Toglet::setVisibility");
  itsScene->setVisibility(getCanvas(), vis);
}

void Toglet::setHold(bool hold_on)
{
DOTRACE("Toglet::setHold");
  itsScene->isItHolding = hold_on;
}

void Toglet::allowRefresh(bool allow)
{
DOTRACE("Toglet::allowRefresh");
  itsScene->isItRefreshing = allow;
  itsScene->flushChanges(getCanvas(), width(), height());
}

const Util::Ref<GxCamera>& Toglet::getCamera() const
{
DOTRACE("Toglet::getCamera");

  return itsScene->itsCamera;
}

void Toglet::setCamera(const Util::Ref<GxCamera>& cam)
{
DOTRACE("Toglet::setCamera");

  itsScene->setCamera(cam);
  fullRender();
}

void Toglet::setDrawable(const Ref<GxNode>& node)
{
DOTRACE("Toglet::setDrawable");
  itsScene->setDrawable(node);
}

void Toglet::animate(unsigned int framesPerSecond)
{
DOTRACE("Toglet::animate");
  if (framesPerSecond == 0)
    {
      itsScene->itsTimer.cancel();
    }
  else
    {
      itsScene->itsTimer.setDelayMsec(1000/framesPerSecond);
      itsScene->itsTimer.schedule();
    }
}

void Toglet::onNodeChange()
{
  itsScene->onNodeChange(getCanvas(), width(), height());
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
