///////////////////////////////////////////////////////////////////////
//
// tkwidget.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 15 16:59:35 2001
// written: Fri May 16 08:41:05 2003
// $Id$
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

#ifndef TKWIDGET_H_DEFINED
#define TKWIDGET_H_DEFINED

#include "gwt/widget.h"

struct Tcl_Interp;

typedef struct Tk_Window_ *Tk_Window;

namespace Gfx
{
  template <class T> class Vec2;
}

namespace Tcl
{
  class Interp;
  class TkWidget;
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

class Tcl::TkWidget : public GWT::Widget
{
public:
  /// Build a TkWidget; calls Tk_CreateWindowFromPath() internally.
  TkWidget(Tcl::Interp& interp,
           const char* classname,
           const char* pathname,
           bool topLevel = false);
  virtual ~TkWidget();

  void destroyWidget();

  int width() const;
  int height() const;
  Gfx::Vec2<int> size() const;
  void setWidth(int w);
  void setHeight(int h);
  void setSize(Gfx::Vec2<int> sz);

  Tcl::Interp& interp() const;
  Tk_Window tkWin() const;
  const char* pathname() const;
  double pixelsPerInch() const;

  void pack();

  void iconify();

  virtual void displayCallback() = 0;

  virtual void reshapeCallback(int width, int height) = 0;

  /// Bind the given script to the event_sequence.
  void bind(const char* event_sequence, const char* script);

  /// Overridden from GWT::Widget.
  virtual void takeFocus();

  void requestRedisplay();

  void hook();

private:
  TkWidget(const TkWidget&);
  TkWidget& operator=(const TkWidget&);

  friend class TkWidgImpl;

  TkWidgImpl* const rep;
};

static const char vcid_tkwidget_h[] = "$Header$";
#endif // !TKWIDGET_H_DEFINED
