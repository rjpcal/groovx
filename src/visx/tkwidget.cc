///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Tue Sep 17 12:00:29 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "visx/tkwidget.h"

#include "tcl/tclcode.h"
#include "tcl/tclsafeinterp.h"

#include "util/ref.h"
#include "util/strings.h"

#include "visx/xbmaprenderer.h"

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

class TkWidgImpl
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(Tcl::TkWidget* o, Tcl_Interp* p) :
    owner(o),
    interp(p),
    tkWin(0),
    shutdownRequested(false)
  {}

  ~TkWidgImpl()
  {}

  Tcl::TkWidget* owner;
  Tcl_Interp* interp;
  Tk_Window tkWin;

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

  static void cEventuallyFreeCallback(char* clientData) throw()
  {
    Tcl::TkWidget* widg = reinterpret_cast<Tcl::TkWidget*>(clientData);
    widg->decrRefCount();
  }

  static void cEventCallback(ClientData clientData, XEvent* rawEvent);
};


void TkWidgImpl::cEventCallback(ClientData clientData, XEvent* rawEvent)
{
DOTRACE("TkWidgImpl::cEventCallback");

  Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

  try
    {
      switch (rawEvent->type)
        {
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

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget(Tcl_Interp* interp) :
  rep(new TkWidgImpl(this, interp))
{
  incrRefCount();
}

#define EVENT_MASK StructureNotifyMask|KeyPressMask|ButtonPressMask

Tcl::TkWidget::~TkWidget()
{
  Tk_DeleteEventHandler(rep->tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(this));
  delete rep;
}

void Tcl::TkWidget::init(Tk_Window win)
{
  Assert(rep->tkWin == 0);
  rep->tkWin = win;
  XBmapRenderer::initClass(win);

  Tk_CreateEventHandler(rep->tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(this));
}

void Tcl::TkWidget::destroyWidget()
{
DOTRACE("Tcl::TkWidget::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !Tcl_InterpDeleted(rep->interp) )
    {
      fstring destroy_cmd_str = "destroy ";
      destroy_cmd_str.append( pathname() );

      Tcl::Code destroy_cmd(destroy_cmd_str.c_str(),
                            Tcl::Code::BACKGROUND_ERROR);
      destroy_cmd.invoke(rep->interp);
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

  DebugEvalNL(screen_ppi);
  return screen_ppi;
}

void Tcl::TkWidget::pack()
{
DOTRACE("Tcl::TkWidget::pack");

  fstring pack_cmd_str = "pack ";
  pack_cmd_str.append( pathname() );
  pack_cmd_str.append( " -side left -expand 1 -fill both; update" );
  Tcl::Code pack_cmd(pack_cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);
  pack_cmd.invoke(rep->interp);
}

void Tcl::TkWidget::bind(const char* event_sequence, const char* script)
{
DOTRACE("Tcl::TkWidget::bind");

  fstring cmd_str("bind ", pathname(), " ");
  cmd_str.append( event_sequence, " ");
  cmd_str.append("{ ", script, " }");

  Tcl::Code cmd(cmd_str, Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(rep->interp);
}

void Tcl::TkWidget::takeFocus()
{
DOTRACE("Tcl::TkWidget::takeFocus");

  fstring cmd_str = "focus -force ";
  cmd_str.append( pathname() );

  Tcl::Code cmd(cmd_str.c_str(), Tcl::Code::THROW_EXCEPTION);

  cmd.invoke(rep->interp);
}

void Tcl::TkWidget::hook()
{
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addKeyListener(Util::Ref<GWT::KeyListener>(DbgKeyListener::make()));
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
