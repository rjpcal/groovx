///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 15 17:05:12 2001
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

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tkwidget.h"

#include "geom/vec2.h"

#include "nub/ref.h"

#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/fstring.h"

#include <cstdio>
#include <iostream>
#include <tcl.h>
#include <tk.h>
#include <X11/Xutil.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::fstring;

namespace WindowSystem
{
  void winInfo(Tk_Window tkWin);
  void iconify(Tk_Window tkWin);
  void grabKeyboard(Tk_Window tkWin);
  void ungrabKeyboard(Tk_Window tkWin);
}

#if defined(GL_PLATFORM_GLX)

void WindowSystem::winInfo(Tk_Window tkWin)
{
DOTRACE("WindowSystem::winInfo[glx]");
  Display* dpy = Tk_Display(tkWin);

  int natoms = 0;
  Atom* atoms = XListProperties(dpy, Tk_WindowId(tkWin),
                                &natoms);

  fprintf(stderr, "%d atoms\n", natoms);

  for (int i = 0; i < natoms; ++i)
    {
      const char* name = XGetAtomName(dpy, atoms[i]);

      fprintf(stderr, "[%d] %s\n", i, name);

      long long_offset = 0;
      long long_length = 16;

      Atom actual_type_return = 0;
      int actual_format_return = 0;
      unsigned long nitems_return = 0;
      unsigned long bytes_after_return = 0;
      unsigned char* prop_return = 0;

      XGetWindowProperty(dpy, Tk_WindowId(tkWin),
                         atoms[i],
                         long_offset,
                         long_length,
                         False,
                         AnyPropertyType,
                         &actual_type_return,
                         &actual_format_return,
                         &nitems_return,
                         &bytes_after_return,
                         &prop_return);

      XFree(prop_return);
    }

  XFree(atoms);
}

void WindowSystem::iconify(Tk_Window tkWin)
{
DOTRACE("WindowSystem::iconify[glx]");
  XIconifyWindow(Tk_Display(tkWin),
                 Tk_WindowId(tkWin),
                 Tk_ScreenNumber(tkWin));
}

void WindowSystem::grabKeyboard(Tk_Window tkWin)
{
DOTRACE("WindowSystem::grabKeyboard[glx]");
  const int result =
    XGrabKeyboard(Tk_Display(tkWin),
                  Tk_WindowId(tkWin),
                  False /* don't send key events to their normal windows */,
                  GrabModeAsync /* pointer mode */,
                  GrabModeAsync /* keyboard mode */,
                  CurrentTime /* when grab should take place */);

  switch (result)
    {
    case AlreadyGrabbed:
      throw rutz::error("couldn't grab keyboard: keyboard already grabbed", SRC_POS);
    case GrabNotViewable:
      throw rutz::error("couldn't grab keyboard: grab window not viewable", SRC_POS);
    case GrabInvalidTime:
      throw rutz::error("couldn't grab keyboard: grab time invalid", SRC_POS);
    case GrabFrozen:
      throw rutz::error("couldn't grab keyboard: pointer already frozen", SRC_POS);
    }
}

void WindowSystem::ungrabKeyboard(Tk_Window tkWin)
{
DOTRACE("WindowSystem::ungrabKeyboard[glx]");
  XUngrabKeyboard(Tk_Display(tkWin), CurrentTime);
}

#elif defined(GL_PLATFORM_AGL)

void WindowSystem::winInfo(Tk_Window /*tkWin*/)
{
DOTRACE("WindowSystem::winInfo[agl]");
  // FIXME
  throw rutz::error("WindowSystem::winInfo not supported", SRC_POS);
}

void WindowSystem::iconify(Tk_Window /*tkWin*/)
{
DOTRACE("WindowSystem::iconify[agl]");
  // FIXME
  throw rutz::error("WindowSystem::iconify not supported", SRC_POS);
}

void WindowSystem::grabKeyboard(Tk_Window /*tkWin*/)
{
DOTRACE("WindowSystem::grabKeyboard[agl]");
  // don't need to do anything to grab keyboard with Mac OS X Aqua
}

void WindowSystem::ungrabKeyboard(Tk_Window /*tkWin*/)
{
DOTRACE("WindowSystem::ungrabKeyboard[agl]");
  // don't need to do anything to (un)grab keyboard with Mac OS X Aqua
}

#endif

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget::TkWidgImpl definition
//
///////////////////////////////////////////////////////////////////////

namespace
{
  unsigned long EVENT_MASK = (ExposureMask |
                              StructureNotifyMask |
                              KeyPressMask |
                              ButtonPressMask |
                              EnterWindowMask |
                              LeaveWindowMask);
}

class TkWidgImpl : public Nub::VolatileObject
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(Tcl::TkWidget* o, Tcl::Interp& p,
             const char* classname,
             const char* pathname,
             bool topLevel);

  virtual ~TkWidgImpl() throw();

  Tcl::TkWidget* owner;
  Tcl::Interp interp;
  const Tk_Window tkWin;
  Tk_Cursor cursor;

  bool updatePending;
  bool shutdownRequested;

  void dbgButtonPress(const Tcl::ButtonPressEvent& ev)
  {
    std::cerr << "ButtonPress: "
              << "button " << ev.button
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  void dbgKeyPress(const Tcl::KeyPressEvent& ev)
  {
    std::cerr << "KeyPress: "
              << "keys '" << ev.keys << "'"
              << " control " << ev.controlPressed
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  void buttonEventProc(XButtonEvent* eventPtr);

  void keyEventProc(XKeyEvent* eventPtr);

  static void cEventCallback(ClientData clientData, XEvent* rawEvent) throw();

  static void cRenderCallback(ClientData clientData) throw();

  static void cEventuallyFreeCallback(char* clientData) throw();

  static void cTakeFocusCallback(ClientData clientData) throw();
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
  cursor(0),
  updatePending(false),
  shutdownRequested(false)
{
DOTRACE("TkWidgImpl::TkWidgImpl");

  if (tkWin == 0)
    {
      throw rutz::error("TkWidget couldn't create Tk_Window", SRC_POS);
    }

  Tk_SetClass(tkWin, classname);

  Tk_CreateEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(owner));
}

TkWidgImpl::~TkWidgImpl() throw()
{
DOTRACE("TkWidgImpl::~TkWidgImpl");

  if (cursor != 0)
    Tk_FreeCursor(Tk_Display(tkWin), cursor);

  Tk_DeleteEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        static_cast<void*>(owner));

  Tcl_CancelIdleCall(cRenderCallback, static_cast<ClientData>(owner));
  Tk_DestroyWindow(tkWin);
}

void TkWidgImpl::buttonEventProc(XButtonEvent* eventPtr)
{
DOTRACE("TkWidgImpl::buttonEventProc");

  const bool controlPressed = eventPtr->state & ControlMask;
  const bool shiftPressed = eventPtr->state & ShiftMask;

  // This is an escape hatch for top-level frameless windows gone
  // awry... need to always provide a reliable way to iconify the window
  // since the title bar and "minimize" button might not exist.
  if (controlPressed && shiftPressed)
    {
      switch (eventPtr->button)
        {
        case 1: owner->minimize(); return;
        case 3: owner->destroyWidget(); return;
        }
    }

  Tcl::ButtonPressEvent ev = {eventPtr->button, eventPtr->x, eventPtr->y};
  owner->sigButtonPressed.emit(ev);
}

void TkWidgImpl::keyEventProc(XKeyEvent* eventPtr)
{
DOTRACE("TkWidgImpl::keyEventProc");

  const bool controlPressed = eventPtr->state & ControlMask;

  // Need to save and later restore the event "state" in order that
  // subsequent processing of this event (e.g. processing by Tk "bind"
  // tables") proceeds correctly.
  const unsigned int saveState = eventPtr->state;

  // Turn off (temporarily) any ControlMask so that we get a more sensible
  // output from XLookupString().
  eventPtr->state &= ~ControlMask;

  char buf[32];
#if defined(GL_PLATFORM_GLX)
  const int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);
  buf[len] = '\0';
#elif defined(GL_PLATFORM_AGL)
  strncpy(buf, eventPtr->trans_chars, 31);
  buf[31] = '\0';
#endif

  // Restore the state
  eventPtr->state = saveState;

  Tcl::KeyPressEvent ev = {&buf[0], eventPtr->x, eventPtr->y, controlPressed};
  owner->sigKeyPressed.emit(ev);
}

void TkWidgImpl::cEventCallback(ClientData clientData, XEvent* rawEvent) throw()
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

            widg->reshapeCallback(Tk_Width(rep->tkWin),
                                  Tk_Height(rep->tkWin));
            widg->requestRedisplay();
          }
          break;
        case EnterNotify:
          widg->grabKeyboard();
          break;
        case LeaveNotify:
          widg->ungrabKeyboard();
          break;
        case KeyPress:
          rep->keyEventProc(reinterpret_cast<XKeyEvent*>(rawEvent));
          break;
        case ButtonPress:
          rep->buttonEventProc(reinterpret_cast<XButtonEvent*>(rawEvent));
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
            ASSERT(!widg->rep->shutdownRequested);

            widg->rep->shutdownRequested = true;

            Tcl_EventuallyFree(static_cast<ClientData>(widg),
                               cEventuallyFreeCallback);
          }
          break;
        }
    }
  catch (...)
    {
      widg->rep->interp.handleLiveException("cEventCallback", SRC_POS);
      widg->rep->interp.backgroundError();
    }
}

void TkWidgImpl::cRenderCallback(ClientData clientData) throw()
{
DOTRACE("TkWidgImpl::cRenderCallback");

  Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

  try
    {
      widg->displayCallback();
      widg->rep->updatePending = false;
    }
  catch (...)
    {
      widg->rep->interp.handleLiveException("cRenderCallback", SRC_POS);
      widg->rep->interp.backgroundError();
    }
}

void TkWidgImpl::cEventuallyFreeCallback(char* clientData) throw()
{
DOTRACE("TkWidgImpl::cEventuallyFreeCallback");
  Tcl::TkWidget* widg = reinterpret_cast<Tcl::TkWidget*>(clientData);
  delete widg;
}

void TkWidgImpl::cTakeFocusCallback(ClientData clientData) throw()
{
DOTRACE("TkWidgImpl::cTakeFocusCallback");
  Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);
  try
    {
      widg->takeFocus();
    }
  catch (...)
    {
      widg->rep->interp.handleLiveException("cEventCallback", SRC_POS);
      widg->rep->interp.backgroundError();
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

  this->markAsVolatile();
}

Tcl::TkWidget::~TkWidget() throw()
{
DOTRACE("Tcl::TkWidget::~TkWidget");
  delete rep;
}

int Tcl::TkWidget::width() const
{
DOTRACE("Tcl::TkWidget::width");
  return Tk_Width(rep->tkWin);
}

int Tcl::TkWidget::height() const
{
DOTRACE("Tcl::TkWidget::height");
  return Tk_Height(rep->tkWin);
}

geom::vec2<int> Tcl::TkWidget::size() const
{
  return geom::vec2<int>(width(), height());
}

void Tcl::TkWidget::setWidth(int w)
{
DOTRACE("Tcl::TkWidget::setWidth");
  // Need to specify Tk_ReqHeight(rep->tkWin) instead of
  // Tk_Height(rep->tkWin), since the latter might not reflect the
  // requested height if the event loop has not been entered since a
  // call to setHeight().
  Tk_GeometryRequest(rep->tkWin, w, Tk_ReqHeight(rep->tkWin));
}

void Tcl::TkWidget::setHeight(int h)
{
DOTRACE("Tcl::TkWidget::setHeight");
  // Need to specify Tk_ReqWidth(rep->tkWin) instead of
  // Tk_Width(rep->tkWin), since the latter might not reflect the
  // requested height if the event loop has not been entered since a
  // call to setWidth().
  Tk_GeometryRequest(rep->tkWin, Tk_ReqWidth(rep->tkWin), h);
}

void Tcl::TkWidget::setSize(geom::vec2<int> sz)
{
DOTRACE("Tcl::TkWidget::setSize");
  Tk_GeometryRequest(rep->tkWin, sz.x(), sz.y());
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

void Tcl::TkWidget::winInfo() throw()
{
DOTRACE("Tcl::TkWidget::winInfo");

  WindowSystem::winInfo(rep->tkWin);
}

Tcl::Interp& Tcl::TkWidget::interp() const
{
  return rep->interp;
}

Tk_Window Tcl::TkWidget::tkWin() const
{
  ASSERT(rep->tkWin != 0);
  return rep->tkWin;
}

const char* Tcl::TkWidget::pathname() const
{
  ASSERT(rep->tkWin != 0);
  return Tk_PathName(rep->tkWin);
}

double Tcl::TkWidget::pixelsPerInch() const
{
DOTRACE("Tcl::TkWidget::pixelsPerInch");

  ASSERT(rep->tkWin != 0);

  Screen* scr = Tk_Screen(rep->tkWin);
  const int screen_pixel_width = WidthOfScreen(scr);
  const int screen_mm_width = WidthMMOfScreen(scr);

  const double screen_inch_width = screen_mm_width / 25.4;

  const double screen_ppi = screen_pixel_width / screen_inch_width;

  dbg_eval_nl(3, screen_ppi);
  return screen_ppi;
}

void Tcl::TkWidget::setCursor(const char* cursor_spec)
{
DOTRACE("Tcl::TkWidget::setCursor");
  if (cursor_spec == 0 || cursor_spec == '\0')
    {
      // Empty string means to unset the cursor
      if (rep->cursor != 0)
        Tk_FreeCursor(Tk_Display(rep->tkWin), rep->cursor);

      rep->cursor = 0;

      Tk_UndefineCursor(rep->tkWin);
    }
  else
    {
      Tk_Cursor new_cursor = Tk_GetCursor(rep->interp.intp(), rep->tkWin,
                                          cursor_spec);

      if (new_cursor == 0)
        {
          throw rutz::error(fstring("couldn't set cursor to '",
                                    cursor_spec, "'"), SRC_POS);
        }

      // OK, creating the new cursor succeeded, now free the old one
      if (rep->cursor != 0)
        Tk_FreeCursor(Tk_Display(rep->tkWin), rep->cursor);

      rep->cursor = new_cursor;

      Tk_DefineCursor(rep->tkWin, rep->cursor);
    }
}

const char* Tcl::TkWidget::getCursor() const
{
DOTRACE("Tcl::TkWidget::getCursor");
  return rep->cursor == 0
    ? ""
    : Tk_NameOfCursor(Tk_Display(rep->tkWin), rep->cursor);
}

void Tcl::TkWidget::warpPointer(int x, int y) const
{
DOTRACE("Tcl::TkWidget::warpPointer");

  // NOTE: The XWarpPointer emulation routine provided by Tk for Mac OS X
  // Aqua currently does nothing (as of Tk 8.5a1).
  XWarpPointer(Tk_Display(rep->tkWin),
               0, Tk_WindowId(rep->tkWin),
               0, 0, 0, 0,
               x, y);
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

void Tcl::TkWidget::repack(const char* options)
{
DOTRACE("Tcl::TkWidget::repack");

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      const fstring pack_cmd("pack ", pathname(), " ", options, "; update");
      rep->interp.eval(pack_cmd);
    }
}

void Tcl::TkWidget::unpack()
{
DOTRACE("Tcl::TkWidget::unpack");

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      const fstring unpack_cmd("pack forget ", pathname());
      rep->interp.eval(unpack_cmd);
    }
}

void Tcl::TkWidget::iconify()
{
DOTRACE("Tcl::TkWidget::iconify");

  WindowSystem::iconify(rep->tkWin);
}

void Tcl::TkWidget::grabKeyboard()
{
DOTRACE("Tcl::TkWidget::grabKeyboard");

  WindowSystem::grabKeyboard(rep->tkWin);

  // Oddly enough, we can't just call takeFocus() directly here. In
  // particular, we run into problems if grabKeyboard() is called from an
  // EnterNotify callback. In that case, if we call takeFocus() during that
  // callback, Tk somehow seems to erase our focus request by the time
  // control returns back to the main event loop. So, if we make the call
  // as an idle callback instead, then we don't switch focus until after Tk
  // is completely finished processing the EnterNotify event.
  Tcl_DoWhenIdle(TkWidgImpl::cTakeFocusCallback,
                 static_cast<ClientData>(this));
}

void Tcl::TkWidget::ungrabKeyboard()
{
DOTRACE("Tcl::TkWidget::ungrabKeyboard");

  WindowSystem::ungrabKeyboard(rep->tkWin);
}

void Tcl::TkWidget::maximize()
{
DOTRACE("Tcl::TkWidget::maximize");

  const int w = WidthOfScreen(Tk_Screen(rep->tkWin));
  const int h = HeightOfScreen(Tk_Screen(rep->tkWin));
  setSize(geom::vec2<int>(w, h));

  Tk_Window mainWin = rep->tkWin;
  while ( !Tk_IsTopLevel(mainWin) )
    {
      mainWin = Tk_Parent(mainWin);
    }

  Tk_MoveToplevelWindow(mainWin, 0, 0);

  grabKeyboard();
}

void Tcl::TkWidget::minimize()
{
DOTRACE("Tcl::TkWidget::minimize");

  ungrabKeyboard();

  setSize(geom::vec2<int>(200, 200));
}

void Tcl::TkWidget::bind(const fstring& event_sequence, const fstring& script)
{
DOTRACE("Tcl::TkWidget::bind");

  fstring cmd_str("bind ", pathname(), " ");
  cmd_str.append( event_sequence, " ");

  if (script.length() == 0)
    {
      // If the script is the empty string, then we want to destroy the
      // binding, so we need to actually give an empty string to the "bind"
      // command; any empty pair of braces "{  }" will not suffice.
      cmd_str.append( "\"\"" );
    }
  else
    {
      cmd_str.append("{ ", script, " }");
    }

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
  sigButtonPressed.connect(rep, &TkWidgImpl::dbgButtonPress);
  sigKeyPressed.connect(rep, &TkWidgImpl::dbgKeyPress);
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
