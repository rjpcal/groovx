///////////////////////////////////////////////////////////////////////
//
// aglwrapper.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 20 10:21:15 2004
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

#ifndef AGLWRAPPER_CC_DEFINED
#define AGLWRAPPER_CC_DEFINED

#if defined(GL_PLATFORM_AGL)

#include "aglwrapper.h"

#include "gfx/glxopts.h"

#include "util/error.h"

#define HAVE_LIMITS_H
#include <tkMacOSX.h>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

namespace
{
  // Builds a formatted data array for constructing a GLX context.
  class AttribList
  {
  private:
    enum { MAXSIZE = 1000 };

    void push(GLint v)
    {
      Assert(next < MAXSIZE);
      data[next++] = v;
    }

    void cap()
    {
      data[next] = AGL_NONE;
    }

    GLint data[MAXSIZE];
    int next;

  public:
    /// Default constructor.
    AttribList(const GlxOpts& opts) : next(0)
    {
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

      if (!opts.indirect)       this->accelerated();
    }

    /// Get the underlying data array to use to construct a AGLContext.
    GLint* get() { cap(); return data; }

    /// Set RGBA-mode minimum bit-depths.
    void rgba(int rbits, int gbits, int bbits, int abits = -1)
    {
      push( AGL_RGBA );
      push( AGL_RED_SIZE );
      push( rbits );          dbgEval(3, rbits);
      push( AGL_GREEN_SIZE );
      push( gbits );          dbgEval(3, gbits);
      push( AGL_BLUE_SIZE );
      push( bbits );          dbgEvalNL(3, bbits);
      if (abits > 0)
        {
          push( AGL_ALPHA_SIZE );
          push( abits );      dbgEvalNL(3, abits);
        }
    }

    /// Specify the stacking level for this context.
    void level(int lev)
    {
      push( AGL_LEVEL );
      push( lev );
    }

    /// Specify a minimum bit-depth for color-index mode.
    void colorIndex(int bits)
    {
      push( AGL_BUFFER_SIZE );
      push( bits );
    }

    /// Specify a minimum bit-depth for the depth-buffer (i.e. z-buffer).
    void depthBuffer(int bits)
    {
      push( AGL_DEPTH_SIZE );
      push( bits );
    }

    /// Request double-buffering mode.
    void doubleBuffer()
    {
      push( AGL_DOUBLEBUFFER );
    }

    /// Specify a minimum bit-depth for the stencil buffer.
    void stencilBuffer(int bits)
    {
      push( AGL_STENCIL_SIZE );
      push( bits );
    }

    /// Specify minimum bit-depths for the accum buffer.
    void accum(int rbits, int gbits, int bbits, int abits = -1)
    {
      push( AGL_ACCUM_RED_SIZE );
      push( rbits );
      push( AGL_ACCUM_GREEN_SIZE );
      push( gbits );
      push( AGL_ACCUM_BLUE_SIZE );
      push( bbits );
      if (abits > 0)
        {
          push( AGL_ACCUM_ALPHA_SIZE );
          push( abits );
        }
    }

    /// Request a number of aux buffers.
    void auxBuffers(int n)
    {
      push( AGL_AUX_BUFFERS );
      push( n );
    }

    /// Request a hardware-accelarted renderer.
    void accelerated()
    {
      push( AGL_ACCELERATED );
    }

    /// Request transparency if available.
    void transparent()
    {
      // FIXME not implementable?
    }
  };
}

AglWrapper::AglWrapper(Display* dpy, GlxOpts& opts) :
  itsDisplay(dpy),
  itsPixFormat(0),
  itsContext(0),
  itsCurrentWin(0)
{
DOTRACE("AglWrapper::AglWrapper");

  // AGLDevice is a typedef for GDHandle
  AGLDevice gdev = GetMainDevice();
  const GLint ndev = 1;

  AttribList attribs(opts);

  itsPixFormat = aglChoosePixelFormat(&gdev, ndev, attribs.get());

  if (itsPixFormat == 0)
    throw Util::Error("couldn't choose Apple-OpenGL pixel format");

  AGLContext share = 0;

  itsContext = aglCreateContext(itsPixFormat, share);

  if (itsContext == 0)
    throw Util::Error("couldn't create Apple-OpenGL graphics context");
}

AglWrapper* AglWrapper::make(Display* dpy, GlxOpts& opts)
{
DOTRACE("AglWrapper::make");
  return new AglWrapper(dpy, opts);
}

AglWrapper::~AglWrapper()
{
DOTRACE("AglWrapper::~AglWrapper");

  aglDestroyPixelFormat(itsPixFormat);
  itsPixFormat = 0;

  aglDestroyContext(itsContext);
  itsContext = 0;
}

bool AglWrapper::isDirect() const
{
DOTRACE("AglWrapper::isDirect");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_ACCELERATED, &value);

  if (status == GL_FALSE)
    throw Util::Error("couldn't get Apple-OpenGL accelerated attribute");

  return (value == GL_TRUE);
}

bool AglWrapper::isDoubleBuffered() const
{
DOTRACE("AglWrapper::isDoubleBuffered");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_DOUBLEBUFFER, &value);

  if (status == GL_FALSE)
    throw Util::Error("couldn't get Apple-OpenGL doublebuffer attribute");

  return (value == GL_TRUE);
}

unsigned int AglWrapper::bitsPerPixel() const
{
DOTRACE("AglWrapper::bitsPerPixel");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_PIXEL_SIZE, &value);

  if (status == GL_FALSE)
    throw Util::Error("couldn't get Apple-OpenGL pixel size attribute");

  Assert(value > 0);

  return static_cast<unsigned int>(value);
}

void AglWrapper::makeCurrent(Window win)
{
DOTRACE("AglWrapper::makeCurrent");

  // AGLDrawable is a typedef for CGrafPtr
  CGrafPtr drawable = TkMacOSXGetDrawablePort(win);

  if (GET_DBG_LEVEL() >= 3)
    {
      WindowRef macWindow = GetWindowFromPort(drawable);
      Rect rectPort;
      GetWindowPortBounds (macWindow, &rectPort);
      dbgEval(0, rectPort.right);
      dbgEvalNL(0, rectPort.left);
      dbgEval(0, rectPort.bottom);
      dbgEvalNL(0, rectPort.top);
    }

  int status1 = aglSetDrawable(itsContext, drawable);

  if (status1 == GL_FALSE)
    throw Util::Error("couldn't set Apple-OpenGL drawable");

  int status2 = aglSetCurrentContext(itsContext);

  if (status2 == GL_FALSE)
    throw Util::Error("couldn't set current Apple-OpenGL context");

  int status3 = aglUpdateContext(itsContext);

  if (status3 == GL_FALSE)
    throw Util::Error("couldn't update Apple-OpenGL context");
}

void AglWrapper::onReshape(int /*width*/, int /*height*/)
{
DOTRACE("AglWrapper::onReshape");

  int status = aglUpdateContext(itsContext);

  if (status == GL_FALSE)
    throw Util::Error("couldn't update Apple-OpenGL context");
}

void AglWrapper::swapBuffers() const
{
DOTRACE("AglWrapper::swapBuffers");
  aglSwapBuffers(itsContext);
}

Window AglWrapper::makeTkRealWindow(Tk_Window tkwin, Window parent,
                                    int width, int height) throw()
{
DOTRACE("AglWrapper::makeTkRealWindow");

  Assert(false); // FIXME
  (void) tkwin;
  (void) parent;
  (void) width;
  (void) height;
  return (Window) 0;
}

#endif // defined(GL_PLATFORM_AGL)

static const char vcid_aglwrapper_cc[] = "$Header$";
#endif // !AGLWRAPPER_CC_DEFINED
