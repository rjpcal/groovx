///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:03 2002
// written: Sat Aug 10 15:12:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_H_DEFINED
#define GLXWRAPPER_H_DEFINED

#include <GL/glx.h>
#include <X11/Xlib.h>

class GlxAttribs;
class GlxOpts;

class GlxWrapper
{
private:
  Display* itsDisplay;
  XVisualInfo* itsVisInfo;
  GLXContext itsContext;

  void createContext(bool direct, GlxWrapper* share = 0);

public:
  GlxWrapper(Display* dpy, XVisualInfo* visinfo, bool direct,
             GlxWrapper* share = 0);

  GlxWrapper(Display* dpy, GlxAttribs& attribs, bool direct,
             GlxWrapper* share = 0);

  static GlxWrapper* make(Display* dpy, GlxOpts& opts);

  ~GlxWrapper();

  bool isDirect() const
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

  bool isDoubleBuffered() const;

  void makeCurrent(Window win) const;

  XVisualInfo* visInfo() const { return itsVisInfo; }
};

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
