///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:03 2002
// written: Mon Sep 16 11:54:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_H_DEFINED
#define GLXWRAPPER_H_DEFINED

#include <GL/glx.h>
#include <X11/Xlib.h>

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

namespace Gfx
{
  class Canvas;
}

class GLCanvas;

class GlxAttribs;
class GlxOpts;

class GlxWrapper
{
private:
  Display* itsDisplay;
  XVisualInfo* itsVisInfo;
  GLXContext itsContext;
  Util::SoftRef<GLCanvas> itsCanvas;

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

  Canvas& canvas() const;
};

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
