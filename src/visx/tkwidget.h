///////////////////////////////////////////////////////////////////////
//
// tkwidget.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 16:59:35 2001
// written: Sat Jun 16 07:24:25 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_H_DEFINED
#define TKWIDGET_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WIDGET_H_DEFINED)
#include "gwt/widget.h"
#endif

typedef struct Tk_Window_ *Tk_Window;

namespace Tcl
{
  class TkWidget;
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * TkWidget is a base class that wraps Tk widgets and allows C++ event
 * bindings to be attached to them.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TkWidget : public GWT::Widget {
protected:
  virtual Tk_Window tkWin() const = 0;

public:
  TkWidget();
  virtual ~TkWidget();

  virtual void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  virtual void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  void removeButtonListeners();
  void removeKeyListeners();

private:
  TkWidget(const TkWidget&);
  TkWidget& operator=(const TkWidget&);

  class Impl;
  friend class Impl;

  Impl* const itsImpl;
};

static const char vcid_tkwidget_h[] = "$Header$";
#endif // !TKWIDGET_H_DEFINED
