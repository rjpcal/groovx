///////////////////////////////////////////////////////////////////////
//
// glxopts.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  4 16:29:23 2002
// written: Mon Jan 13 11:01:39 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLXOPTS_CC_DEFINED
#define GLXOPTS_CC_DEFINED

#include "glxopts.h"

#include "gfx/glxattribs.h"

#include "util/pointers.h"

#include "util/trace.h"

shared_ptr<GlxAttribs> GlxOpts::buildAttribList()
{
DOTRACE("GlxOpts::buildAttribList");

  shared_ptr<GlxAttribs> attribs(new GlxAttribs);

  if (rgbaFlag)        attribs->rgba(rgbaRed, rgbaGreen, rgbaBlue,
                                     alphaFlag ? alphaSize : -1);

  else                 attribs->colorIndex( colorIndexSize );

  if (depthFlag)       attribs->depthBuffer( depthSize );

  if (doubleFlag)      attribs->doubleBuffer();

  if (stencilFlag)     attribs->stencilBuffer( stencilSize );

  if (accumFlag)       attribs->accum(accumRed, accumGreen, accumBlue,
                                      alphaFlag ? accumAlpha : -1);

  if (auxNumber > 0)   attribs->auxBuffers( auxNumber );

  if (level != 0)      attribs->level( level );

  if (transparent)     attribs->transparent();

  return attribs;
}

static const char vcid_glxopts_cc[] = "$Header$";
#endif // !GLXOPTS_CC_DEFINED
