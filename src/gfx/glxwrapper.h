///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:03 2002
// written: Wed Nov 20 20:39:48 2002
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

public:
  GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

  static GlxWrapper* make(Display* dpy, GlxOpts& opts);

  ~GlxWrapper();

  bool isDirect() const
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

  bool isDoubleBuffered() const;

  void makeCurrent(Window win) const;

  XVisualInfo* visInfo() const { return itsVisInfo; }

  GLXContext context() const { return itsContext; }

  /// Swaps buffers if in double-buffering mode.
  void swapBuffers(Window window) const;
};

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
