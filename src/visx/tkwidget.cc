///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Tue Sep 17 11:04:49 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "visx/tkwidget.h"

#include "tcl/tclcode.h"

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

class Tcl::TkWidget::TkWidgImpl
{
  TkWidgImpl(const TkWidgImpl&);
  TkWidgImpl& operator=(const TkWidgImpl&);

public:
  TkWidgImpl(Tcl::TkWidget* o, Tcl_Interp* p) :
    owner(o),
    interp(p),
    tkWin(0)
  {}

  ~TkWidgImpl()
  {
    destroyEventHandler(BUTTON);
    destroyEventHandler(KEY);
  }

  Tcl::TkWidget* owner;
  Tcl_Interp* interp;
  Tk_Window tkWin;

  enum EventType { KEY, BUTTON };

  static void buttonEventProc(ClientData clientData, XEvent* rawEvent)
  {
    Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

    Assert(rawEvent->type == ButtonPress);

    XButtonEvent* eventPtr = (XButtonEvent*) rawEvent;

    widg->dispatchButtonEvent(eventPtr->button, eventPtr->x, eventPtr->y);
  }

  static void keyEventProc(ClientData clientData, XEvent* rawEvent)
  {
    Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

    Assert(rawEvent->type == KeyPress);

    XKeyEvent* eventPtr = (XKeyEvent*) rawEvent;

    char buf[32];

    bool controlPressed = eventPtr->state & ControlMask;
    eventPtr->state &= ~ControlMask;

    int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);

    buf[len] = '\0';

    widg->dispatchKeyEvent(&buf[0], eventPtr->x, eventPtr->y,
                           controlPressed);
  }

  void createEventHandler(EventType type)
  {
    switch (type)
      {
      case KEY:
        Tk_CreateEventHandler(owner->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(owner));
        break;
      case BUTTON:
        Tk_CreateEventHandler(owner->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(owner));
        break;
      }
  }

  void destroyEventHandler(EventType type)
  {
    switch (type)
      {
      case KEY:
        Tk_DeleteEventHandler(owner->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(owner));
        break;
      case BUTTON:
        Tk_DeleteEventHandler(owner->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(owner));
        break;
      }
  }
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget(Tcl_Interp* interp) : rep(new TkWidgImpl(interp)) {}

Tcl::TkWidget::~TkWidget()
{
  delete rep;
}

void Tcl::TkWidget::init(Tk_Window win)
{
  Assert(rep->tkWin == 0);
  rep->tkWin = win;
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

void Tcl::TkWidget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  if (rep && !hasButtonListeners())
    {
      rep->createEventHandler(TkWidgImpl::BUTTON);
    }

  GWT::Widget::addButtonListener(b);
}

void Tcl::TkWidget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  if (rep && !hasKeyListeners())
    {
      rep->createEventHandler(TkWidgImpl::KEY);
    }

  GWT::Widget::addKeyListener(k);
}

void Tcl::TkWidget::removeButtonListeners()
{
  GWT::Widget::removeButtonListeners();

  if (rep && !hasButtonListeners())
    {
      rep->destroyEventHandler(TkWidgImpl::BUTTON);
    }
}

void Tcl::TkWidget::removeKeyListeners()
{
  GWT::Widget::removeKeyListeners();

  if (rep && !hasKeyListeners())
    {
      rep->destroyEventHandler(TkWidgImpl::KEY);
    }
}

void Tcl::TkWidget::hook()
{
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addKeyListener(Util::Ref<GWT::KeyListener>(DbgKeyListener::make()));
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
