/** @file gfx/aglwrapper.cc GlWindowInterface implementation using
    Apple's AGL API */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Nov 11 16:28:30 2005
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#define GVX_TRACE_EXPR true

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  // Builds a formatted data array for constructing a GLX context.
  class AttribList
  {
  private:
    static constexpr int MAXSIZE = 1000;

    void push(GLint v)
    {
      GVX_ASSERT(next < MAXSIZE);
      data[next++] = v;
    }

    void pushNS(NSOpenGLPixelFormatAttribute v)
    {
      GVX_ASSERT(nextNS < MAXSIZE);
      dataNS[nextNS++] = v;
    }

    // void cap()
    // {
    //   data[next] = AGL_NONE;
    // }

    void capNS()
    {
      dataNS[nextNS] = 0;
    }

    GLint data[MAXSIZE];
    int next;

    NSOpenGLPixelFormatAttribute dataNS[MAXSIZE];
    int nextNS;

  public:
    /// Default constructor.
    AttribList(const GlxOpts& opts) : next(0), nextNS(0)
    {
      if (opts.rgbaFlag)        this->rgba(opts.rgbaRed, opts.rgbaGreen, opts.rgbaBlue,
                                           opts.alphaFlag ? opts.alphaSize : -1);

      else                      this->colorIndex( opts.colorIndexSize );

      if (opts.depthFlag)       this->depthBuffer( (unsigned int) opts.depthSize );

      if (opts.doubleFlag)      this->doubleBuffer();

      if (opts.stencilFlag)     this->stencilBuffer( (unsigned int) opts.stencilSize );

      if (opts.accumFlag)       this->accum(opts.accumRed, opts.accumGreen, opts.accumBlue,
                                            opts.alphaFlag ? opts.accumAlpha : -1);

      if (opts.auxNumber > 0)   this->auxBuffers( (unsigned int) opts.auxNumber );

      if (opts.level != 0)      this->level( opts.level );

      if (opts.transparent)     this->transparent();

      if (!opts.indirect)       this->accelerated();
    }

    // GLint* get() { cap(); return data; }

    /// Get the underlying data array to use to construct a NSOpenGLPixelFormat.
    NSOpenGLPixelFormatAttribute* getNS() { capNS(); return dataNS; }

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
      pushNS( NSOpenGLPFAColorSize );
      pushNS( (unsigned int)(rbits + gbits + bbits) );
      pushNS( NSOpenGLPFAClosestPolicy );
      if (abits > 0)
        {
          push( AGL_ALPHA_SIZE ); pushNS( NSOpenGLPFAAlphaSize );
          push( abits ); pushNS( (unsigned int) abits );      dbg_eval_nl(3, abits);
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
    void depthBuffer(unsigned int bits)
    {
      push( AGL_DEPTH_SIZE ); pushNS( NSOpenGLPFADepthSize );
      push( (int) bits ); pushNS( bits );
    }

    /// Request double-buffering mode.
    void doubleBuffer()
    {
      push( AGL_DOUBLEBUFFER ); pushNS( NSOpenGLPFADoubleBuffer );
    }

    /// Specify a minimum bit-depth for the stencil buffer.
    void stencilBuffer(unsigned int bits)
    {
      push( AGL_STENCIL_SIZE ); pushNS( NSOpenGLPFAStencilSize );
      push( (int) bits ); pushNS( bits );
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
    void auxBuffers(unsigned int n)
    {
      push( AGL_AUX_BUFFERS ); pushNS( NSOpenGLPFAAuxBuffers );
      push( (int) n ); pushNS( n );
    }

    /// Request a hardware-accelarted renderer.
    void accelerated()
    {
      push( AGL_ACCELERATED ); pushNS( NSOpenGLPFAAccelerated );
    }

    /// Request transparency if available.
    void transparent()
    {
      // FIXME not implementable?
    }
  };
}

AglWrapper::AglWrapper(GlxOpts& opts) :
  // itsPixFormat(nullptr),
  // itsContext(nullptr),
  // itsDrawable(nullptr),
  itsPixFormatNS(nil),
  itsContextNS(nil),
  itsViewNS(nil)
{
GVX_TRACE("AglWrapper::AglWrapper");

  AttribList attribs(opts);

  // itsPixFormat = aglCreatePixelFormat(attribs.get());

  // if (itsPixFormat == nullptr)
  //   throw rutz::error("couldn't choose Apple-OpenGL pixel format",
  //                     SRC_POS);

  NSOpenGLPixelFormatAttribute glAttributes[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFASampleBuffers, 0,
        0,
    };

  // itsPixFormatNS = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs.getNS()];
  void(attribs.getNS());
  itsPixFormatNS = [[NSOpenGLPixelFormat alloc] initWithAttributes:glAttributes];

  if (itsPixFormatNS == nullptr)
    throw rutz::error("couldn't choose NSOpenGLPixelFormat",
                      SRC_POS);

  dbg_eval_nl(0, (void*)itsPixFormatNS);

  // AGLContext share = nullptr;

  // itsContext = aglCreateContext(itsPixFormat, share);

  // if (itsContext == nullptr)
  //   throw rutz::error("couldn't create Apple-OpenGL graphics context",
  //                     SRC_POS);

  itsContextNS = [[NSOpenGLContext alloc] initWithFormat:itsPixFormatNS shareContext:nil];

  if (itsContextNS == nullptr)
    throw rutz::error("couldn't create NSOpenGLContext",
                      SRC_POS);

  dbg_eval_nl(0, (void*)itsContextNS);
}

AglWrapper* AglWrapper::make(GlxOpts& opts)
{
GVX_TRACE("AglWrapper::make");
  return new AglWrapper(opts);
}

AglWrapper::~AglWrapper()
{
GVX_TRACE("AglWrapper::~AglWrapper");

  // aglDestroyPixelFormat(itsPixFormat);
  // itsPixFormat = nullptr;

  // aglDestroyContext(itsContext);
  // itsContext = nullptr;

  // FIXME clean up here
}

bool AglWrapper::isDirect() const
{
GVX_TRACE("AglWrapper::isDirect");

  // GLint value = 0;
  // int status = aglDescribePixelFormat(itsPixFormat, AGL_ACCELERATED, &value);

  // if (status == GL_FALSE)
  //   throw rutz::error("couldn't get Apple-OpenGL accelerated attribute",
  //                     SRC_POS);

  // return (value == GL_TRUE);

  return true; // FIXME
}

bool AglWrapper::isDoubleBuffered() const
{
GVX_TRACE("AglWrapper::isDoubleBuffered");

  // GLint value = 0;
  // int status = aglDescribePixelFormat(itsPixFormat, AGL_DOUBLEBUFFER, &value);

  // if (status == GL_FALSE)
  //   throw rutz::error("couldn't get Apple-OpenGL doublebuffer attribute",
  //                     SRC_POS);

  // return (value == GL_TRUE);

  return true; // FIXME
}

unsigned int AglWrapper::bitsPerPixel() const
{
GVX_TRACE("AglWrapper::bitsPerPixel");

  GLint value = 0;
  // int status = aglDescribePixelFormat(itsPixFormat, AGL_PIXEL_SIZE, &value);

  // if (status == GL_FALSE)
  //   throw rutz::error("couldn't get Apple-OpenGL pixel size attribute",
  //                     SRC_POS);

  GVX_ASSERT(value > 0);

  return static_cast<unsigned int>(value);
}

void AglWrapper::makeCurrent()
{
GVX_TRACE("AglWrapper::makeCurrent");

  // GVX_ASSERT(itsDrawable != nullptr);

  // int status1 = aglSetDrawable(itsContext, itsDrawable);

  // if (status1 == GL_FALSE)
  //   throw rutz::error("couldn't set Apple-OpenGL drawable", SRC_POS);

  // int status2 = aglSetCurrentContext(itsContext);

  // if (status2 == GL_FALSE)
  //   throw rutz::error("couldn't set current Apple-OpenGL context", SRC_POS);

  [itsContextNS makeCurrentContext];

  GVX_ASSERT(itsViewNS != nil);

  [itsContextNS setView:itsViewNS];

  // int status3 = aglUpdateContext(itsContext);

  // if (status3 == GL_FALSE)
  //   throw rutz::error("couldn't update Apple-OpenGL context", SRC_POS);

  [itsContextNS update];
  dbg_print_nl(0, "makeCurrent");

  [itsViewNS display];
}

void AglWrapper::onReshape(int /*width*/, int /*height*/)
{
GVX_TRACE("AglWrapper::onReshape");

  // int status = aglUpdateContext(itsContext);

  // if (status == GL_FALSE)
  //   throw rutz::error("couldn't update Apple-OpenGL context", SRC_POS);

  dbg_print_nl(0, "reshape");
  [itsContextNS update];
}

void AglWrapper::swapBuffers() const
{
GVX_TRACE("AglWrapper::swapBuffers");
  // aglSwapBuffers(itsContext);
  dbg_print_nl(0, "flush");

  [itsContextNS flushBuffer];
}

#pragma clang diagnostic pop

#endif // GVX_GL_PLATFORM_AGL
