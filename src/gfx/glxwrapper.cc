///////////////////////////////////////////////////////////////////////
//
// glxwrapper.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:07 2002
// written: Wed Nov 20 20:40:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_CC_DEFINED
#define GLXWRAPPER_CC_DEFINED

#include "glxwrapper.h"

#include "gfx/glxattribs.h"
#include "gfx/glxopts.h"

#include "util/error.h"
#include "util/pointers.h"

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  // For caching the latest subject of GlxWrapper::makeCurrent()
  const GlxWrapper* currentGlxWrapper = 0;
  Window currentWindow = 0;
}

GlxWrapper::GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share) :
  itsDisplay(dpy),
  itsVisInfo(0),
  itsContext(0)
{
DOTRACE("GlxWrapper::GlxWrapper");

  int dummy;
  if (!glXQueryExtension(dpy, &dummy, &dummy))
    {
      throw Util::Error("Togl: X server has no OpenGL GLX extension");
    }

  shared_ptr<GlxAttribs> attribs = opts.buildAttribList();

  itsVisInfo = glXChooseVisual(itsDisplay,
                               DefaultScreen(itsDisplay),
                               attribs->get());

  // If we had requested single-buffering, then now try for
  // double-buffering, since that can emulate single-buffering
  if (itsVisInfo == 0)
    {
      attribs->doubleBuffer();
      itsVisInfo = glXChooseVisual(itsDisplay,
                                   DefaultScreen(itsDisplay),
                                   attribs->get());
    }

  // If we still didn't get a matching visual, then bail out
  if (itsVisInfo == 0)
    {
      throw Util::Error("couldn't find a matching visual");
    }

  dbgEvalNL(3, (void*)itsVisInfo->visualid);
  dbgEvalNL(3, itsVisInfo->depth);
  dbgEvalNL(3, itsVisInfo->bits_per_rgb);

  itsContext = glXCreateContext(itsDisplay,
                                itsVisInfo,
                                share ? share->itsContext : None,
                                opts.indirect ? GL_FALSE : GL_TRUE);

  dbgEvalNL(3, itsContext);

  if (itsContext == 0)
    {
      throw Util::Error("could not create GL rendering context");
    }
}

GlxWrapper::~GlxWrapper()
{
  glXDestroyContext(itsDisplay, itsContext);
  XFree(itsVisInfo);
}

GlxWrapper* GlxWrapper::make(Display* dpy, GlxOpts& opts)
{
DOTRACE("GlxWrapper::make");

  // Create a new OpenGL rendering context.
  GlxWrapper* glx = new GlxWrapper(dpy, opts);

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

  if (currentGlxWrapper != this || currentWindow != win)
    {
      glXMakeCurrent(itsDisplay, win, itsContext);
      currentGlxWrapper = this;
      currentWindow = win;
    }
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

void GlxWrapper::swapBuffers(Window window) const
{
  glXSwapBuffers(itsDisplay, window);
}

static const char vcid_glxwrapper_cc[] = "$Header$";
#endif // !GLXWRAPPER_CC_DEFINED