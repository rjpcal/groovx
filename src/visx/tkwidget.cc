///////////////////////////////////////////////////////////////////////
//
// tkwidget.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 17:05:12 2001
// written: Fri Jun 15 18:34:44 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_CC_DEFINED
#define TKWIDGET_CC_DEFINED

#include "tcl/tkwidget.h"

#include "util/dlink_list.h"

#include <tk.h>
#include <X11/Xutil.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#if 0
#include <iostream.h>

class DbgButtonListener : public Tcl::ButtonListener {
public:
  virtual void onButtonPress(unsigned int button, int x, int y)
  {
    cerr << "ButtonPress: "
         << "button " << button
         << " x " << x << " y " << y << endl;
  }
};

class DbgKeyListener : public Tcl::KeyListener {
public:
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
// Tcl::TkWidget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::TkWidget::Impl
{
public:

  typedef dlink_list<shared_ptr<ButtonListener> > Buttons;
  typedef dlink_list<shared_ptr<KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;

  Impl() : itsButtonListeners(), itsKeyListeners() {}

  static void buttonEventProc(ClientData clientData, XEvent* rawEvent)
  {
    DOTRACE("buttonEventProc");

    Impl* impl = static_cast<Impl*>(clientData);

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

    Impl* impl = static_cast<Impl*>(clientData);

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

Tcl::TkWidget::TkWidget() : itsImpl(new Impl)
{
#if 0
  addButtonListener(shared_ptr<Tcl::ButtonListener>(new DbgButtonListener));
  addKeyListener(shared_ptr<Tcl::KeyListener>(new DbgKeyListener));
#endif
}

Tcl::TkWidget::~TkWidget() { delete itsImpl; }

void Tcl::TkWidget::addButtonListener(shared_ptr<Tcl::ButtonListener> b)
{
  if (itsImpl->itsButtonListeners.empty())
    {
      Tk_CreateEventHandler(tkWin(), ButtonPressMask, Impl::buttonEventProc,
                            static_cast<void*>(itsImpl));
    }

  itsImpl->itsButtonListeners.push_back(b);
}

void Tcl::TkWidget::removeButtonListeners()
{
  itsImpl->itsButtonListeners.clear();

  Tk_DeleteEventHandler(tkWin(), ButtonPressMask, Impl::buttonEventProc,
                        static_cast<void*>(itsImpl));
}

void Tcl::TkWidget::addKeyListener(shared_ptr<Tcl::KeyListener> k)
{
  if (itsImpl->itsKeyListeners.empty())
    {
      Tk_CreateEventHandler(tkWin(), KeyPressMask, Impl::keyEventProc,
                            static_cast<void*>(itsImpl));
    }

  itsImpl->itsKeyListeners.push_back(k);
}

void Tcl::TkWidget::removeKeyListeners()
{
  itsImpl->itsKeyListeners.clear();

  Tk_DeleteEventHandler(tkWin(), KeyPressMask, Impl::keyEventProc,
                        static_cast<void*>(itsImpl));
}

static const char vcid_tkwidget_cc[] = "$Header$";
#endif // !TKWIDGET_CC_DEFINED
