///////////////////////////////////////////////////////////////////////
//
// widget.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Dec  2 15:05:17 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef WIDGET_H_DEFINED
#define WIDGET_H_DEFINED

#include "util/object.h"
#include "util/signal.h"

namespace GWT
{
  class Widget;

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

//  ###################################################################
//  ===================================================================
/// GWT::Widget is the base widget class.

class GWT::Widget : public virtual Util::Object
{
public:
  /// Default constructor
  Widget();

  /// Virtual destructor.
  virtual ~Widget();

  Util::Signal1<const ButtonPressEvent&> sigButtonPressed;
  Util::Signal1<const KeyPressEvent&> sigKeyPressed;

  /// Take the keyboard focus.
  virtual void takeFocus() = 0;

private:
  Widget(const Widget&);
  Widget& operator=(const Widget&);
};

static const char vcid_widget_h[] = "$Header$";
#endif // !WIDGET_H_DEFINED
