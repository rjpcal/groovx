///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Sat Jul  3 12:28:25 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include <GL/gl.h>
#include <GL/glu.h>				  // for gluProject()
#include <cmath>					  // for abs()
#include <cstring>				  // for memcpy

#include "pbm.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "Bitmap";
}

Bitmap::Bitmap() :
  GrObj()
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(const char* filename) :
  GrObj(),
  itsFilename(filename)
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(istream& is, IOFlag flag) :
  GrObj(is, flag)
{
DOTRACE("Bitmap::Bitmap");
  init();
  deserialize(is, flag);
}

void Bitmap::init() {
DOTRACE("Bitmap::init");
  itsRasterX = itsRasterY = 0.0;
  itsZoomX = itsZoomY = 1.0;
  itsBytes = 0;
  itsContrastFlip = false;
  itsVerticalFlip = false;
  itsUsingGlBitmap = true;
}

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
  delete [] itsBytes;
  itsBytes = 0;
}

void Bitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Bitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsFilename << sep;
  os << itsRasterX << sep << itsRasterY << sep;
  os << itsZoomX << sep << itsZoomY << sep;
  os << itsContrastFlip << sep;
  os << itsVerticalFlip << sep;
  os << itsUsingGlBitmap << endl;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void Bitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("Bitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> itsFilename;
  is >> itsRasterX >> itsRasterY;
  is >> itsZoomX >> itsZoomY;
  int val;
  is >> val;
  itsContrastFlip = bool(val);
  is >> val;
  itsVerticalFlip = bool(val);
  is >> val;
  itsUsingGlBitmap = bool(val);

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag); }

  if ( !itsFilename.empty() ) {
	 loadPbmFile(itsFilename.c_str());
  }
}

int Bitmap::charCount() const {
DOTRACE("Bitmap::charCount");
  return 128;
}

int Bitmap::bytesPerRow() const {
DOTRACE("Bitmap::bytesPerRow");
  return ( (itsWidth*itsBitsPerPixel - 1)/8 + 1 );
}

int Bitmap::byteCount() const {
DOTRACE("Bitmap::byteCount");
  return bytesPerRow() * itsHeight;
}

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");
  Pbm pbm(filename);
  unsigned char* bytes;
  pbm.grabBytes(bytes, itsWidth, itsHeight, itsBitsPerPixel);
  
  delete [] itsBytes;
  itsBytes = bytes;
  
  // Wait to set itsFilename until we are sure that the new read
  // succeeded; otherwise, the contents of itsBytes will be out of
  // sync with itsFilename.
  itsFilename = filename;

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }
  
  sendStateChangeMsg();
}

void Bitmap::flipContrast() {
DOTRACE("Bitmap::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsContrastFlip = !itsContrastFlip;

  doFlipContrast();
}

void Bitmap::doFlipContrast() {
DOTRACE("Bitmap::doFlipContrast");
  int num_bytes = byteCount();

  // In this case we want to flip each bit
  if (itsBitsPerPixel == 1) {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes[i] ^= 0xff;
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes[i] = 0xff - itsBytes[i];
	 }
  }
  sendStateChangeMsg();
}

void Bitmap::flipVertical() {
DOTRACE("Bitmap::flipVertical");

  itsVerticalFlip = !itsVerticalFlip;

  doFlipVertical();
}

void Bitmap::doFlipVertical() {
DOTRACE("Bitmap::doFlipVertical");

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  unsigned char* new_bytes = new unsigned char[num_bytes];
  
  for (int row = 0; row < itsHeight; ++row) {
	 int new_row = (itsHeight-1)-row;
	 memcpy(static_cast<void*> (new_bytes + (new_row * bytes_per_row)),
			  static_cast<void*> (itsBytes  + (row     * bytes_per_row)),
			  bytes_per_row);
  }
  
  delete [] itsBytes;
  itsBytes = new_bytes;

  sendStateChangeMsg();
}

double Bitmap::getRasterX() const {
DOTRACE("Bitmap::getRasterX");
  return itsRasterX;
}

double Bitmap::getRasterY() const {
DOTRACE("Bitmap::getRasterY");
  return itsRasterY;
}

double Bitmap::getZoomX() const {
DOTRACE("Bitmap::getZoomX");
  return itsZoomX;
}

double Bitmap::getZoomY() const {
DOTRACE("Bitmap::getZoomY");
  return itsZoomY;
}

bool Bitmap::getUsingGlBitmap() const {
DOTRACE("Bitmap::getUsingGlBitmap");
  return itsUsingGlBitmap; 
}

void Bitmap::setRasterX(double val) {
DOTRACE("Bitmap::setRasterX");
  itsRasterX = val;
  sendStateChangeMsg();
}

void Bitmap::setRasterY(double val) {
DOTRACE("Bitmap::setRasterY");
  itsRasterY = val;
  sendStateChangeMsg();
}

void Bitmap::setZoomX(double val) {
DOTRACE("Bitmap::setZoomX");
  itsZoomX = val;
  sendStateChangeMsg();
}

void Bitmap::setZoomY(double val) {
DOTRACE("Bitmap::setZoomY");
  itsZoomY = val;
  sendStateChangeMsg();
}

void Bitmap::setUsingGlBitmap(bool val) {
DOTRACE("Bitmap::setUsingGlBitmap");
  itsUsingGlBitmap = val;
  sendStateChangeMsg();
}

void Bitmap::center() {
DOTRACE("Bitmap::center");
  GLdouble mv_matrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mv_matrix);

  GLdouble proj_matrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble left_x, bottom_y, right_x, top_y, dummy_z;

  GLint result = gluUnProject(viewport[0], viewport[1], 0,
										mv_matrix, proj_matrix, viewport,
										&left_x, &bottom_y, &dummy_z);

  result = gluUnProject(viewport[0]+itsWidth, viewport[1]+itsHeight, 0,
								mv_matrix, proj_matrix, viewport,
								&right_x, &top_y, &dummy_z);

  GLdouble win_width = abs(right_x - left_x);
  GLdouble win_height = abs(top_y - bottom_y);

  DebugEval(win_width); DebugEvalNL(win_height);

  itsRasterX = -win_width/2.0;
  itsRasterY = -win_height/2.0;
  
  if (!itsUsingGlBitmap) {
	 itsRasterX *= abs(itsZoomX);
	 itsRasterY *= abs(itsZoomY);
  }

  sendStateChangeMsg();
}

void Bitmap::undraw() const {
DOTRACE("Bitmap::undraw");
  GLdouble mv_matrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mv_matrix);
  
  // Projection matrix looks like this:
  //  _                                              _       
  // |      2                                         |      
  // | ------------      0            0        t_x    |      
  // | right - left                                   |      
  // |                   2                            |      
  // |      0       ------------      0        t_y    |      
  // |              top - bottom                      |      
  // |                               -2               |      
  // |      0            0       ----------    t_z    |      
  // |                           far - near           |      
  // |                                                |      
  // |      0            0            0         1     |       
  // |_                                              _|      


  GLdouble proj_matrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // Map itsRasterX/itsRasterY from GL coordinates to window coordinates
  GLdouble win_raster_x, win_raster_y, win_raster_z;
  GLint result = gluProject(itsRasterX, itsRasterY, 0.0,
									 mv_matrix, proj_matrix, viewport,
									 &win_raster_x, &win_raster_y, &win_raster_z);

  DebugEval(result);
  DebugEval(win_raster_x);
  DebugEval(win_raster_y);
  DebugEvalNL(win_raster_z);

  if (itsZoomX < 0.0) {
	 win_raster_x += itsWidth*itsZoomX;
  }
  if (itsZoomY < 0.0) {
	 win_raster_y += itsHeight*itsZoomY;
  }

  glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
      glScissor( int(win_raster_x)-1, 
					  int(win_raster_y)-1, 
					  int(itsWidth*abs(itsZoomX))+2, 
					  int(itsHeight*abs(itsZoomY))+2 );
		glClear(GL_COLOR_BUFFER_BIT);
	 glDisable(GL_SCISSOR_TEST);
  glPopAttrib();	 
}

void Bitmap::grRender() const {
DOTRACE("Bitmap::grRender");
  glRasterPos2d(itsRasterX, itsRasterY);
  glPixelZoom(itsZoomX, itsZoomY);
  
  if (itsBitsPerPixel == 24) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glDrawPixels(itsWidth, itsHeight, GL_RGB, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(itsBytes));
  }
  else if (itsBitsPerPixel == 8) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glDrawPixels(itsWidth, itsHeight, GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
					  static_cast<GLvoid*>(itsBytes));
  }
  else if (itsBitsPerPixel == 1) {
	 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 if (itsUsingGlBitmap) {
		glBitmap(itsWidth, itsHeight, 0.0, 0.0, 0.0, 0.0,
					static_cast<GLubyte*>(itsBytes));
	 }
	 else {
		glDrawPixels(itsWidth, itsHeight, GL_COLOR_INDEX, GL_BITMAP,
						 static_cast<GLvoid*>(itsBytes));
	 }
  }
}

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
