///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Sat Jul 21 18:28:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tcl/tkwidget.h"

#include "util/ref.h"

#include <tk.h>
#include <X11/Xutil.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_DEBUG
#define LOCAL_ASSERT
#include "util/debug.h"

#ifdef LOCAL_DEBUG
#include <iostream.h>

class DbgButtonListener : public GWT::ButtonListener {
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

class DbgKeyListener : public GWT::KeyListener {
public:
  static DbgKeyListener* make() { return new DbgKeyListener; }

  virtual GWT::EventStatus onKeyPress(unsigned int modifiers,
                                      const char* keys,
                                      int x, int y)
  {
    std::cerr << "KeyPress: "
              << "keys " << keys
              << " mods " << modifiers
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
public:

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

    int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);

    buf[len] = '\0';

    widg->dispatchKeyEvent(eventPtr->state, &buf[0],
                           eventPtr->x, eventPtr->y);
  }

  enum EventType { KEY, BUTTON };

  static void createEventHandler(Tcl::TkWidget* widg, EventType type)
  {
    switch (type)
      {
      case KEY:
        Tk_CreateEventHandler(widg->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(widg));
        break;
      case BUTTON:
        Tk_CreateEventHandler(widg->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(widg));
        break;
      }
  }

  static void destroyEventHandler(Tcl::TkWidget* widg, EventType type)
  {
    switch (type)
      {
      case KEY:
        Tk_DeleteEventHandler(widg->tkWin(), KeyPressMask, keyEventProc,
                              static_cast<void*>(widg));
        break;
      case BUTTON:
        Tk_DeleteEventHandler(widg->tkWin(), ButtonPressMask, buttonEventProc,
                              static_cast<void*>(widg));
        break;
      }
  }
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget() : itsImpl(new TkWidgImpl) {}

Tcl::TkWidget::~TkWidget()
{
  TkWidgImpl::destroyEventHandler(this, TkWidgImpl::BUTTON);
  TkWidgImpl::destroyEventHandler(this, TkWidgImpl::KEY);
  delete itsImpl;
}

void Tcl::TkWidget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  if (!hasButtonListeners())
    {
      TkWidgImpl::createEventHandler(this, TkWidgImpl::BUTTON);
    }

  GWT::Widget::addButtonListener(b);
}

void Tcl::TkWidget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  if (!hasKeyListeners())
    {
      TkWidgImpl::createEventHandler(this, TkWidgImpl::KEY);
    }

  GWT::Widget::addKeyListener(k);
}

void Tcl::TkWidget::removeButtonListeners()
{
  GWT::Widget::removeButtonListeners();

  if (!hasButtonListeners())
    {
      TkWidgImpl::destroyEventHandler(this, TkWidgImpl::BUTTON);
    }
}

void Tcl::TkWidget::removeKeyListeners()
{
  GWT::Widget::removeKeyListeners();

  if (!hasKeyListeners())
    {
      TkWidgImpl::destroyEventHandler(this, TkWidgImpl::KEY);
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
