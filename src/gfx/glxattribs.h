///////////////////////////////////////////////////////////////////////
//
// glxattribs.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Aug  3 16:36:13 2002
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

#ifndef GLXATTRIBS_H_DEFINED
#define GLXATTRIBS_H_DEFINED

#include "gfx/glxopts.h"

#include <GL/glx.h>

#include "util/debug.h"
DBG_REGISTER

/// Builds a formatted data array for constructing a GLX context.
class GlxAttribs
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
  GlxAttribs(const GlxOpts& opts) : next(0)
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

static const char vcid_glxattribs_h[] = "$Header$";
#endif // !GLXATTRIBS_H_DEFINED
