///////////////////////////////////////////////////////////////////////
//
// glxwrapper.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:07 2002
// written: Mon Aug  5 13:16:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_CC_DEFINED
#define GLXWRAPPER_CC_DEFINED

#include "togl/glxwrapper.h"

#include "togl/glxattribs.h"
#include "togl/glxopts.h"

#include "util/error.h"
#include "util/pointers.h"

#include "util/debug.h"
#include "util/trace.h"

void GlxWrapper::createContext(bool direct, GlxWrapper* share)
{
DOTRACE("GlxWrapper::createContext");
  itsContext = glXCreateContext(itsDisplay,
                                itsVisInfo,
                                share ? share->itsContext : None,
                                direct ? GL_TRUE : GL_FALSE);

  DebugEvalNL(itsContext);

  if (itsContext == 0)
    {
      throw Util::Error("could not create GL rendering context");
    }
}

GlxWrapper::GlxWrapper(Display* dpy, XVisualInfo* visinfo, bool direct,
                       GlxWrapper* share) :
  itsDisplay(dpy),
  itsVisInfo(visinfo),
  itsContext(0)
{
DOTRACE("GlxWrapper::GlxWrapper");
  createContext(direct, share);
}

GlxWrapper::GlxWrapper(Display* dpy, GlxAttribs& attribs, bool direct,
                       GlxWrapper* share) :
  itsDisplay(dpy),
  itsVisInfo(0),
  itsContext(0)
{
DOTRACE("GlxWrapper::GlxWrapper");
  itsVisInfo = glXChooseVisual(itsDisplay,
                               DefaultScreen(itsDisplay),
                               attribs.get());

  // If we had requested single-buffering, then now try for
  // double-buffering, since that can emulate single-buffering
  if (itsVisInfo == 0)
    {
      attribs.doubleBuffer();
      itsVisInfo = glXChooseVisual(itsDisplay,
                                   DefaultScreen(itsDisplay),
                                   attribs.get());
    }

  // If we still didn't get a matching visual, then bail out
  if (itsVisInfo == 0)
    {
      throw Util::Error("couldn't find a matching visual");
    }

  DebugEvalNL((void*)itsVisInfo->visualid);
  DebugEvalNL(itsVisInfo->depth);
  DebugEvalNL(itsVisInfo->bits_per_rgb);

  createContext(direct, share);
}

GlxWrapper* GlxWrapper::make(Display* dpy, GlxOpts& opts)
{
DOTRACE("GlxWrapper::make");

  shared_ptr<GlxAttribs> attribs = opts.buildAttribList();

  // Create a new OpenGL rendering context.
  GlxWrapper* glx = new GlxWrapper(dpy, *attribs, !opts.indirect);

  // Make sure we don't try to use a depth buffer with indirect rendering
  if ( !glx->isDirect() && opts.depthFlag == true )
    {
      delete glx;
      opts.depthFlag = false;
      return make(dpy, opts);
    }

  opts.indirect = !glx->isDirect();

  return glx;
}

void GlxWrapper::makeCurrent(Window win) const
{
DOTRACE("GlxWrapper::makeCurrent");

  glXMakeCurrent(itsDisplay, win, itsContext);
}

bool GlxWrapper::isDoubleBuffered() const
{
DOTRACE("GlxWrapper::isDoubleBuffered");

  int dbl_flag;

  if (glXGetConfig(itsDisplay, itsVisInfo, GLX_DOUBLEBUFFER, &dbl_flag) != 0)
    {
      throw Util::Error("glXGetConfig failed");
    }

  return bool(dbl_flag);
}

static const char vcid_glxwrapper_cc[] = "$Header$";
#endif // !GLXWRAPPER_CC_DEFINED
