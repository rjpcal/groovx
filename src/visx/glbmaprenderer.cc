///////////////////////////////////////////////////////////////////////
//
// glbmaprenderer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:52:41 1999
// written: Mon Jun 11 15:08:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBMAPRENDERER_CC_DEFINED
#define GLBMAPRENDERER_CC_DEFINED

#include "glbmaprenderer.h"

#include "gwt/canvas.h"

#include <GL/gl.h>

#include "util/trace.h"

GLBmapRenderer::GLBmapRenderer () :
  itsUsingGlBitmap(true)
{
DOTRACE("GLBmapRenderer::GLBmapRenderer ");
}

GLBmapRenderer::~GLBmapRenderer () {
DOTRACE("GLBmapRenderer::~GLBmapRenderer ");
}

void GLBmapRenderer::doRender(GWT::Canvas& canvas,
										unsigned char* bytes,
										double x_pos,
										double y_pos,
										int width,
										int height,
										int bits_per_pixel,
										int byte_alignment,
										double zoom_x,
										double zoom_y) const {
DOTRACE("GLBmapRenderer::doRender");
  glRasterPos2d(x_pos, y_pos);
  glPixelZoom(zoom_x, zoom_y);

  glPixelStorei(GL_UNPACK_ALIGNMENT, byte_alignment);

  if (bits_per_pixel == 24) {
	 glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(bytes));
  }
  else if (bits_per_pixel == 8) {
	 glDrawPixels(width, height, GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(bytes));
  }
  else if (bits_per_pixel == 1) {
	 if (itsUsingGlBitmap) {
		glBitmap(width, height, 0.0, 0.0, 0.0, 0.0,
					static_cast<GLubyte*>(bytes));
	 }
	 else {

		if (canvas.isRgba())
		  {
			 GLfloat simplemap[] = {0.0, 1.0};

			 glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, simplemap);
			 glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, simplemap);
			 glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, simplemap);
			 glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, simplemap);
		  }

		glDrawPixels(width, height, GL_COLOR_INDEX, GL_BITMAP,
						 static_cast<GLvoid*>(bytes));
	 }
  }
}

static const char vcid_glbmaprenderer_cc[] = "$Header$";
#endif // !GLBMAPRENDERER_CC_DEFINED
