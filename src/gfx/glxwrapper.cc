/** @file gfx/glxwrapper.cc GlWindowInterface implementation using the
    X11 GLX API. */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Nov 11 16:10:15 2005
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GFX_GLXWRAPPER_CC_UTC20051112001015_DEFINED
#define GROOVX_GFX_GLXWRAPPER_CC_UTC20051112001015_DEFINED

#ifdef GVX_GL_PLATFORM_GLX

#include "gfx/glxwrapper.h"

#include "gfx/glxopts.h"

#include "rutz/error.h"
#include "rutz/shared_ptr.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

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
      GVX_ASSERT(next < MAXSIZE);
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
      push( rbits );          dbg_eval(3, rbits);
      push( GLX_GREEN_SIZE );
      push( gbits );          dbg_eval(3, gbits);
      push( GLX_BLUE_SIZE );
      push( bbits );          dbg_eval_nl(3, bbits);
      if (abits > 0)
        {
          push( GLX_ALPHA_SIZE );
          push( abits );      dbg_eval_nl(3, abits);
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
GVX_TRACE("GlxWrapper::GlxWrapper");

  int dummy;
  if (!glXQueryExtension(dpy, &dummy, &dummy))
    {
      throw rutz::error("GlxWrapper: X server has no OpenGL GLX extension", SRC_POS);
    }

  AttribList attribs(opts);

  dbg_eval_nl(3, (void*)itsDisplay);
  dbg_eval_nl(3, DefaultScreen(itsDisplay));

  int errorBase = 0;
  int eventBase = 0;

  if (False == glXQueryExtension(itsDisplay, &errorBase, &eventBase))
    {
      throw rutz::error("no GLX extension on the X server", SRC_POS);
    }

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
      throw rutz::error("couldn't find a matching visual", SRC_POS);
    }

  dbg_eval_nl(3, (void*)itsVisInfo->visualid);
  dbg_eval_nl(3, itsVisInfo->depth);
  dbg_eval_nl(3, itsVisInfo->bits_per_rgb);

  itsContext = glXCreateContext(itsDisplay,
                                itsVisInfo,
                                share ? share->itsContext : None,
                                opts.indirect ? GL_FALSE : GL_TRUE);

  dbg_eval_nl(3, itsContext);

  if (itsContext == 0)
    {
      throw rutz::error("could not create GL rendering context", SRC_POS);
    }
}

GlxWrapper::~GlxWrapper()
{
GVX_TRACE("GlxWrapper::~GlxWrapper");

  glXDestroyContext(itsDisplay, itsContext);
  XFree(itsVisInfo);
}

GlxWrapper* GlxWrapper::make(Display* dpy, GlxOpts& opts, GlxWrapper* share)
{
GVX_TRACE("GlxWrapper::make");

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

void GlxWrapper::makeCurrent()
{
GVX_TRACE("GlxWrapper::makeCurrent");

  GVX_ASSERT(itsCurrentWin != 0);

  if (currentGlxWrapper != this)
    {
      glXMakeCurrent(itsDisplay, itsCurrentWin, itsContext);
      currentGlxWrapper = this;
    }
}

void GlxWrapper::onReshape(int /*width*/, int /*height*/)
{
  // nothing to do here
}

bool GlxWrapper::isDoubleBuffered() const
{
GVX_TRACE("GlxWrapper::isDoubleBuffered");

  int dbl_flag;

  if (glXGetConfig(itsDisplay, itsVisInfo, GLX_DOUBLEBUFFER, &dbl_flag) != 0)
    {
      throw rutz::error("glXGetConfig failed", SRC_POS);
    }

  return bool(dbl_flag);
}

unsigned int GlxWrapper::bitsPerPixel() const
{
GVX_TRACE("GlxWrapper::bitsPerPixel");
  return itsVisInfo->depth;
}

void GlxWrapper::swapBuffers() const
{
  glXSwapBuffers(itsDisplay, itsCurrentWin);
}

#endif // GVX_GL_PLATFORM_GLX

static const char __attribute__((used)) vcid_groovx_gfx_glxwrapper_cc_utc20051112001015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GLXWRAPPER_CC_UTC20051112001015DEFINED
