///////////////////////////////////////////////////////////////////////
//
// glxwrapper.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:07 2002
// written: Sun Nov  3 13:41:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_CC_DEFINED
#define GLXWRAPPER_CC_DEFINED

#include "togl/glxwrapper.h"

#include "gfx/glcanvas.h"

#include "grsh/grsh.h"

#include "gx/rgbacolor.h"

#include "togl/glxattribs.h"
#include "togl/glxopts.h"

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
  itsContext(0),
  itsOpts(opts),
  itsCanvas()
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

  //
  // Set up canvas
  //

  itsCanvas = Util::SoftRef<GLCanvas>
    (GLCanvas::make(itsVisInfo->depth, opts.rgbaFlag, isDoubleBuffered()));

  if (opts.rgbaFlag)
    {
      itsCanvas->setColor(Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0));
      itsCanvas->setClearColor(Gfx::RgbaColor(1.0, 1.0, 1.0, 1.0));
    }
  else
    {
      // FIXME use XBlackPixel(), XWhitePixel() here?
      itsCanvas->setColorIndex(0);
      itsCanvas->setClearColorIndex(1);
    }

  // Check for a single/double buffering snafu
  if (opts.doubleFlag == 0 && isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double buffered
      // visual.  Set the GL draw buffer to be the front buffer to
      // simulate single buffering.
      glDrawBuffer(GL_FRONT);
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
      Grsh::installCanvas(*itsCanvas);
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

Gfx::Canvas& GlxWrapper::canvas() const
{
  return *itsCanvas;
}

void GlxWrapper::flush(Window window) const
{
  if (itsOpts.doubleFlag)
    {
      glXSwapBuffers(itsDisplay, window);
    }
  else
    {
      glFlush();
    }
}

static const char vcid_glxwrapper_cc[] = "$Header$";
#endif // !GLXWRAPPER_CC_DEFINED
