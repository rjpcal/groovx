///////////////////////////////////////////////////////////////////////
//
// widget.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 12:52:59 1999
// written: Mon Nov 25 18:24:06 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_CC_DEFINED
#define WIDGET_CC_DEFINED

#include "gwt/widget.h"

#include "util/dlink_list.h"
#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget::Impl definition
//
///////////////////////////////////////////////////////////////////////

class GWT::Widget::Impl
{
public:
  Impl() :
    itsButtonListeners(),
    itsKeyListeners()
  {}

  typedef dlink_list<Util::Ref<GWT::ButtonListener> > Buttons;
  typedef dlink_list<Util::Ref<GWT::KeyListener> >    Keys;

  Buttons itsButtonListeners;
  Keys itsKeyListeners;
};

///////////////////////////////////////////////////////////////////////
//
// GWT::Widget member definitions
//
///////////////////////////////////////////////////////////////////////

GWT::Widget::Widget() :
  itsImpl(new Impl)
{
DOTRACE("GWT::Widget::Widget");
}

GWT::Widget::~Widget()
{
DOTRACE("GWT::Widget::~Widget");

  delete itsImpl;
}

void GWT::Widget::addButtonListener(Util::Ref<GWT::ButtonListener> b)
{
  itsImpl->itsButtonListeners.push_back(b);
}

void GWT::Widget::addKeyListener(Util::Ref<GWT::KeyListener> k)
{
  itsImpl->itsKeyListeners.push_back(k);
}

bool GWT::Widget::hasButtonListeners() const
{
  return !(itsImpl->itsButtonListeners.is_empty());
}

bool GWT::Widget::hasKeyListeners() const
{
  return !(itsImpl->itsKeyListeners.is_empty());
}


void GWT::Widget::removeButtonListeners()
{
  itsImpl->itsButtonListeners.clear();
}

void GWT::Widget::removeKeyListeners()
{
  itsImpl->itsKeyListeners.clear();
}

void GWT::Widget::dispatchButtonEvent(unsigned int button, int x, int y)
{
  ButtonPressEvent ev = { button, x, y };

  for (Impl::Buttons::iterator
         itr = itsImpl->itsButtonListeners.begin(),
         end = itsImpl->itsButtonListeners.end();
       itr != end;
       ++itr)
    {
      (*itr)->onButtonPress(ev);
    }
}

void GWT::Widget::dispatchKeyEvent(const char* keys, int x, int y,
                                   bool controlPressed)
{
  KeyPressEvent ev = { keys, x, y, controlPressed };

  for (Impl::Keys::iterator
         itr = itsImpl->itsKeyListeners.begin(),
         end = itsImpl->itsKeyListeners.end();
       itr != end;
       ++itr)
    {
      (*itr)->onKeyPress(ev);
    }
}

static const char vcid_widget_cc[] = "$Header$";
#endif // !WIDGET_CC_DEFINED
