///////////////////////////////////////////////////////////////////////
//
// glxwrapper.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:38:07 2002
// written: Sat Aug  3 16:41:37 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_CC_DEFINED
#define GLXWRAPPER_CC_DEFINED

#include "togl/glxwrapper.h"

#include "togl/glxattribs.h"

#include "util/error.h"

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

static const char vcid_glxwrapper_cc[] = "$Header$";
#endif // !GLXWRAPPER_CC_DEFINED
