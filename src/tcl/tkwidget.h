///////////////////////////////////////////////////////////////////////
//
// tkwidget.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 15 16:59:35 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_H_DEFINED
#define TKWIDGET_H_DEFINED

#include "nub/object.h"
#include "nub/signal.h"

typedef struct Tk_Window_ *Tk_Window;

namespace geom
{
  template <class T> class vec2;
}

namespace Tcl
{
  class Interp;
  class TkWidget;

  enum EventStatus { HANDLED, NOT_HANDLED };

  /// A mouse button-press event.
  struct ButtonPressEvent
  {
    unsigned int button;
    int x;
    int y;
  };

  /// A keypress event.
  struct KeyPressEvent
  {
    const char* keys;
    int x;
    int y;
    bool controlPressed;
  };
}

class TkWidgImpl;


///////////////////////////////////////////////////////////////////////
/**
 *
 * TkWidget is a base class that wraps Tk widgets and allows C++ event
 * bindings to be attached to them.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TkWidget : public virtual Nub::Object
{
public:
  /// Build a TkWidget; calls Tk_CreateWindowFromPath() internally.
  TkWidget(Tcl::Interp& interp,
           const char* classname,
           const char* pathname,
           bool topLevel = false);
  virtual ~TkWidget() throw();

  void destroyWidget();

  /// Print this window's X11 tidbits to stderr.
  void winInfo() throw();

  int width() const;
  int height() const;
  geom::vec2<int> size() const;
  void setWidth(int w);
  void setHeight(int h);
  void setSize(geom::vec2<int> sz);

  Tcl::Interp& interp() const;
  Tk_Window tkWin() const;
  const char* pathname() const;
  double pixelsPerInch() const;

  /// Change the cursor for this window to the one specified by the given string.
  void setCursor(const char* cursor_spec);

  /// Get a string describing the current cursor.
  const char* getCursor() const;

  /// Move the mouse pointer (cursor) to the given coordinates within the widget.
  void warpPointer(int x, int y) const;

  /// Pack this widget with default packing options.
  void pack();

  /// Re-pack this widget, with optional packing options.
  /** The options string is just an ordinary list of arguments to the Tk
      "pack" command. */
  void repack(const char* options);

  /// Make the "packer" forget this widget.
  void unpack();

  void iconify();

  /// Grab the keyboard and take the keyboard focus.
  void grabKeyboard();

  /// Ungrab the keyboard.
  void ungrabKeyboard();

  /// Make the window fullscreen and grab the keyboard
  void maximize();

  /// Make the window small (200x200 or less) and ungrab the keyboard.
  void minimize();

  virtual void displayCallback() = 0;

  virtual void reshapeCallback(int width, int height) = 0;

  /// Bind the given script to the event_sequence.
  void bind(const rutz::fstring& event_sequence,
            const rutz::fstring& script);

  /// Force the keyboard focus to go to this window.
  void takeFocus();

  /// Drop the keyboard focus back to wherever the mouse pointer is.
  void loseFocus();

  void requestRedisplay();

  void hook();

  Nub::Signal1<const Tcl::ButtonPressEvent&> sigButtonPressed;
  Nub::Signal1<const Tcl::KeyPressEvent&> sigKeyPressed;

private:
  TkWidget(const TkWidget&);
  TkWidget& operator=(const TkWidget&);

  TkWidgImpl* const rep;
};

static const char vcid_tkwidget_h[] = "$Id$ $URL$";
#endif // !TKWIDGET_H_DEFINED
