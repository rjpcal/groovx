///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Fri Sep 21 11:04:05 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "visx/tkwidget.h"

#include "util/ref.h"

#include <tk.h>
#include <X11/Xutil.h>

#include "util/trace.h"
#define LOCAL_DEBUG
#include "util/debug.h"

#ifdef LOCAL_DEBUG
#include <iostream.h>

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
#endif

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
  TkWidgImpl(Tcl::TkWidget* owner, Tk_Window widg) :
    itsOwner(owner),
    itsTkWin(widg)
  {}

  ~TkWidgImpl()
  {
    destroyEventHandler(BUTTON);
    destroyEventHandler(KEY);
  }

  Tcl::TkWidget* itsOwner;
  Tk_Window itsTkWin;

  enum EventType { KEY, BUTTON };

  static void buttonEventProc(ClientData clientData, XEvent* rawEvent)
  {
    DOTRACE("Tcl::TkWidget::TkWidgImpl::buttonEventProc");

    Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

    Assert(rawEvent->type == ButtonPress);

    XButtonEvent* eventPtr = (XButtonEvent*) rawEvent;

    widg->dispatchButtonEvent(eventPtr->button, eventPtr->x, eventPtr->y);
  }

  static void keyEventProc(ClientData clientData, XEvent* rawEvent)
  {
    DOTRACE("Tcl::TkWidget::TkWidgImpl::keyEventProc");

    Tcl::TkWidget* widg = static_cast<Tcl::TkWidget*>(clientData);

    Assert(rawEvent->type == KeyPress);

    XKeyEvent* eventPtr = (XKeyEvent*) rawEvent;

    static char buf[32];

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
        Tk_CreateEventHandler(itsOwner->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(itsOwner));
        break;
      case BUTTON:
        Tk_CreateEventHandler(itsOwner->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(itsOwner));
        break;
      }
  }

  void destroyEventHandler(EventType type)
  {
    switch (type)
      {
      case KEY:
        Tk_DeleteEventHandler(itsOwner->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(itsOwner));
        break;
      case BUTTON:
        Tk_DeleteEventHandler(itsOwner->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(itsOwner));
        break;
      }
  }
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget() : itsImpl(0) {}

Tcl::TkWidget::~TkWidget()
{
  delete itsImpl;
}

Tk_Window Tcl::TkWidget::tkWin() const
{
  return itsImpl ? itsImpl->itsTkWin : 0;
}

void Tcl::TkWidget::setTkWin(Tk_Window win)
{
  delete itsImpl;
  itsImpl = new TkWidgImpl(this, win);
}

void Tcl::TkWidget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  if (itsImpl && !hasButtonListeners())
    {
      itsImpl->createEventHandler(TkWidgImpl::BUTTON);
    }

  GWT::Widget::addButtonListener(b);
}

void Tcl::TkWidget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  if (itsImpl && !hasKeyListeners())
    {
      itsImpl->createEventHandler(TkWidgImpl::KEY);
    }

  GWT::Widget::addKeyListener(k);
}

void Tcl::TkWidget::removeButtonListeners()
{
  GWT::Widget::removeButtonListeners();

  if (itsImpl && !hasButtonListeners())
    {
      itsImpl->destroyEventHandler(TkWidgImpl::BUTTON);
    }
}

void Tcl::TkWidget::removeKeyListeners()
{
  GWT::Widget::removeKeyListeners();

  if (itsImpl && !hasKeyListeners())
    {
      itsImpl->destroyEventHandler(TkWidgImpl::KEY);
    }
}

void Tcl::TkWidget::hook()
{
#ifdef LOCAL_DEBUG
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addKeyListener(Util::Ref<GWT::KeyListener>(DbgKeyListener::make()));
#endif
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
