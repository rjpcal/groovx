///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Fri May 16 08:34:03 2003
// $Id$
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

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tkwidget.h"

#include "gx/vec2.h"

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


///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget::TkWidgImpl definition
//
///////////////////////////////////////////////////////////////////////

#define EVENT_MASK ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask

class TkWidgImpl : public Util::VolatileObject
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(Tcl::TkWidget* o, Tcl::Interp& p,
             const char* classname,
             const char* pathname,
             bool topLevel);

  ~TkWidgImpl();

  Tcl::TkWidget* owner;
  Tcl::Interp interp;
  const Tk_Window tkWin;

  int width;
  int height;

  bool updatePending;
  bool shutdownRequested;

  void onButtonPress(const GWT::ButtonPressEvent& ev)
  {
    std::cerr << "ButtonPress: "
              << "button " << ev.button
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  void onKeyPress(const GWT::KeyPressEvent& ev)
  {
    std::cerr << "KeyPress: "
              << "keys " << ev.keys
              << " control " << ev.controlPressed
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  static void buttonEventProc(Tcl::TkWidget* widg, XButtonEvent* eventPtr)
  {
    const bool controlPressed = eventPtr->state & ControlMask;
    const bool shiftPressed = eventPtr->state & ShiftMask;

    // This is an escape hatch for top-level frameless windows gone
    // awry... need to always provide a reliable way to iconify the window
    // since the title bar and "minimize" button might not exist.
    if (controlPressed && shiftPressed && eventPtr->button == 3)
      {
        widg->destroyWidget();
      }

    GWT::ButtonPressEvent ev = {eventPtr->button, eventPtr->x, eventPtr->y};
    widg->sigButtonPressed.emit(ev);
  }

  static void keyEventProc(Tcl::TkWidget* widg, XKeyEvent* eventPtr)
  {
    char buf[32];

    const bool controlPressed = eventPtr->state & ControlMask;
    eventPtr->state &= ~ControlMask;

    const int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);

    buf[len] = '\0';

    GWT::KeyPressEvent ev = {&buf[0], eventPtr->x, eventPtr->y, controlPressed};
    widg->sigKeyPressed.emit(ev);
  }

  static void cEventCallback(ClientData clientData, XEvent* rawEvent);

  static void cRenderCallback(ClientData clientData) throw();

  static void cEventuallyFreeCallback(char* clientData) throw()
  {
    Tcl::TkWidget* widg = reinterpret_cast<Tcl::TkWidget*>(clientData);
    widg->decrRefCount();
  }
};

TkWidgImpl::TkWidgImpl(Tcl::TkWidget* o, Tcl::Interp& p,
                       const char* classname,
                       const char* pathname,
                       bool topLevel) :
  owner(o),
  interp(p),
  tkWin(Tk_CreateWindowFromPath(interp.intp(),
                                Tk_MainWindow(interp.intp()),
                                const_cast<char*>(pathname),
                                topLevel ? (char*) "" : (char*) 0)),
  width(400),
  height(400),
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
}

TkWidgImpl::~TkWidgImpl()
{
DOTRACE("TkWidgImpl::~TkWidgImpl");

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
  TkWidgImpl* rep = widg->rep;

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

            if (rep->width != Tk_Width(rep->tkWin) ||
                rep->height != Tk_Height(rep->tkWin))
              {
                rep->width = Tk_Width(rep->tkWin);
                rep->height = Tk_Height(rep->tkWin);
                XResizeWindow(Tk_Display(rep->tkWin), Tk_WindowId(rep->tkWin),
                              rep->width, rep->height);
              }

            widg->reshapeCallback(rep->width, rep->height);
            widg->requestRedisplay();
          }
       break;
        case KeyPress:
          keyEventProc(widg, (XKeyEvent*) rawEvent);
          break;
        case ButtonPress:
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
      widg->rep->interp.handleLiveException("cEventCallback", true);
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
      widg->rep->interp.handleLiveException("cRenderCallback", true);
    }
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget(Tcl::Interp& interp,
                        const char* classname,
                        const char* pathname,
                        bool topLevel) :
  rep(new TkWidgImpl(this, interp, classname, pathname, topLevel))
{
DOTRACE("Tcl::TkWidget::TkWidget");
}

Tcl::TkWidget::~TkWidget()
{
DOTRACE("Tcl::TkWidget::~TkWidget");
  delete rep;
}

int Tcl::TkWidget::width() const { return rep->width; }
int Tcl::TkWidget::height() const { return rep->height; }

Gfx::Vec2<int> Tcl::TkWidget::size() const
{ return Gfx::Vec2<int>(width(), height()); }

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

void Tcl::TkWidget::destroyWidget()
{
DOTRACE("Tcl::TkWidget::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !rep->interp.interpDeleted() )
    {
      Tk_DestroyWindow(rep->tkWin);
    }
}

Tcl::Interp& Tcl::TkWidget::interp() const
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

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      fstring pack_cmd = "pack ";
      pack_cmd.append( pathname() );
      pack_cmd.append( " -side left -expand 1 -fill both; update" );
      rep->interp.eval(pack_cmd);
    }
}

void Tcl::TkWidget::iconify()
{
DOTRACE("Tcl::TkWidget::iconify");

  XIconifyWindow(Tk_Display(rep->tkWin),
                 Tk_WindowId(rep->tkWin),
                 Tk_ScreenNumber(rep->tkWin));
}

void Tcl::TkWidget::bind(const char* event_sequence, const char* script)
{
DOTRACE("Tcl::TkWidget::bind");

  fstring cmd_str("bind ", pathname(), " ");
  cmd_str.append( event_sequence, " ");
  cmd_str.append("{ ", script, " }");

  rep->interp.eval(cmd_str);
}

void Tcl::TkWidget::takeFocus()
{
DOTRACE("Tcl::TkWidget::takeFocus");

  fstring cmd("focus -force ", pathname());

  rep->interp.eval(cmd);
}

void Tcl::TkWidget::requestRedisplay()
{
DOTRACE("Tcl::TkWidget::requestRedisplay");

  if (!rep->updatePending)
    {
      Tcl_DoWhenIdle(TkWidgImpl::cRenderCallback, static_cast<ClientData>(this));
      rep->updatePending = true;
    }
}

void Tcl::TkWidget::hook()
{
  sigButtonPressed.connect(rep, &TkWidgImpl::onButtonPress);
  sigButtonPressed.connect(rep, &TkWidgImpl::onButtonPress);
  sigKeyPressed.connect(rep, &TkWidgImpl::onKeyPress);
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
