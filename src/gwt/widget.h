///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Dec  2 15:05:17 1999
// written: Sat Nov 23 14:04:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

#include "util/object.h"

class Scene;

class GxCamera;
class GxNode;

namespace Gfx
{
  class Canvas;
}

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

  virtual Gfx::Canvas& getCanvas() const = 0;

  virtual int width() const = 0;
  virtual int height() const = 0;

  virtual void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  virtual void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  bool hasButtonListeners() const;
  bool hasKeyListeners() const;

  virtual void removeButtonListeners();
  virtual void removeKeyListeners();

  virtual void takeFocus() = 0;

  virtual void undraw();

  /** "Bare-bones rendering": only render the current object; the caller is
      expected to take care of clearing the color buffer first and flushing
      the graphics stream afterwards. */
  void render();

  /** "Full-featured rendering": first clears the color buffer, then renders
      the current object, then flushes the graphics stream and swaps buffers
      if necessary. */
  void fullRender();

  /** "Bare-bones clearscreen": clear the color buffer and set the current
      object to empty, but don't flush the graphics stream. */
  void clearscreen();

  /** "Full-featured clearscreen": clear the color buffer, set the current
      object to empty, and flush the graphics stream. */
  void fullClearscreen();

  // Change the global visibility, which determines whether anything
  // will be displayed by a "redraw" command, or by remap events sent
  // to the screen window.
  void setVisibility(bool vis);

  void setHold(bool hold_on);

  void allowRefresh(bool allow);

  const Util::Ref<GxCamera>& getCamera() const;
  void setCamera(const Util::Ref<GxCamera>& cam);

  void setDrawable(const Util::Ref<GxNode>& node);

  void animate(unsigned int framesPerSecond);

  void dispatchButtonEvent(unsigned int button, int x, int y);
  void dispatchKeyEvent(const char* keys, int x, int y,
                        bool controlPressed);

private:
  Widget(const Widget&);
  Widget& operator=(const Widget&);

  class Impl;
  Impl* const itsImpl;
  Scene* const itsScene;
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
