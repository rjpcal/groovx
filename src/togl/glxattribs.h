///////////////////////////////////////////////////////////////////////
//
// glxattribs.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 16:36:13 2002
// written: Sat Aug 10 15:16:12 2002
// $Id$
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
    push( rbits );          DebugEval(rbits);
    push( GLX_GREEN_SIZE );
    push( gbits );          DebugEval(gbits);
    push( GLX_BLUE_SIZE );
    push( bbits );          DebugEvalNL(bbits);
    if (abits > 0)
      {
        push( GLX_ALPHA_SIZE );
        push( abits );      DebugEvalNL(abits);
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
