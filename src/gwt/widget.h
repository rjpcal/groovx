///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 15:05:17 1999
// written: Sat Nov 23 14:30:21 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

#include "util/object.h"

namespace GWT
{
  class Widget;

  class ButtonListener;
  class KeyListener;

  enum EventStatus { HANDLED, NOT_HANDLED };
}

namespace Util
{
  template <class T> class Ref;
};


//
// Listener class definitions
//

/// Listener for button-press events.
class GWT::ButtonListener : public Util::Object
{
public:
  virtual EventStatus onButtonPress(unsigned int button, int x, int y) = 0;
};

/// Listener for key-press events.
class GWT::KeyListener : public Util::Object
{
public:
  virtual EventStatus onKeyPress(const char* keys, int x, int y,
                                 bool controlPressed) = 0;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * GWT::Widget is the base widget class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GWT::Widget : public virtual Util::Object
{
public:
  /// Default constructor
  Widget();

  /// Virtual destructor.
  virtual ~Widget();

  virtual void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  virtual void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  bool hasButtonListeners() const;
  bool hasKeyListeners() const;

  virtual void removeButtonListeners();
  virtual void removeKeyListeners();

  virtual void takeFocus() = 0;

  void dispatchButtonEvent(unsigned int button, int x, int y);
  void dispatchKeyEvent(const char* keys, int x, int y,
                        bool controlPressed);

private:
  Widget(const Widget&);
  Widget& operator=(const Widget&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
