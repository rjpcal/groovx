///////////////////////////////////////////////////////////////////////
//
// tkwidget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 16:59:35 2001
// written: Fri Jan 18 16:06:51 2002
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

class Tcl::TkWidget : public GWT::Widget
{
protected:
  Tk_Window tkWin() const;
  void setTkWin(Tk_Window win);

public:
  TkWidget();
  virtual ~TkWidget();

  virtual void addButtonListener (Util::Ref<GWT::ButtonListener> b);
  virtual void addKeyListener    (Util::Ref<GWT::KeyListener> k);

  virtual void removeButtonListeners();
  virtual void removeKeyListeners();

  void hook();

private:
  TkWidget(const TkWidget&);
  TkWidget& operator=(const TkWidget&);

  class TkWidgImpl;
  friend class TkWidgImpl;

  TkWidgImpl* itsImpl;
};

static const char vcid_tkwidget_h[] = "$Header$";
#endif // !TKWIDGET_H_DEFINED
