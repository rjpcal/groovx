///////////////////////////////////////////////////////////////////////
//
// glbmaprenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:52:41 1999
// written: Wed Aug  8 08:07:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBMAPRENDERER_CC_DEFINED
#define GLBMAPRENDERER_CC_DEFINED

#include "glbmaprenderer.h"

#include "bmapdata.h"
#include "point.h"

#include "gwt/canvas.h"

#include <GL/gl.h>

#include "util/trace.h"

GLBmapRenderer::GLBmapRenderer () :
  itsUsingGlBitmap(true)
{
DOTRACE("GLBmapRenderer::GLBmapRenderer ");
}

GLBmapRenderer::~GLBmapRenderer ()
{
DOTRACE("GLBmapRenderer::~GLBmapRenderer ");
}

void GLBmapRenderer::doRender(GWT::Canvas& canvas,
                              const BmapData& data,
                              const Point<double>& world_pos,
                              const Point<double>& zoom) const
{
DOTRACE("GLBmapRenderer::doRender");
  glRasterPos2d(world_pos.x(), world_pos.y());
  glPixelZoom(zoom.x(), zoom.y());

  glPixelStorei(GL_UNPACK_ALIGNMENT, data.byteAlignment());

  if (data.bitsPerPixel() == 24)
    {
      glDrawPixels(data.width(), data.height(), GL_RGB, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
  else if (data.bitsPerPixel() == 8)
    {
      glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
  else if (data.bitsPerPixel() == 1)
    {
      if (itsUsingGlBitmap)
        {
          glBitmap(data.width(), data.height(), 0.0, 0.0, 0.0, 0.0,
                   static_cast<GLubyte*>(data.bytesPtr()));
        }
      else
        {
          if (canvas.isRgba())
            {
              GLfloat simplemap[] = {0.0, 1.0};

              glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, simplemap);
              glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, simplemap);
              glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, simplemap);
              glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, simplemap);
            }

          glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX, GL_BITMAP,
                       static_cast<GLvoid*>(data.bytesPtr()));
        }
    }
}

static const char vcid_glbmaprenderer_cc[] = "$Header$";
#endif // !GLBMAPRENDERER_CC_DEFINED
