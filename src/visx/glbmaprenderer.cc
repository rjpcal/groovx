///////////////////////////////////////////////////////////////////////
//
// glbmaprenderer.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:52:41 1999
// written: Fri Jan 18 16:07:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBMAPRENDERER_CC_DEFINED
#define GLBMAPRENDERER_CC_DEFINED

#include "visx/glbmaprenderer.h"

#include "gfx/canvas.h"

#include "gx/bmapdata.h"

#include "util/trace.h"

GLBmapRenderer::GLBmapRenderer () :
  itsUsingGlBitmap(false)
{
DOTRACE("GLBmapRenderer::GLBmapRenderer ");
}

GLBmapRenderer::~GLBmapRenderer ()
{
DOTRACE("GLBmapRenderer::~GLBmapRenderer ");
}

void GLBmapRenderer::doRender(Gfx::Canvas& canvas,
                              const Gfx::BmapData& data,
                              const Gfx::Vec2<double>& world_pos,
                              const Gfx::Vec2<double>& zoom) const
{
DOTRACE("GLBmapRenderer::doRender");
  if (data.bitsPerPixel() == 1 && itsUsingGlBitmap)
    {
      canvas.drawBitmap(data, world_pos);
    }
  else
    {
      canvas.drawPixels(data, world_pos, zoom);
    }
}

static const char vcid_glbmaprenderer_cc[] = "$Header$";
#endif // !GLBMAPRENDERER_CC_DEFINED
