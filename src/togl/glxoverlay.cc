///////////////////////////////////////////////////////////////////////
//
// glxoverlay.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Aug  5 17:09:31 2002
// written: Mon Aug  5 17:15:06 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXOVERLAY_CC_DEFINED
#define GLXOVERLAY_CC_DEFINED

#include "togl/glxoverlay.h"

#include "togl/glxwrapper.h"

#include "util/error.h"

#include <tk.h>

#include <GL/glx.h>

#include "util/trace.h"

GlxOverlay::GlxOverlay(Display* dpy, Window parent,
        bool direct, GlxWrapper* share,
        int width, int height) :
  itsDisplay(dpy),
  itsGlx(0),
  itsWindow(0),
  itsUpdatePending(false),
  itsCmap(),
  itsTransparentPixel(0),
  isItMapped(false),
  itsDisplayProc(0),
  itsData(0)
{
DOTRACE("GlxOverlay::GlxOverlay");

#ifdef GLX_TRANSPARENT_TYPE_EXT
  static int ovAttributeList[] =
    {
      GLX_BUFFER_SIZE, 2,
      GLX_LEVEL, 1,
      GLX_TRANSPARENT_TYPE_EXT, GLX_TRANSPARENT_INDEX_EXT,
      None
    };
#else
  static int ovAttributeList[] =
    {
      GLX_BUFFER_SIZE, 2,
      GLX_LEVEL, 1,
      None
    };
#endif

  XVisualInfo* visinfo = glXChooseVisual( itsDisplay,
                                          DefaultScreen(itsDisplay),
                                          ovAttributeList );

  if (!visinfo)
    {
      throw Util::Error("no suitable overlay visual available");
    }

#ifdef GLX_TRANSPARENT_INDEX_EXT
  {
    int fail = glXGetConfig(itsDisplay, visinfo,
                            GLX_TRANSPARENT_INDEX_VALUE_EXT,
                            &itsTransparentPixel);
    if (fail)
      itsTransparentPixel = 0;
  }
#endif

  // share display lists with normal layer context
  itsGlx = new GlxWrapper(itsDisplay, visinfo, direct, share);

  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap( itsDisplay,
                                  RootWindow(itsDisplay, visinfo->screen),
                                  visinfo->visual, AllocNone );
  itsCmap = swa.colormap;

  swa.border_pixel = 0;
  swa.event_mask =
    KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|
    EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|
    VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask;

  itsWindow = XCreateWindow( itsDisplay, parent,
                             0, 0,
                             width, height, 0,
                             visinfo->depth, InputOutput,
                             visinfo->visual,
                             CWBorderPixel|CWColormap|CWEventMask,
                             &swa );

  /* Make sure window manager installs our colormap */
  XSetWMColormapWindows( itsDisplay, itsWindow, &itsWindow, 1 );
}

GlxOverlay::~GlxOverlay()
{
DOTRACE("GlxOverlay::~GlxOverlay");
  delete itsGlx;
}

void GlxOverlay::setDisplayProc(Callback* proc, void* data)
{
DOTRACE("GlxOverlay::setDisplayProc");
  itsDisplayProc = proc;
  itsData = data;
}

void GlxOverlay::render()
{
DOTRACE("GlxOverlay::render");
  if (itsDisplayProc)
    {
      itsGlx->makeCurrent(itsWindow);
      itsDisplayProc(itsData);
    }
  itsUpdatePending = false;
}

void GlxOverlay::reconfigure(int width, int height)
{
DOTRACE("GlxOverlay::reconfigure");
  XResizeWindow(itsDisplay, itsWindow, width, height);
  XRaiseWindow(itsDisplay, itsWindow);
}

void GlxOverlay::makeCurrent()
{
DOTRACE("GlxOverlay::makeCurrent");
  itsGlx->makeCurrent(itsWindow);
}

void GlxOverlay::show()
{
DOTRACE("GlxOverlay::show");
  if (itsWindow)
    {
      XMapWindow(itsDisplay, itsWindow);
      XInstallColormap(itsDisplay, itsCmap);
      isItMapped = true;
    }
}

void GlxOverlay::hide()
{
DOTRACE("GlxOverlay::hide");
  if (itsWindow && isItMapped)
    {
      XUnmapWindow(itsDisplay, itsWindow);
      isItMapped = false;
    }
}

namespace
{
  void dummyRenderCallback(ClientData clientData)
  {
    DOTRACE("<glxoverlay.cc>::dummyOverlayRenderCallback");
    GlxOverlay* p = static_cast<GlxOverlay*>(clientData);
    p->render();
  }
}

void GlxOverlay::requestRedisplay()
{
DOTRACE("GlxOverlay::requestRedisplay");
  if (!itsUpdatePending && itsWindow && itsDisplayProc && isItMapped)
    {
      Tk_DoWhenIdle( dummyRenderCallback,
                     static_cast<ClientData>(this) );
      itsUpdatePending = true;
    }
}

unsigned long GlxOverlay::allocColor(float red, float green, float blue)
{
DOTRACE("GlxOverlay::allocColor");
  if (itsCmap)
    {
      XColor xcol;
      xcol.red   = (short) (red* 65535.0);
      xcol.green = (short) (green* 65535.0);
      xcol.blue  = (short) (blue* 65535.0);

      if ( !XAllocColor(itsDisplay, itsCmap, &xcol) )
        return (unsigned long) -1;
      return xcol.pixel;
    }
  else
    return (unsigned long) -1;
}

void GlxOverlay::freeColor(unsigned long pixel)
{
DOTRACE("GlxOverlay::freeColor");
  if (itsCmap)
    {
      XFreeColors(itsDisplay, itsCmap, &pixel, 1, 0);
    }
}

static const char vcid_glxoverlay_cc[] = "$Header$";
#endif // !GLXOVERLAY_CC_DEFINED
