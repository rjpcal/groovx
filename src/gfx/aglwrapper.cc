/** @file gfx/aglwrapper.cc GlWindowInterface implementation using
    Apple's AGL API */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Nov 11 16:28:30 2005
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifdef GVX_GL_PLATFORM_AGL

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_3

#include "gfx/aglwrapper.h"

#include "gfx/glxopts.h"

#include "rutz/error.h"

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

AglWrapper::AglWrapper(GlxOpts& opts) :
  itsPixFormat(nullptr),
  itsContext(nullptr),
  itsDrawable(nullptr)
{
GVX_TRACE("AglWrapper::AglWrapper");

  AttribList attribs(opts);

  itsPixFormat = aglCreatePixelFormat(attribs.get());

  if (itsPixFormat == nullptr)
    throw rutz::error("couldn't choose Apple-OpenGL pixel format",
                      SRC_POS);

  AGLContext share = nullptr;

  itsContext = aglCreateContext(itsPixFormat, share);

  if (itsContext == nullptr)
    throw rutz::error("couldn't create Apple-OpenGL graphics context",
                      SRC_POS);
}

AglWrapper* AglWrapper::make(GlxOpts& opts)
{
GVX_TRACE("AglWrapper::make");
  return new AglWrapper(opts);
}

AglWrapper::~AglWrapper()
{
GVX_TRACE("AglWrapper::~AglWrapper");

  aglDestroyPixelFormat(itsPixFormat);
  itsPixFormat = nullptr;

  aglDestroyContext(itsContext);
  itsContext = nullptr;
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

void AglWrapper::makeCurrent()
{
GVX_TRACE("AglWrapper::makeCurrent");

  GVX_ASSERT(itsDrawable != nullptr);

  int status1 = aglSetDrawable(itsContext, itsDrawable);

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

#pragma clang diagnostic pop

#endif // GVX_GL_PLATFORM_AGL
