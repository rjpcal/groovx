///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Thu Jun 24 14:48:09 1999
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
#define LOCAL_DEBUG
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
  os << itsVerticalFlip << endl;

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

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");
  itsFilename = filename;

  Pbm pbm(itsFilename.c_str());
  unsigned char* bytes;
  pbm.grabBytes(bytes, itsWidth, itsHeight, itsBitsPerPixel);
  
  delete [] itsBytes;
  itsBytes = bytes;
  
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
  int num_bytes = (itsWidth*itsHeight*itsBitsPerPixel)/8 + 1;

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

  int bytes_per_row = (itsWidth*itsBitsPerPixel)/8 + 1;
  int num_bytes = bytes_per_row * itsHeight;
  
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

double Bitmap::getRasterX() const {
DOTRACE("Bitmap::getRasterX");
  return itsRasterX;
}

double Bitmap::getRasterY() const {
DOTRACE("Bitmap::getRasterY");
  return itsRasterY;
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

double Bitmap::getZoomX() const {
DOTRACE("Bitmap::getZoomX");
  return itsZoomX;
}

double Bitmap::getZoomY() const {
DOTRACE("Bitmap::getZoomY");
  return itsZoomY;
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

  glEnable(GL_SCISSOR_TEST);
    glScissor( int(win_raster_x)-1, 
					int(win_raster_y)-1, 
					int(itsWidth*abs(itsZoomX))+2, 
					int(itsHeight*abs(itsZoomY))+2 );
    glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);
}

void Bitmap::grRecompile() const {
DOTRACE("Bitmap::grRecompile");
  grNewList();

  glNewList(grDisplayList(), GL_COMPILE);
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
//   		glDrawPixels(itsWidth, itsHeight, GL_COLOR_INDEX, GL_BITMAP,
//   						 static_cast<GLvoid*>(itsBytes));
 		glBitmap(itsWidth, itsHeight, 0.0, 0.0, 0.0, 0.0,
 					static_cast<GLubyte*>(itsBytes));
	 }
  glEndList();

  grPostUpdated();
}

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
