///////////////////////////////////////////////////////////////////////
//
// tkwidget.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 15 16:59:35 2001
// written: Sat Nov 23 14:49:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TKWIDGET_H_DEFINED
#define TKWIDGET_H_DEFINED

#include "gwt/widget.h"

struct Tcl_Interp;

typedef struct Tk_Window_ *Tk_Window;

namespace Tcl
{
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
  TkWidget(Tcl_Interp* interp, const char* classname, const char* pathname);
  virtual ~TkWidget();

  void destroyWidget();

  int width() const;
  int height() const;
  void setWidth(int w);
  void setHeight(int h);

  int timeOut() const;
  void setTimeOut(int msec);

  Tcl_Interp* interp() const;
  Tk_Window tkWin() const;
  const char* pathname() const;
  double pixelsPerInch() const;

  void pack();

  virtual void displayCallback() = 0;

  virtual void reshapeCallback(int width, int height) = 0;

  /// Default version is a no-op.
  virtual void timerCallback();

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
