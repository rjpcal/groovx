///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Wed Jun 16 17:37:35 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstring>				  // for memcpy

#include "pbm.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

Bitmap::Bitmap() :
  GrObj(),
  itsRasterX(0.0), itsRasterY(0.0),
  itsZoomX(1.0), itsZoomY(1.0),
  itsBytes(0)
{
DOTRACE("Bitmap::Bitmap");

}

Bitmap::Bitmap(const char* filename) :
  GrObj(),
  itsFilename(filename),
  itsRasterX(0.0), itsRasterY(0.0),
  itsZoomX(1.0), itsZoomY(1.0),
  itsBytes(0)
{
DOTRACE("Bitmap::Bitmap");
}

Bitmap::Bitmap(istream& is, IOFlag flag) :
  GrObj(is, flag),
  itsBytes(0)
{
DOTRACE("Bitmap::Bitmap");
  deserialize(is, flag);
}

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
}

void Bitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Bitmap::serialize");
}

void Bitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("Bitmap::deserialize");
}

int Bitmap::charCount() const {
DOTRACE("Bitmap::charCount");
  return 0;
}

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");
  itsFilename = filename;
  Pbm pbm(itsFilename.c_str());
  unsigned char* bytes;
  pbm.grabBytes(bytes, itsWidth, itsHeight, itsBitsPerPixel);
  itsBytes = auto_ptr<unsigned char>(bytes);

  sendStateChangeMsg();
}

void Bitmap::flipContrast() {
DOTRACE("Bitmap::flipContrast");

  int num_bytes = (itsWidth*itsHeight*itsBitsPerPixel)/8 + 1;
	 
  // In this case we want to flip each bit
  if (itsBitsPerPixel == 1) {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes.get()[i] ^= 0xff;
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes.get()[i] = 0xff - itsBytes.get()[i];
	 }
  }
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
						 static_cast<GLvoid*>(itsBytes.get()));
	 }
	 else if (itsBitsPerPixel == 8) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glDrawPixels(itsWidth, itsHeight, GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
						 static_cast<GLvoid*>(itsBytes.get()));
	 }
	 else if (itsBitsPerPixel == 1) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
 		glDrawPixels(itsWidth, itsHeight, GL_COLOR_INDEX, GL_BITMAP,
 						 static_cast<GLvoid*>(itsBytes.get()));
	 }
  glEndList();

  grPostUpdated();
}

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
