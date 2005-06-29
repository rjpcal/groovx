///////////////////////////////////////////////////////////////////////
//
// aglwrapper.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jul 20 10:18:14 2004
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED

#include "gfx/glwindowinterface.h"

#include <AGL/agl.h>

class GlxOpts;

/// Wraps the GLXContext mechanism.
class AglWrapper : public GlWindowInterface
{
private:
  Display* itsDisplay;
  AGLPixelFormat itsPixFormat;
  AGLContext itsContext;
  Window itsCurrentWin;

  AglWrapper(const AglWrapper&);
  AglWrapper& operator=(const AglWrapper&);

  /// Construct.
  AglWrapper(Display* dpy, GlxOpts& opts);

public:
  /// Factory function.
  static AglWrapper* make(Display* dpy, GlxOpts& opts);

  /// Destructor.
  virtual ~AglWrapper();

  /// Query whether rendering context has direct access to the hardware.
  virtual bool isDirect() const;

  /// Query whether the rendering context is double-buffered.
  virtual bool isDoubleBuffered() const;

  /// Get the bit depth of the draw buffer(s).
  virtual unsigned int bitsPerPixel() const;

  /// Bind the rendering context to the given window.
  virtual void makeCurrent(Window win);

  /// Should be called when the corresponding window's geometry is changed.
  virtual void onReshape(int width, int height);

  /// Swaps buffers if in double-buffering mode.
  virtual void swapBuffers() const;

  /// Instantiate an actual Aqua window for the given Tk_Window.
  virtual Window makeTkRealWindow(Tk_Window tkwin, Window parent,
                                  int width, int height) throw();
};

//---------------------------------------------------------------------
//
// Implementation
//
//---------------------------------------------------------------------

#include "gfx/glxopts.h"

#include "rutz/error.h"

#define HAVE_LIMITS_H
#include <tkMacOSX.h>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  // Builds a formatted data array for constructing a GLX context.
  class AttribList
  {
  private:
    enum { MAXSIZE = 1000 };

    void push(GLint v)
    {
      GVX_ASSERT(next < MAXSIZE);
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
      push( rbits );          dbg_eval(3, rbits);
      push( AGL_GREEN_SIZE );
      push( gbits );          dbg_eval(3, gbits);
      push( AGL_BLUE_SIZE );
      push( bbits );          dbg_eval_nl(3, bbits);
      if (abits > 0)
        {
          push( AGL_ALPHA_SIZE );
          push( abits );      dbg_eval_nl(3, abits);
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
GVX_TRACE("AglWrapper::AglWrapper");

  // AGLDevice is a typedef for GDHandle
  AGLDevice gdev = GetMainDevice();
  const GLint ndev = 1;

  AttribList attribs(opts);

  itsPixFormat = aglChoosePixelFormat(&gdev, ndev, attribs.get());

  if (itsPixFormat == 0)
    throw rutz::error("couldn't choose Apple-OpenGL pixel format",
                      SRC_POS);

  AGLContext share = 0;

  itsContext = aglCreateContext(itsPixFormat, share);

  if (itsContext == 0)
    throw rutz::error("couldn't create Apple-OpenGL graphics context",
                      SRC_POS);
}

AglWrapper* AglWrapper::make(Display* dpy, GlxOpts& opts)
{
GVX_TRACE("AglWrapper::make");
  return new AglWrapper(dpy, opts);
}

AglWrapper::~AglWrapper()
{
GVX_TRACE("AglWrapper::~AglWrapper");

  aglDestroyPixelFormat(itsPixFormat);
  itsPixFormat = 0;

  aglDestroyContext(itsContext);
  itsContext = 0;
}

bool AglWrapper::isDirect() const
{
GVX_TRACE("AglWrapper::isDirect");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_ACCELERATED, &value);

  if (status == GL_FALSE)
    throw rutz::error("couldn't get Apple-OpenGL accelerated attribute",
                      SRC_POS);

  return (value == GL_TRUE);
}

bool AglWrapper::isDoubleBuffered() const
{
GVX_TRACE("AglWrapper::isDoubleBuffered");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_DOUBLEBUFFER, &value);

  if (status == GL_FALSE)
    throw rutz::error("couldn't get Apple-OpenGL doublebuffer attribute",
                      SRC_POS);

  return (value == GL_TRUE);
}

unsigned int AglWrapper::bitsPerPixel() const
{
GVX_TRACE("AglWrapper::bitsPerPixel");

  GLint value = 0;
  int status = aglDescribePixelFormat(itsPixFormat, AGL_PIXEL_SIZE, &value);

  if (status == GL_FALSE)
    throw rutz::error("couldn't get Apple-OpenGL pixel size attribute",
                      SRC_POS);

  GVX_ASSERT(value > 0);

  return static_cast<unsigned int>(value);
}

void AglWrapper::makeCurrent(Window win)
{
GVX_TRACE("AglWrapper::makeCurrent");

  // AGLDrawable is a typedef for CGrafPtr
  CGrafPtr drawable = TkMacOSXGetDrawablePort(win);

  if (GVX_DBG_LEVEL() >= 3)
    {
      WindowRef macWindow = GetWindowFromPort(drawable);
      Rect rectPort;
      GetWindowPortBounds (macWindow, &rectPort);
      dbg_eval(0, rectPort.right);
      dbg_eval_nl(0, rectPort.left);
      dbg_eval(0, rectPort.bottom);
      dbg_eval_nl(0, rectPort.top);
    }

  int status1 = aglSetDrawable(itsContext, drawable);

  if (status1 == GL_FALSE)
    throw rutz::error("couldn't set Apple-OpenGL drawable", SRC_POS);

  int status2 = aglSetCurrentContext(itsContext);

  if (status2 == GL_FALSE)
    throw rutz::error("couldn't set current Apple-OpenGL context", SRC_POS);

  int status3 = aglUpdateContext(itsContext);

  if (status3 == GL_FALSE)
    throw rutz::error("couldn't update Apple-OpenGL context", SRC_POS);
}

void AglWrapper::onReshape(int /*width*/, int /*height*/)
{
GVX_TRACE("AglWrapper::onReshape");

  int status = aglUpdateContext(itsContext);

  if (status == GL_FALSE)
    throw rutz::error("couldn't update Apple-OpenGL context", SRC_POS);
}

void AglWrapper::swapBuffers() const
{
GVX_TRACE("AglWrapper::swapBuffers");
  aglSwapBuffers(itsContext);
}

Window AglWrapper::makeTkRealWindow(Tk_Window tkwin, Window parent,
                                    int width, int height) throw()
{
GVX_TRACE("AglWrapper::makeTkRealWindow");

  GVX_ASSERT(false); // FIXME
  (void) tkwin;
  (void) parent;
  (void) width;
  (void) height;
  return (Window) 0;
}

static const char vcid_groovx_gfx_aglwrapper_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_AGLWRAPPER_H_UTC20050626084024_DEFINED
