///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 15:05:17 1999
// written: Mon Nov 25 18:22:51 2002
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

  struct ButtonPressEvent
  {
    unsigned int button;
    int x;
    int y;
  };

  struct KeyPressEvent
  {
    const char* keys;
    int x;
    int y;
    bool controlPressed;
  };
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
  virtual void onButtonPress(const ButtonPressEvent& ev) = 0;
};

/// Listener for key-press events.
class GWT::KeyListener : public Util::Object
{
public:
  virtual void onKeyPress(const KeyPressEvent& ev) = 0;
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

  void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  bool hasButtonListeners() const;
  bool hasKeyListeners() const;

  void removeButtonListeners();
  void removeKeyListeners();

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
