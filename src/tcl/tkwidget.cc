///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Thu Nov 21 18:03:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tkwidget.h"

#include "tcl/tclcode.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/ref.h"
#include "util/strings.h"

#include <iostream>
#include <tcl.h>
#include <tk.h>
#include <X11/Xutil.h>

#include "util/trace.h"
#include "util/debug.h"


class DbgButtonListener : public GWT::ButtonListener
{
public:
  static DbgButtonListener* make() { return new DbgButtonListener; }

  virtual GWT::EventStatus onButtonPress(unsigned int button, int x, int y)
  {
    std::cerr << "ButtonPress: "
              << "button " << button
              << " x " << x << " y " << y << std::endl;

    if (button < 3)
      return GWT::NOT_HANDLED;
    return GWT::HANDLED;
  }
};

class DbgKeyListener : public GWT::KeyListener
{
public:
  static DbgKeyListener* make() { return new DbgKeyListener; }

  virtual GWT::EventStatus onKeyPress(const char* keys, int x, int y,
                                      bool controlPressed)
  {
    std::cerr << "KeyPress: "
              << "keys " << keys
              << " control " << controlPressed
              << " x " << x << " y " << y << std::endl;

    return GWT::HANDLED;
  }
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget::TkWidgImpl definition
//
///////////////////////////////////////////////////////////////////////

#define EVENT_MASK ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask

class TkWidgImpl
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(Tcl::TkWidget* o, Tcl_Interp* p,
             const char* classname, const char* pathname);

  ~TkWidgImpl();

  Tcl::TkWidget* owner;
  Tcl_Interp* interp;
  const Tk_Window tkWin;

  int width;
  int height;

  int timeout;
  Tcl_TimerToken timerToken;

  bool updatePending;
  bool shutdownRequested;

  enum EventType { KEY, BUTTON };

  static void buttonEventProc(Tcl::TkWidget* widg, XButtonEvent* eventPtr)
  {
    widg->dispatchButtonEvent(eventPtr->button, eventPtr->x, eventPtr->y);
  }

  static void keyEventProc(Tcl::TkWidget* widg, XKeyEvent* eventPtr)
  {
    char buf[32];

    bool controlPressed = eventPtr->state & ControlMask;
    eventPtr->state &= ~ControlMask;

    int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);

    buf[len] = '\0';

    widg->dispatchKeyEvent(&buf[0], eventPtr->x, eventPtr->y,
                           controlPressed);
  }

  static void cEventCallback(ClientData clientData, XEvent* rawEvent);

  static void cTimerCallback(ClientData clientData) throw();

  static void cRenderCallback(ClientData clientData) throw();

  static void cEventuallyFreeCallback(char* clientData) throw()
  {
    Tcl::TkWidget* widg = reinterpret_cast<Tcl::TkWidget*>(clientData);
    widg->decrRefCount();
  }
};

TkWidgImpl::TkWidgImpl(Tcl::TkWidget* o, Tcl_Interp* p,
                       const char* classname, const char* pathname) :
  owner(o),
  interp(p),
  tkWin(Tk_CreateWindowFromPath(interp, Tk_MainWindow(interp),
                                const_cast<char*>(pathname),
                                (char *) 0)),
  width(400),
  height(400),
  timeout(-1),
  timerToken(0),
  updatePending(false),
  shutdownRequested(false)
{
DOTRACE("TkWidgImpl::TkWidgImpl");

  if (tkWin == 0)
    {
      throw Util::Error("TkWidget constructor couldn't create Tk_Window");
    }

  Tk_SetClass(tkWin, classname);

  Tk_CreateEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(owner));

  if (timeout > 0)
    {
      timerToken =
        Tcl_CreateTimerHandler(timeout, &cTimerCallback,
                               static_cast<ClientData>(owner));
    }
}

TkWidgImpl::~TkWidgImpl()
{
DOTRACE("TkWidgImpl::~TkWidgImpl");

  Tcl_DeleteTimerHandler(timerToken);

  Tk_DeleteEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(owner));

  Tcl_CancelIdleCall(cRenderCallback, static_cast<ClientData>(owner));
  Tk_DestroyWindow(tkWin);
}

void TkWidgImpl::cEventCallback(ClientData clientData, XEvent* rawEvent)
{
DOTRACE("TkWidgImpl::cEventCallback");

  Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

  try
    {
      switch (rawEvent->type)
        {
        case Expose:
          {
            DOTRACE("TkWidgImpl::cEventCallback-Expose");
            if (rawEvent->xexpose.count == 0)
              {
                widg->requestRedisplay();
              }
          }
          break;
        case ConfigureNotify:
          {
            DOTRACE("TkWidgImpl::cEventCallback-ConfigureNotify");
            widg->requestReconfigure();
          }
       break;
        case KeyPress:
          if (widg->hasKeyListeners())
            keyEventProc(widg, (XKeyEvent*) rawEvent);
          break;
        case ButtonPress:
          if (widg->hasButtonListeners())
            buttonEventProc(widg, (XButtonEvent*) rawEvent);
          break;
        case MapNotify:
          {
            DOTRACE("TkWidget::cEventCallback-MapNotify");
          }
          break;
        case DestroyNotify:
          {
            DOTRACE("TkWidget::cEventCallback-DestroyNotify");

            // Idiot-check that we don't have recursive destroy calls
            Assert(!widg->rep->shutdownRequested);

            widg->rep->shutdownRequested = true;

            Tcl_EventuallyFree(static_cast<ClientData>(widg),
                               cEventuallyFreeCallback);
          }
          break;
        }
    }
  catch (...)
    {
      Tcl::Interp(widg->interp()).handleLiveException("cEventCallback", true);
    }
}

void TkWidgImpl::cTimerCallback(ClientData clientData) throw()
{
DOTRACE("TkWidgImpl::cTimerCallback");

  Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

  try
    {
      widg->timerCallback();
      widg->rep->timerToken =
        Tcl_CreateTimerHandler(widg->rep->timeout, cTimerCallback,
                               static_cast<ClientData>(widg));
    }
  catch (...)
    {
      Tcl::Interp(widg->interp()).handleLiveException("cTimerCallback", true);
    }
}

void TkWidgImpl::cRenderCallback(ClientData clientData) throw()
{
DOTRACE("TkWidgImpl::cRenderCallback");

  Tcl::TkWidget* widg = reinterpret_cast<Tcl::TkWidget*>(clientData);

  try
    {
      widg->displayCallback();
      widg->rep->updatePending = false;
    }
  catch (...)
    {
      Tcl::Interp(widg->rep->interp).handleLiveException("cRenderCallback", true);
    }
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget(Tcl_Interp* interp,
                        const char* classname, const char* pathname) :
  rep(new TkWidgImpl(this, interp, classname, pathname))
{
DOTRACE("Tcl::TkWidget::TkWidget");

  incrRefCount();
}

Tcl::TkWidget::~TkWidget()
{
DOTRACE("Tcl::TkWidget::~TkWidget");
  delete rep;
}

int Tcl::TkWidget::width() const { return rep->width; }
int Tcl::TkWidget::height() const { return rep->height; }

void Tcl::TkWidget::setWidth(int w)
{
  rep->width = w;
  Tk_GeometryRequest(rep->tkWin, rep->width, rep->height);
}

void Tcl::TkWidget::setHeight(int h)
{
  rep->height = h;
  Tk_GeometryRequest(rep->tkWin, rep->width, rep->height);
}

int Tcl::TkWidget::timeOut() const
{
  return rep->timeout;
}

void Tcl::TkWidget::setTimeOut(int msec)
{
  Tcl_DeleteTimerHandler(rep->timerToken);

  rep->timeout = msec;

  if (rep->timeout > 0)
    {
      rep->timerToken =
        Tcl_CreateTimerHandler(rep->timeout, &TkWidgImpl::cTimerCallback,
                               static_cast<ClientData>(this));
    }
}

void Tcl::TkWidget::destroyWidget()
{
DOTRACE("Tcl::TkWidget::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(rep->interp) )
    {
      Tk_DestroyWindow(rep->tkWin);
    }
}

Tcl_Interp* Tcl::TkWidget::interp() const
{
  return rep->interp;
}

Tk_Window Tcl::TkWidget::tkWin() const
{
  Assert(rep->tkWin != 0);
  return rep->tkWin;
}

const char* Tcl::TkWidget::pathname() const
{
  Assert(rep->tkWin != 0);
  return Tk_PathName(rep->tkWin);
}

double Tcl::TkWidget::pixelsPerInch() const
{
DOTRACE("Tcl::TkWidget::pixelsPerInch");

  Assert(rep->tkWin != 0);

  Screen* scr = Tk_Screen(rep->tkWin);
  const int screen_pixel_width = XWidthOfScreen(scr);
  const int screen_mm_width = XWidthMMOfScreen(scr);

  const double screen_inch_width = screen_mm_width / 25.4;

  const double screen_ppi = screen_pixel_width / screen_inch_width;

  dbgEvalNL(3, screen_ppi);
  return screen_ppi;
}

void Tcl::TkWidget::pack()
{
DOTRACE("Tcl::TkWidget::pack");

  fstring pack_cmd_str = "pack ";
  pack_cmd_str.append( pathname() );
  pack_cmd_str.append( " -side left -expand 1 -fill both; update" );
  Tcl::Code pack_cmd(Tcl::toTcl(pack_cmd_str), Tcl::Code::THROW_EXCEPTION);
  pack_cmd.invoke(rep->interp);
}

void Tcl::TkWidget::timerCallback()
{
DOTRACE("Tcl::TkWidget::timerCallback");

  std::cout << "timer callback" << std::endl;
}

void Tcl::TkWidget::bind(const char* event_sequence, const char* script)
{
DOTRACE("Tcl::TkWidget::bind");

  fstring cmd_str("bind ", pathname(), " ");
  cmd_str.append( event_sequence, " ");
  cmd_str.append("{ ", script, " }");

  Tcl::Code cmd(Tcl::toTcl(cmd_str), Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(rep->interp);
}

void Tcl::TkWidget::takeFocus()
{
DOTRACE("Tcl::TkWidget::takeFocus");

  fstring cmd_str = "focus -force ";
  cmd_str.append( pathname() );

  Tcl::Code cmd(Tcl::toTcl(cmd_str), Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(rep->interp);
}

void Tcl::TkWidget::requestRedisplay()
{
DOTRACE("Tcl::TkWidget::requestRedisplay");

  if (!rep->updatePending)
    {
      Tk_DoWhenIdle(TkWidgImpl::cRenderCallback, static_cast<ClientData>(this));
      rep->updatePending = true;
    }
}

void Tcl::TkWidget::requestReconfigure()
{
DOTRACE("Tcl::TkWidget::requestReconfigure");

  if (rep->width != Tk_Width(rep->tkWin) ||
      rep->height != Tk_Height(rep->tkWin))
    {
      rep->width = Tk_Width(rep->tkWin);
      rep->height = Tk_Height(rep->tkWin);
      XResizeWindow(Tk_Display(rep->tkWin), Tk_WindowId(rep->tkWin),
                    rep->width, rep->height);
    }

  requestRedisplay();
}

void Tcl::TkWidget::hook()
{
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addKeyListener(Util::Ref<GWT::KeyListener>(DbgKeyListener::make()));
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED