///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:17 1999
// written: Mon Sep 20 09:57:21 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include <GL/gl.h>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "GLBitmap";
}

GLBitmap::GLBitmap() :
  Bitmap()
{
DOTRACE("GLBitmap::GLBitmap");
  init();
}

GLBitmap::GLBitmap(const char* filename) :
  Bitmap(filename)
{
DOTRACE("GLBitmap::GLBitmap");
  init(); 
}

GLBitmap::GLBitmap(istream& is, IOFlag flag) :
  Bitmap()
{
DOTRACE("GLBitmap::GLBitmap");
  deserialize(is, flag);
}

void GLBitmap::init() {
DOTRACE("GLBitmap::init");
  itsUsingGlBitmap = true;
  setUsingZoom(true);
}

GLBitmap::~GLBitmap() {
DOTRACE("GLBitmap::~GLBitmap");
}

void GLBitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("GLBitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsUsingGlBitmap << sep;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { Bitmap::serialize(os, flag); }
}

void GLBitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("GLBitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  int val;
  is >> val;
  itsUsingGlBitmap = bool(val);

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { Bitmap::deserialize(is, flag); }
}

int GLBitmap::charCount() const {
DOTRACE("GLBitmap::charCount");
  return 128;
}

bool GLBitmap::getUsingGlBitmap() const {
DOTRACE("GLBitmap::getUsingGlBitmap");
  return itsUsingGlBitmap; 
}

void GLBitmap::setUsingGlBitmap(bool val) {
DOTRACE("GLBitmap::setUsingGlBitmap");
  itsUsingGlBitmap = val;

  // glPixelZoom() does not work with glBitmap()
  if (itsUsingGlBitmap) {
	 setUsingZoom(false);
  }

  sendStateChangeMsg();
}

void GLBitmap::doUndraw(int winRasterX, int winRasterY,
								int winWidthX, int winHeightY) const {
DOTRACE("GLBitmap::doUndraw");
  glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
      glScissor(winRasterX, winRasterY, winWidthX, winHeightY);
		glClear(GL_COLOR_BUFFER_BIT);
	 glDisable(GL_SCISSOR_TEST);
  glPopAttrib();	 
}

void GLBitmap::doRender(unsigned char* bytes,
								double x_pos,
								double y_pos,
								int width,
								int height,
								int bits_per_pixel,
								double zoom_x,
								double zoom_y) const {
DOTRACE("GLBitmap::doRender");
  glRasterPos2d(x_pos, y_pos);
  glPixelZoom(zoom_x, zoom_y);
  
  if (bits_per_pixel == 24) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(bytes));
  }
  else if (bits_per_pixel == 8) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glDrawPixels(width, height, GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(bytes));
  }
  else if (bits_per_pixel == 1) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 if (itsUsingGlBitmap) {
		glBitmap(width, height, 0.0, 0.0, 0.0, 0.0,
					static_cast<GLubyte*>(bytes));
	 }
	 else {
		glDrawPixels(width, height, GL_COLOR_INDEX, GL_BITMAP,
						 static_cast<GLvoid*>(bytes));
	 }
  }
}

static const char vcid_glbitmap_cc[] = "$Header$";
#endif // !GLBITMAP_CC_DEFINED
