///////////////////////////////////////////////////////////////////////
//
// glxattribs.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Aug  3 16:36:13 2002
// written: Wed Mar 19 17:56:05 2003
// $Id$
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

#include <GL/glx.h>

#include "util/debug.h"

class GlxAttribs
{
private:
  void push(int v)
  {
    Assert(next < 1000);
    data[next++] = v;
  }

  void cap()
  {
    data[next] = None;
  }

  int data[1000];
  int next;

public:
  GlxAttribs() : next(0)
  {
    push( GLX_USE_GL );
  }

  int* get() { cap(); return data; }

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

  void level(int lev)
  {
    push( GLX_LEVEL );
    push( lev );
  }

  void colorIndex(int bits)
  {
    push( GLX_BUFFER_SIZE );
    push( bits );
  }

  void depthBuffer(int bits)
  {
    push( GLX_DEPTH_SIZE );
    push( bits );
  }

  void doubleBuffer()
  {
    push( GLX_DOUBLEBUFFER );
  }

  void stencilBuffer(int bits)
  {
    push( GLX_STENCIL_SIZE );
    push( bits );
  }

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

  void auxBuffers(int n)
  {
    push( GLX_AUX_BUFFERS );
    push( n );
  }

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
