///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:03 2002
// written: Thu Nov 21 09:25:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_H_DEFINED
#define GLXWRAPPER_H_DEFINED

#include "util/ref.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

class GlxOpts;

class GlxWrapper
{
private:
  Display* itsDisplay;
  XVisualInfo* itsVisInfo;
  GLXContext itsContext;
  Window itsCurrentWin;

public:
  GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

  static GlxWrapper* make(Display* dpy, GlxOpts& opts);

  ~GlxWrapper();

  bool isDirect() const
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

  bool isDoubleBuffered() const;

  void makeCurrent(Window win);

  /// Swaps buffers if in double-buffering mode.
  void swapBuffers() const;

  Display* display() const { return itsDisplay; }

  Window window() const { return itsCurrentWin; }

  XVisualInfo* visInfo() const { return itsVisInfo; }

  GLXContext context() const { return itsContext; }
};

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
