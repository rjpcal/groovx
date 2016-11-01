/** @file tk/widget.cc C++ wrapper class for Tk widgets */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 15 17:05:12 2001
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

#include "tk/widget.h"

#include "geom/vec2.h"

#include "nub/ref.h"

#include "tcl/interp.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <cstdio>
#include <iostream>
#include <tcl.h>
#include <tk.h>
#include <X11/Xutil.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

namespace WindowSystem
{
  void winInfo(Tk_Window tkWin);
  void iconify(Tk_Window tkWin);
  void grabKeyboard(Tk_Window tkWin);
  void ungrabKeyboard(Tk_Window tkWin);
}

#if defined(GVX_GL_PLATFORM_GLX)

void WindowSystem::winInfo(Tk_Window tkWin)
{
GVX_TRACE("WindowSystem::winInfo[glx]");
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
GVX_TRACE("WindowSystem::iconify[glx]");
  XIconifyWindow(Tk_Display(tkWin),
                 Tk_WindowId(tkWin),
                 Tk_ScreenNumber(tkWin));
}

void WindowSystem::grabKeyboard(Tk_Window tkWin)
{
GVX_TRACE("WindowSystem::grabKeyboard[glx]");
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
GVX_TRACE("WindowSystem::ungrabKeyboard[glx]");
  XUngrabKeyboard(Tk_Display(tkWin), CurrentTime);
}

#elif defined(GVX_GL_PLATFORM_AGL)

void WindowSystem::winInfo(Tk_Window /*tkWin*/)
{
GVX_TRACE("WindowSystem::winInfo[agl]");
  // FIXME
  throw rutz::error("WindowSystem::winInfo not supported", SRC_POS);
}

void WindowSystem::iconify(Tk_Window /*tkWin*/)
{
GVX_TRACE("WindowSystem::iconify[agl]");
  // FIXME
  throw rutz::error("WindowSystem::iconify not supported", SRC_POS);
}

void WindowSystem::grabKeyboard(Tk_Window /*tkWin*/)
{
GVX_TRACE("WindowSystem::grabKeyboard[agl]");
  // don't need to do anything to grab keyboard with Mac OS X Aqua
}

void WindowSystem::ungrabKeyboard(Tk_Window /*tkWin*/)
{
GVX_TRACE("WindowSystem::ungrabKeyboard[agl]");
  // don't need to do anything to (un)grab keyboard with Mac OS X Aqua
}

#endif

///////////////////////////////////////////////////////////////////////
//
// tcl::TkWidget::TkWidgImpl definition
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

  // These are just to make sure that all our clientdata casts go
  // to/from the right types.
  void* cast_to_void(TkWidgImpl* p) { return static_cast<void*>(p); }
  TkWidgImpl* cast_from_void(void* p) { return static_cast<TkWidgImpl*>(p); }
}

class TkWidgImpl : public nub::volatile_object
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(tcl::TkWidget* o, tcl::interpreter& p,
             const char* classname,
             const char* pathname,
             bool topLevel);

  virtual ~TkWidgImpl() noexcept;

  tcl::TkWidget*   owner;
  tcl::interpreter interp;
  Tk_Window const  tkWin;
  Tk_Cursor        cursor;

  bool updatePending;
  bool shutdownRequested;

  void dbgButtonPress(const tcl::ButtonPressEvent& ev)
  {
    std::cerr << "ButtonPress: "
              << "button " << ev.button
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  void dbgKeyPress(const tcl::KeyPressEvent& ev)
  {
    std::cerr << "KeyPress: "
              << "keys '" << ev.keys << "'"
              << " control " << ev.controlPressed
              << " x " << ev.x << " y " << ev.y << std::endl;
  }

  void buttonEventProc(XButtonEvent* ev);

  void keyEventProc(XKeyEvent* ev);

  static void cEventCallback(ClientData clientData, XEvent* ev) noexcept;

  static void cRenderCallback(ClientData clientData) noexcept;

  static void cEventuallyFreeCallback(char* clientData) noexcept;

  static void cTakeFocusCallback(ClientData clientData) noexcept;
};

TkWidgImpl::TkWidgImpl(tcl::TkWidget* o, tcl::interpreter& p,
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
GVX_TRACE("TkWidgImpl::TkWidgImpl");

  if (tkWin == 0)
    {
      throw rutz::error("TkWidget couldn't create Tk_Window", SRC_POS);
    }

  Tk_SetClass(tkWin, classname);

  Tk_CreateEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        cast_to_void(this));
}

TkWidgImpl::~TkWidgImpl() noexcept
{
GVX_TRACE("TkWidgImpl::~TkWidgImpl");

  if (cursor != 0)
    Tk_FreeCursor(Tk_Display(tkWin), cursor);

  Tk_DeleteEventHandler(tkWin, EVENT_MASK,
                        TkWidgImpl::cEventCallback,
                        cast_to_void(this));

  Tcl_CancelIdleCall(cRenderCallback, cast_to_void(this));
  Tk_DestroyWindow(tkWin);
}

void TkWidgImpl::buttonEventProc(XButtonEvent* ev)
{
GVX_TRACE("TkWidgImpl::buttonEventProc");

  const bool controlPressed = ev->state & ControlMask;
  const bool shiftPressed = ev->state & ShiftMask;

  // This is an escape hatch for top-level frameless windows gone
  // awry... need to always provide a reliable way to iconify the
  // window since the title bar and "minimize" button might not exist.
  if (controlPressed && shiftPressed)
    {
      switch (ev->button)
        {
        case 1: owner->minimize(); return;
        case 3: owner->destroyWidget(); return;
        }
    }

  tcl::ButtonPressEvent e = {ev->button, ev->x, ev->y};
  owner->sigButtonPressed.emit(e);
}

void TkWidgImpl::keyEventProc(XKeyEvent* ev)
{
GVX_TRACE("TkWidgImpl::keyEventProc");

  const bool controlPressed = ev->state & ControlMask;

  // Need to save and later restore the event "state" in order that
  // subsequent processing of this event (e.g. processing by Tk "bind"
  // tables") proceeds correctly.
  const unsigned int saveState = ev->state;

  // Turn off (temporarily) any ControlMask so that we get a more
  // sensible output from XLookupString().
  ev->state &= (unsigned int)(~ControlMask);

  char buf[32];
#if defined(GVX_GL_PLATFORM_GLX)
  const int len = XLookupString(ev, &buf[0], 30, 0, 0);
  buf[len] = '\0';
#elif defined(GVX_GL_PLATFORM_AGL)
  strncpy(buf, ev->trans_chars, 31);
  buf[31] = '\0';
#endif

  // Restore the state
  ev->state = saveState;

  tcl::KeyPressEvent e = {&buf[0], ev->x, ev->y, controlPressed};
  owner->sigKeyPressed.emit(e);
}

void TkWidgImpl::cEventCallback(ClientData clientData,
                                XEvent* ev) noexcept
{
GVX_TRACE("TkWidgImpl::cEventCallback");

  TkWidgImpl* const rep = cast_from_void(clientData);
  tcl::TkWidget* const widg = rep->owner;

  try
    {
      switch (ev->type)
        {
        case Expose:
          {
            GVX_TRACE("TkWidgImpl::cEventCallback-Expose");
            if (ev->xexpose.count == 0)
              {
                widg->requestRedisplay();
              }
          }
          break;
        case ConfigureNotify:
          {
            GVX_TRACE("TkWidgImpl::cEventCallback-ConfigureNotify");

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
          rep->keyEventProc(&(ev->xkey));
          break;
        case ButtonPress:
          rep->buttonEventProc(&(ev->xbutton));
          break;
        case MapNotify:
          {
            GVX_TRACE("TkWidget::cEventCallback-MapNotify");
          }
          break;
        case DestroyNotify:
          {
            GVX_TRACE("TkWidget::cEventCallback-DestroyNotify");

            // Idiot-check that we don't have recursive destroy calls
            GVX_ASSERT(!rep->shutdownRequested);

            rep->shutdownRequested = true;

            Tcl_EventuallyFree(cast_to_void(rep),
                               cEventuallyFreeCallback);
          }
          break;
        }
    }
  catch (...)
    {
      rep->interp.handle_live_exception("cEventCallback", SRC_POS);
      rep->interp.background_error();
    }
}

void TkWidgImpl::cRenderCallback(ClientData clientData) noexcept
{
GVX_TRACE("TkWidgImpl::cRenderCallback");

  TkWidgImpl* const rep = cast_from_void(clientData);

  try
    {
      rep->owner->displayCallback();
      rep->updatePending = false;
    }
  catch (...)
    {
      rep->interp.handle_live_exception("cRenderCallback", SRC_POS);
      rep->interp.background_error();
    }
}

void TkWidgImpl::cEventuallyFreeCallback(char* clientData) noexcept
{
GVX_TRACE("TkWidgImpl::cEventuallyFreeCallback");
  TkWidgImpl* const rep = cast_from_void(clientData);
  delete rep->owner;
}

void TkWidgImpl::cTakeFocusCallback(ClientData clientData) noexcept
{
GVX_TRACE("TkWidgImpl::cTakeFocusCallback");
  TkWidgImpl* const rep = cast_from_void(clientData);
  try
    {
      rep->owner->takeFocus();
    }
  catch (...)
    {
      rep->interp.handle_live_exception("cEventCallback", SRC_POS);
      rep->interp.background_error();
    }
}

///////////////////////////////////////////////////////////////////////
//
// tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

tcl::TkWidget::TkWidget(tcl::interpreter& interp,
                        const char* classname,
                        const char* pathname,
                        bool topLevel) :
  rep(new TkWidgImpl(this, interp, classname, pathname, topLevel))
{
GVX_TRACE("tcl::TkWidget::TkWidget");

  this->mark_as_volatile();
}

tcl::TkWidget::~TkWidget() noexcept
{
GVX_TRACE("tcl::TkWidget::~TkWidget");
  delete rep;
}

int tcl::TkWidget::width() const
{
GVX_TRACE("tcl::TkWidget::width");
  return Tk_Width(rep->tkWin);
}

int tcl::TkWidget::height() const
{
GVX_TRACE("tcl::TkWidget::height");
  return Tk_Height(rep->tkWin);
}

geom::vec2<int> tcl::TkWidget::size() const
{
  return geom::vec2<int>(width(), height());
}

void tcl::TkWidget::setWidth(int w)
{
GVX_TRACE("tcl::TkWidget::setWidth");
  // Need to specify Tk_ReqHeight(rep->tkWin) instead of
  // Tk_Height(rep->tkWin), since the latter might not reflect the
  // requested height if the event loop has not been entered since a
  // call to setHeight().
  Tk_GeometryRequest(rep->tkWin, w, Tk_ReqHeight(rep->tkWin));
}

void tcl::TkWidget::setHeight(int h)
{
GVX_TRACE("tcl::TkWidget::setHeight");
  // Need to specify Tk_ReqWidth(rep->tkWin) instead of
  // Tk_Width(rep->tkWin), since the latter might not reflect the
  // requested height if the event loop has not been entered since a
  // call to setWidth().
  Tk_GeometryRequest(rep->tkWin, Tk_ReqWidth(rep->tkWin), h);
}

void tcl::TkWidget::setSize(geom::vec2<int> sz)
{
GVX_TRACE("tcl::TkWidget::setSize");
  Tk_GeometryRequest(rep->tkWin, sz.x(), sz.y());
}

void tcl::TkWidget::destroyWidget()
{
GVX_TRACE("tcl::TkWidget::destroyWidget");

  // If we are exiting, don't bother destroying the widget; otherwise...
  if ( !rep->interp.is_deleted() )
    {
      Tk_DestroyWindow(rep->tkWin);
    }
}

void tcl::TkWidget::winInfo() noexcept
{
GVX_TRACE("tcl::TkWidget::winInfo");

  WindowSystem::winInfo(rep->tkWin);
}

tcl::interpreter& tcl::TkWidget::interp() const
{
  return rep->interp;
}

Tk_Window tcl::TkWidget::tkWin() const
{
  GVX_ASSERT(rep->tkWin != 0);
  return rep->tkWin;
}

const char* tcl::TkWidget::pathname() const
{
  GVX_ASSERT(rep->tkWin != 0);
  return Tk_PathName(rep->tkWin);
}

double tcl::TkWidget::pixelsPerInch() const
{
GVX_TRACE("tcl::TkWidget::pixelsPerInch");

  GVX_ASSERT(rep->tkWin != 0);

  Screen* scr = Tk_Screen(rep->tkWin);
  const int screen_pixel_width = WidthOfScreen(scr);
  const int screen_mm_width = WidthMMOfScreen(scr);

  const double screen_inch_width = screen_mm_width / 25.4;

  const double screen_ppi = screen_pixel_width / screen_inch_width;

  dbg_eval_nl(3, screen_ppi);
  return screen_ppi;
}

void tcl::TkWidget::setCursor(const char* cursor_spec)
{
GVX_TRACE("tcl::TkWidget::setCursor");
  if (cursor_spec == 0 || *cursor_spec == '\0')
    {
      // Empty string means to unset the cursor
      if (rep->cursor != 0)
        Tk_FreeCursor(Tk_Display(rep->tkWin), rep->cursor);

      rep->cursor = 0;

      Tk_UndefineCursor(rep->tkWin);
    }
  else
    {
      Tk_Cursor new_cursor = Tk_GetCursor(rep->interp.intp(),
                                          rep->tkWin, cursor_spec);

      if (new_cursor == 0)
        {
          throw rutz::error(rutz::sfmt("couldn't set cursor to '%s'",
                                       cursor_spec), SRC_POS);
        }

      // OK, creating the new cursor succeeded, now free the old one
      if (rep->cursor != 0)
        Tk_FreeCursor(Tk_Display(rep->tkWin), rep->cursor);

      rep->cursor = new_cursor;

      Tk_DefineCursor(rep->tkWin, rep->cursor);
    }
}

const char* tcl::TkWidget::getCursor() const
{
GVX_TRACE("tcl::TkWidget::getCursor");
  return rep->cursor == 0
    ? ""
    : Tk_NameOfCursor(Tk_Display(rep->tkWin), rep->cursor);
}

void tcl::TkWidget::warpPointer(int x, int y) const
{
GVX_TRACE("tcl::TkWidget::warpPointer");

  // NOTE: The XWarpPointer emulation routine provided by Tk for Mac
  // OS X Aqua currently does nothing (as of Tk 8.5a1).
  XWarpPointer(Tk_Display(rep->tkWin),
               0, Tk_WindowId(rep->tkWin),
               0, 0, 0, 0,
               x, y);
}

void tcl::TkWidget::pack()
{
GVX_TRACE("tcl::TkWidget::pack");

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      const fstring pack_cmd =
        rutz::sfmt("pack %s -side left -expand 1 -fill both; update",
                   pathname());
      rep->interp.eval(pack_cmd);
    }
}

void tcl::TkWidget::repack(const char* options)
{
GVX_TRACE("tcl::TkWidget::repack");

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      rep->interp.eval(rutz::sfmt("pack %s %s; update",
                                  pathname(), options));
    }
}

void tcl::TkWidget::unpack()
{
GVX_TRACE("tcl::TkWidget::unpack");

  if (!Tk_IsTopLevel(rep->tkWin))
    {
      rep->interp.eval(rutz::sfmt("pack forget %s", pathname()));
    }
}

void tcl::TkWidget::iconify()
{
GVX_TRACE("tcl::TkWidget::iconify");

  WindowSystem::iconify(rep->tkWin);
}

void tcl::TkWidget::grabKeyboard()
{
GVX_TRACE("tcl::TkWidget::grabKeyboard");

  WindowSystem::grabKeyboard(rep->tkWin);

  // Oddly enough, we can't just call takeFocus() directly here. In
  // particular, we run into problems if grabKeyboard() is called from
  // an EnterNotify callback. In that case, if we call takeFocus()
  // during that callback, Tk somehow seems to erase our focus request
  // by the time control returns back to the main event loop. So, if
  // we make the call as an idle callback instead, then we don't
  // switch focus until after Tk is completely finished processing the
  // EnterNotify event.
  Tcl_DoWhenIdle(TkWidgImpl::cTakeFocusCallback,
                 cast_to_void(rep));
}

void tcl::TkWidget::ungrabKeyboard()
{
GVX_TRACE("tcl::TkWidget::ungrabKeyboard");

  WindowSystem::ungrabKeyboard(rep->tkWin);
}

void tcl::TkWidget::maximize()
{
GVX_TRACE("tcl::TkWidget::maximize");

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

void tcl::TkWidget::minimize()
{
GVX_TRACE("tcl::TkWidget::minimize");

  ungrabKeyboard();

  setSize(geom::vec2<int>(200, 200));
}

void tcl::TkWidget::bind(const fstring& event_sequence,
                         const fstring& script)
{
GVX_TRACE("tcl::TkWidget::bind");

 const fstring cmd_str =
   script.length() == 0
   // If the script is the empty string, then we want to destroy the
   // binding, so we need to actually give an empty string to the
   // "bind" command; any empty pair of braces "{ }" will not
   // suffice.
   ? rutz::sfmt("bind %s %s \"\"", pathname(), event_sequence.c_str())
   : rutz::sfmt("bind %s %s { %s }",
                pathname(), event_sequence.c_str(), script.c_str());

  rep->interp.eval(cmd_str);
}

void tcl::TkWidget::takeFocus()
{
GVX_TRACE("tcl::TkWidget::takeFocus");

  rep->interp.eval(rutz::sfmt("focus -force %s", pathname()));
}

void tcl::TkWidget::loseFocus()
{
GVX_TRACE("tcl::TkWidget::loseFocus");
  Tk_Window toplev = rep->tkWin;

  while (!Tk_IsTopLevel(toplev))
    {
      toplev = Tk_Parent(toplev);
      GVX_ASSERT(toplev != 0);
    }

  const char* pathname = Tk_PathName(toplev);

  rep->interp.eval(rutz::sfmt("wm iconify %s; wm deiconify %s ",
                              pathname, pathname));
}

void tcl::TkWidget::requestRedisplay()
{
GVX_TRACE("tcl::TkWidget::requestRedisplay");

  if (!rep->updatePending)
    {
      Tcl_DoWhenIdle(TkWidgImpl::cRenderCallback, cast_to_void(rep));
      rep->updatePending = true;
    }
}

void tcl::TkWidget::hook()
{
  sigButtonPressed.connect(rep, &TkWidgImpl::dbgButtonPress);
  sigKeyPressed.connect(rep, &TkWidgImpl::dbgKeyPress);
}
