///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Mon Jun 18 09:49:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tcl/tkwidget.h"

#include "util/dlink_list.h"
#include "util/ref.h"

#include <tk.h>
#include <X11/Xutil.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#if 0
#include <iostream.h>

class DbgButtonListener : public GWT::ButtonListener {
public:
  static DbgButtonListener* make() { return new DbgButtonListener; }

  virtual void onButtonPress(unsigned int button, int x, int y)
  {
    cerr << "ButtonPress: "
         << "button " << button
         << " x " << x << " y " << y << endl;
  }
};

class DbgKeyListener : public GWT::KeyListener {
public:
  static DbgKeyListener* make() { return new DbgKeyListener; }

  virtual void onKeyPress(unsigned int modifiers, const char* keys,
                          int x, int y)
  {
    cerr << "KeyPress: "
         << "keys " << keys
         << " x " << x << " y " << y << endl;
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

  typedef dlink_list<Util::Ref<GWT::ButtonListener> > Buttons;
  typedef dlink_list<Util::Ref<GWT::KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;

  TkWidgImpl() : itsButtonListeners(), itsKeyListeners() {}

  static void buttonEventProc(ClientData clientData, XEvent* rawEvent)
  {
    DOTRACE("buttonEventProc");

    TkWidgImpl* impl = static_cast<TkWidgImpl*>(clientData);

    Assert(rawEvent->type == ButtonPress);

    XButtonEvent* eventPtr = (XButtonEvent*) rawEvent;

    for (Buttons::iterator
           itr = impl->itsButtonListeners.begin(),
           end = impl->itsButtonListeners.end();
         itr != end;
         ++itr)
      {
        (*itr)->onButtonPress(eventPtr->button, eventPtr->x, eventPtr->y);
      }

  }

  static void keyEventProc(ClientData clientData, XEvent* rawEvent)
  {
    DOTRACE("keyEventProc");

    TkWidgImpl* impl = static_cast<TkWidgImpl*>(clientData);

    Assert(rawEvent->type == KeyPress);

    XKeyEvent* eventPtr = (XKeyEvent*) rawEvent;

    static char buf[32];

    int len = XLookupString(eventPtr, &buf[0], 30, 0, 0);

    buf[len] = '\0';

    for (Keys::iterator
           itr = impl->itsKeyListeners.begin(),
           end = impl->itsKeyListeners.end();
         itr != end;
         ++itr)
      {
        (*itr)->onKeyPress(eventPtr->state, &buf[0],
                           eventPtr->x, eventPtr->y);
      }
  }
};


///////////////////////////////////////////////////////////////////////
//
// Tcl::TkWidget member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TkWidget::TkWidget() : itsImpl(new TkWidgImpl)
{
#if 0
  addButtonListener(Util::Ref<GWT::ButtonListener>(DbgButtonListener::make()));
  addKeyListener(Util::Ref<GWT::KeyListener>(DbgKeyListener::make()));
#endif
}

Tcl::TkWidget::~TkWidget() { delete itsImpl; }

void Tcl::TkWidget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  if (itsImpl->itsButtonListeners.empty())
    {
      Tk_CreateEventHandler(tkWin(), ButtonPressMask, TkWidgImpl::buttonEventProc,
                            static_cast<void*>(itsImpl));
    }

  itsImpl->itsButtonListeners.push_back(b);
}

void Tcl::TkWidget::removeButtonListeners()
{
  itsImpl->itsButtonListeners.clear();

  Tk_DeleteEventHandler(tkWin(), ButtonPressMask, TkWidgImpl::buttonEventProc,
                        static_cast<void*>(itsImpl));
}

void Tcl::TkWidget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  if (itsImpl->itsKeyListeners.empty())
    {
      Tk_CreateEventHandler(tkWin(), KeyPressMask, TkWidgImpl::keyEventProc,
                            static_cast<void*>(itsImpl));
    }

  itsImpl->itsKeyListeners.push_back(k);
}

void Tcl::TkWidget::removeKeyListeners()
{
  itsImpl->itsKeyListeners.clear();

  Tk_DeleteEventHandler(tkWin(), KeyPressMask, TkWidgImpl::keyEventProc,
                        static_cast<void*>(itsImpl));
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
