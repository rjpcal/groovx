///////////////////////////////////////////////////////////////////////
//
// glxwrapper.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Aug  3 16:38:03 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXWRAPPER_H_DEFINED
#define GLXWRAPPER_H_DEFINED

#include "gfx/glwindowinterface.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

class GlxOpts;

/// Wraps the GLXContext mechanism.
class GlxWrapper : public GlWindowInterface
{
private:
  Display* itsDisplay;
  XVisualInfo* itsVisInfo;
  GLXContext itsContext;
  Window itsCurrentWin;

  GlxWrapper(const GlxWrapper&);
  GlxWrapper& operator=(const GlxWrapper&);

  /// Construct.
  GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

public:
  /// Factory function.
  static GlxWrapper* make(Display* dpy, GlxOpts& opts, GlxWrapper* share = 0);

  /// Destructor.
  virtual ~GlxWrapper();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const
  {
    return glXIsDirect(itsDisplay, itsContext) == True ? true : false;
  }

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const;

  /// Bind the rendering context to the given window.
  virtual void makeCurrent(Window win);

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height) = 0;

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const;

  /// Instantiate an actual X11 window for the given Tk_Window.
  virtual Window makeTkRealWindow(Tk_Window tkwin, Window parent,
                                  int width, int height) throw();

  // GLX-specific functions:

  /// Get the associated X11 Display.
  Display* display() const { return itsDisplay; }

  /// Get the currently associated X11 Window.
  Window window() const { return itsCurrentWin; }

  /// Get the associated GLXContext.
  GLXContext context() const { return itsContext; }

  /// Get the associated X11 XVisualInfo.
  XVisualInfo* visInfo() const { return itsVisInfo; }
};

//---------------------------------------------------------------------
//
// Implementation
//
//---------------------------------------------------------------------

#include "gfx/glxopts.h"

#include "util/error.h"
#include "util/pointers.h"

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

namespace
{
  // For caching the latest subject of GlxWrapper::makeCurrent()
  const GlxWrapper* currentGlxWrapper = 0;

  // Builds a formatted data array for constructing a GLX context.
  class AttribList
  {
  private:
    enum { MAXSIZE = 1000 };

    void push(int v)
    {
      Assert(next < MAXSIZE);
      data[next++] = v;
    }

    void cap()
    {
      data[next] = None;
    }

    int data[MAXSIZE];
    int next;

  public:
    /// Default constructor.
    AttribList(const GlxOpts& opts) : next(0)
    {
      push( GLX_USE_GL );

      if (opts.rgbaFlag)        this->rgba(opts.rgbaRed, opts.rgbaGreen, opts.rgbaBlue,
                                           opts.alphaFlag ? opts.alphaSize : -1);

      else                      this->colorIndex( opts.colorIndexSize );

      if (opts.depthFlag)       this->depthBuffer( opts.depthSize );

      if (opts.doubleFlag)      this->doubleBuffer();

      if (opts.stencilFlag)     this->stencilBuffer( opts.stencilSize );

      if (opts.accumFlag)       this->accum(opts.accumRed, opts.accumGreen, opts.accumBlue,
                                            opts.alphaFlag ? opts.accumAlpha : -1);

      if (opts.auxNumber > 0)   this->auxBuffers( opts.auxNumber );

      if (opts.level != 0)      this->level( opts.level );

      if (opts.transparent)     this->transparent();
    }

    /// Get the underlying data array to use to construct a GLXContext.
    int* get() { cap(); return data; }

    /// Set RGBA-mode minimum bit-depths.
    void rgba(int rbits, int gbits, int bbits, int abits = -1)
    {
      push( GLX_RGBA );
      push( GLX_RED_SIZE );
      push( rbits );          dbgEval(3, rbits);
      push( GLX_GREEN_SIZE );
      push( gbits );          dbgEval(3, gbits);
      push( GLX_BLUE_SIZE );
      push( bbits );          dbgEvalNL(3, bbits);
      if (abits > 0)
        {
          push( GLX_ALPHA_SIZE );
          push( abits );      dbgEvalNL(3, abits);
        }
    }

    /// Specify the stacking level for this context.
    void level(int lev)
    {
      push( GLX_LEVEL );
      push( lev );
    }

    /// Specify a minimum bit-depth for color-index mode.
    void colorIndex(int bits)
    {
      push( GLX_BUFFER_SIZE );
      push( bits );
    }

    /// Specify a minimum bit-depth for the depth-buffer (i.e. z-buffer).
    void depthBuffer(int bits)
    {
      push( GLX_DEPTH_SIZE );
      push( bits );
    }

    /// Request double-buffering mode.
    void doubleBuffer()
    {
      push( GLX_DOUBLEBUFFER );
    }

    /// Specify a minimum bit-depth for the stencil buffer.
    void stencilBuffer(int bits)
    {
      push( GLX_STENCIL_SIZE );
      push( bits );
    }

    /// Specify minimum bit-depths for the accum buffer.
    void accum(int rbits, int gbits, int bbits, int abits = -1)
    {
      push( GLX_ACCUM_RED_SIZE );
      push( rbits );
      push( GLX_ACCUM_GREEN_SIZE );
      push( gbits );
      push( GLX_ACCUM_BLUE_SIZE );
      push( bbits );
      if (abits > 0)
        {
          push( GLX_ACCUM_ALPHA_SIZE );
          push( abits );
        }
    }

    /// Request a number of aux buffers.
    void auxBuffers(int n)
    {
      push( GLX_AUX_BUFFERS );
      push( n );
    }

    /// Request transparency if available.
    void transparent()
    {
#ifdef GLX_TRANSPARENT_TYPE_EXT
      push( GLX_TRANSPARENT_TYPE_EXT );
      push( GLX_TRANSPARENT_INDEX_EXT );
#endif
    }
  };
}

GlxWrapper::GlxWrapper(Display* dpy, GlxOpts& opts, GlxWrapper* share) :
  itsDisplay(dpy),
  itsVisInfo(0),
  itsContext(0),
  itsCurrentWin(0)
{
DOTRACE("GlxWrapper::GlxWrapper");

  int dummy;
  if (!glXQueryExtension(dpy, &dummy, &dummy))
    {
      throw Util::Error("GlxWrapper: X server has no OpenGL GLX extension");
    }

  AttribList attribs(opts);

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
DOTRACE("GlxWrapper::~GlxWrapper");

  glXDestroyContext(itsDisplay, itsContext);
  XFree(itsVisInfo);
}

GlxWrapper* GlxWrapper::make(Display* dpy, GlxOpts& opts, GlxWrapper* share)
{
DOTRACE("GlxWrapper::make");

  // Create a new OpenGL rendering context.
  GlxWrapper* glx = new GlxWrapper(dpy, opts, share);

  // Make sure we don't try to use a depth buffer with indirect rendering
  if ( !glx->isDirect() && opts.depthFlag )
    {
      delete glx;
      opts.depthFlag = false;
      return make(dpy, opts, share);
    }

  opts.indirect = !glx->isDirect();

  return glx;
}

void GlxWrapper::makeCurrent(Window win)
{
DOTRACE("GlxWrapper::makeCurrent");

  if (currentGlxWrapper != this || itsCurrentWin != win)
    {
      glXMakeCurrent(itsDisplay, win, itsContext);
      currentGlxWrapper = this;
      itsCurrentWin = win;
    }
}

void GlxWrapper::onReshape(int /*width*/, int /*height*/)
{
  // nothing to do here
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

unsigned int GlxWrapper::bitsPerPixel() const
{
DOTRACE("GlxWrapper::bitsPerPixel");
  return itsVisInfo->depth;
}

void GlxWrapper::swapBuffers() const
{
  glXSwapBuffers(itsDisplay, itsCurrentWin);
}

Window GlxWrapper::makeTkRealWindow(Tk_Window tkwin, Window parent,
                                    int width, int height) throw()
{
DOTRACE("GlxWrapper::makeTkRealWindow");

  Display* dpy = Tk_Display(tkwin);

  Visual* visual = itsVisInfo->visual;
  const int screen = itsVisInfo->screen;

  Colormap cmap =
    visual == DefaultVisual(dpy, screen)
    ? DefaultColormap(dpy, screen)
    : XCreateColormap(dpy,
                      RootWindow(dpy, screen),
                      visual, AllocNone);

  // Make sure Tk knows to switch to the new colormap when the cursor is over
  // this window when running in color index mode.
  Tk_SetWindowVisual(tkwin, visual, itsVisInfo->depth, cmap);

#define ALL_EVENTS_MASK \
KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask| \
EnterWindowMask|LeaveWindowMask|PointerMotionMask|ExposureMask|   \
VisibilityChangeMask|FocusChangeMask|PropertyChangeMask|ColormapChangeMask

  XSetWindowAttributes atts;

  atts.colormap = cmap;
  atts.border_pixel = 0;
  atts.event_mask = ALL_EVENTS_MASK;

  unsigned long valuemask = CWBorderPixel | CWColormap | CWEventMask;

  if (Tk_IsTopLevel(tkwin))
    {
      atts.override_redirect = true;
      valuemask |= CWOverrideRedirect;
    }

  Window win = XCreateWindow(dpy,
                             parent,
                             0, 0,
                             width,
                             height,
                             0, itsVisInfo->depth,
                             InputOutput, visual,
                             valuemask,
                             &atts);

  Tk_ChangeWindowAttributes(tkwin, valuemask, &atts);

  XSelectInput(dpy, win, ALL_EVENTS_MASK);

  return win;
}

static const char vcid_glxwrapper_h[] = "$Header$";
#endif // !GLXWRAPPER_H_DEFINED
