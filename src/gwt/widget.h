///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 15:05:17 1999
// written: Thu Jul 19 20:15:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

namespace GWT
{
  class Canvas;
  class Widget;

  class ButtonListener;
  class KeyListener;

  enum EventStatus { HANDLED, NOT_HANDLED };
}

class GxNode;

namespace Util { template <class T> class Ref; };
namespace Util { template <class T> class WeakRef; }


//
// Listener class definitions
//

class GWT::ButtonListener : public Util::Object {
public:
  virtual EventStatus onButtonPress(unsigned int button, int x, int y) = 0;
};

class GWT::KeyListener : public Util::Object {
public:
  virtual EventStatus onKeyPress(unsigned int modifiers, const char* keys,
                                 int x, int y) = 0;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * GWT::Widget is the base widget class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GWT::Widget : public virtual Util::Object {
public:
  /// Default constructor
  Widget();

  /// Virtual destructor.
  virtual ~Widget();

  virtual Canvas& getCanvas() = 0;

  virtual void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  virtual void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  bool hasButtonListeners() const;
  bool hasKeyListeners() const;

  virtual void removeButtonListeners();
  virtual void removeKeyListeners();

  virtual void bind(const char* event_sequence, const char* script) = 0;
  virtual void takeFocus() = 0;
  virtual void swapBuffers() = 0;

  virtual void display();
  virtual void clearscreen();
  virtual void refresh();
  virtual void undraw();

  // Change the global visibility, which determines whether anything
  // will be displayed by a "redraw" command, or by remap events sent
  // to the screen window.
  void setVisibility(bool vis);

  void setHold(bool hold_on);

  void setDrawable(const Util::Ref<GxNode>& node);

  void dispatchButtonEvent(unsigned int button, int x, int y);
  void dispatchKeyEvent(unsigned int modifiers, const char* keys,
                        int x, int y);

private:
  Widget(const Widget&);
  Widget& operator=(const Widget&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
