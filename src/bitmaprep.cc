///////////////////////////////////////////////////////////////////////
//
// bitmaprep.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 20:18:32 1999
// written: Mon Dec  6 21:49:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_CC_DEFINED
#define BITMAPREP_CC_DEFINED

#include "bitmaprep.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cctype>
#include <cstring>				  // for memcpy
#include <cmath>					  // for abs

#include "bmaprenderer.h"
#include "glcanvas.h"
#include "error.h"
#include "io.h"
#include "pbm.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "Bitmap";
}

///////////////////////////////////////////////////////////////////////
//
// BitmapRep member definitions
//
///////////////////////////////////////////////////////////////////////

void BitmapRep::init() {
DOTRACE("BitmapRep::init");
  itsRasterX = itsRasterY = 0.0;
  itsZoomX = itsZoomY = 1.0;
  itsUsingZoom = false;
  itsBytes.resize(1);
  itsContrastFlip = false;
  itsVerticalFlip = false;

  itsHeight = 1;
  itsWidth = 1;
  itsBitsPerPixel = 1;
  itsByteAlignment = 1;
}

void BitmapRep::serialize(ostream& os, int flag) const {
DOTRACE("BitmapRep::serialize");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  os << itsFilename << '\t';
  os << itsRasterX << sep << itsRasterY << sep;
  os << itsZoomX << sep << itsZoomY << sep;
  os << itsUsingZoom << sep;
  os << itsContrastFlip << sep;
  os << itsVerticalFlip << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void BitmapRep::deserialize(istream& is, int flag) {
DOTRACE("BitmapRep::deserialize");
  if (flag & IO::TYPENAME) { IO::readTypename(is, ioTag); }

  IO::eatWhitespace(is);
  getline(is, itsFilename, '\t');

  is >> itsRasterX >> itsRasterY;
  is >> itsZoomX >> itsZoomY;

  int val;
  is >> val;
  itsUsingZoom = bool(val);

  is >> val;
  itsContrastFlip = bool(val);

  is >> val;
  itsVerticalFlip = bool(val);

  if (is.fail()) throw InputError(ioTag);

  itsBytes.resize(1);
  itsRenderer->notifyBytesChanged();

  if ( !itsFilename.empty() ) {
	 loadPbmFile(itsFilename.c_str());
  }
}

int BitmapRep::charCount() const {
DOTRACE("BitmapRep::charCount");
  return 128;
}

void BitmapRep::readFrom(Reader* reader) {
DOTRACE("BitmapRep::readFrom");
  reader->readValue("filename", itsFilename);
  reader->readValue("rasterX", itsRasterX);
  reader->readValue("rasterY", itsRasterY);
  reader->readValue("zoomX", itsZoomX);
  reader->readValue("zoomY", itsZoomY);
  reader->readValue("usingZoom", itsUsingZoom);
  reader->readValue("contrastFlip", itsContrastFlip);
  reader->readValue("verticalFlip", itsVerticalFlip);
}

void BitmapRep::writeTo(Writer* writer) const {
DOTRACE("BitmapRep::writeTo");
  writer->writeValue("filename", itsFilename);
  writer->writeValue("rasterX", itsRasterX);
  writer->writeValue("rasterY", itsRasterY);
  writer->writeValue("zoomX", itsZoomX);
  writer->writeValue("zoomY", itsZoomY);
  writer->writeValue("usingZoom", itsUsingZoom);
  writer->writeValue("contrastFlip", itsContrastFlip);
  writer->writeValue("verticalFlip", itsVerticalFlip);
}

/////////////
// actions //
/////////////

void BitmapRep::loadPbmFile(const char* filename) {
DOTRACE("BitmapRep::loadPbmFile(const char*)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(filename);

  // Grab ownership of the bitmap data from pbm into this object's itsBytes.
  pbm.grabBytes(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  itsFilename = filename;

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::loadPbmFile(istream& is) {
DOTRACE("BitmapRep::loadPbmFile(istream&)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(is);

  // Grab ownership of the bitmap data from pbm into this object's itsBytes.
  pbm.grabBytes(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  itsFilename = "(direct_from_stream)";

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::writePbmFile(const char* filename) const {
DOTRACE("BitmapRep::writePbmFile");
  Pbm pbm(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  pbm.write(filename);
}

void BitmapRep::grabScreenRect(int left, int top, int right, int bottom) {
  grabScreenRect(Rect<int>(left, top, right, bottom));
}

void BitmapRep::grabScreenRect(const Rect<int>& rect) {
DOTRACE("BitmapRep::grabScreenRect");
  DebugEval(rect.left()); DebugEval(rect.right());
  DebugEval(rect.bottom()); DebugEval(rect.top());

  itsHeight = abs(rect.height());    DebugEvalNL(itsHeight); 
  itsWidth = rect.width();           DebugEval(itsWidth);
  itsBitsPerPixel = 1;
  itsByteAlignment = 1;
  
  itsFilename = "";
  itsRasterY = itsRasterX = 0.0;
  itsZoomY = itsZoomX = 1.0;
  itsUsingZoom = false;
  itsContrastFlip = false;
  itsVerticalFlip = false;

  int num_new_bytes = (itsWidth/8 + 1) * itsHeight + 1;
  Assert(num_new_bytes>0);
  vector<unsigned char> newBytes( num_new_bytes );

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), itsWidth, itsHeight,
					GL_COLOR_INDEX, GL_BITMAP, &(newBytes[0]));

  itsBytes.swap(newBytes);
  itsRenderer->notifyBytesChanged();
}

void BitmapRep::grabWorldRect(double left, double top,
									double right, double bottom) {
  grabWorldRect(Rect<double>(left, top, right, bottom));
}

void BitmapRep::grabWorldRect(const Rect<double>& world_rect) {
DOTRACE("BitmapRep::grabWorldRect");
  Rect<int> screen_rect =
	 GLCanvas::theCanvas().getScreenFromWorld(world_rect);

  grabScreenRect(screen_rect);
}

void BitmapRep::flipContrast() {
DOTRACE("BitmapRep::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsContrastFlip = !itsContrastFlip;

  doFlipContrast();
}

void BitmapRep::doFlipContrast() {
DOTRACE("BitmapRep::doFlipContrast");
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

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::flipVertical() {
DOTRACE("BitmapRep::flipVertical");

  itsVerticalFlip = !itsVerticalFlip;

  doFlipVertical();
}

void BitmapRep::doFlipVertical() {
DOTRACE("BitmapRep::doFlipVertical");

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  vector<unsigned char> new_bytes(num_bytes);
  
  for (int row = 0; row < itsHeight; ++row) {
	 int new_row = (itsHeight-1)-row;
	 memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
			  static_cast<void*> (&(itsBytes [row     * bytes_per_row])),
			  bytes_per_row);
  }
  
  itsBytes.swap(new_bytes);

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::center() {
DOTRACE("BitmapRep::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Rect<int> screen_pos;
  screen_pos.left() = viewport[0];
  screen_pos.right() = viewport[0] + itsWidth;
  screen_pos.bottom() = viewport[1];
  screen_pos.top() = viewport[1] + itsHeight;

  Rect<double> world_pos = 
	 GLCanvas::theCanvas().getWorldFromScreen(screen_pos);

  GLdouble screen_width = abs(world_pos.width());
  GLdouble screen_height = abs(world_pos.height());

  DebugEval(screen_width); DebugEvalNL(screen_height);

  itsRasterX = -screen_width/2.0;
  itsRasterY = -screen_height/2.0;
  
  itsRasterX *= abs(itsZoomX);
  itsRasterY *= abs(itsZoomY);
}

void BitmapRep::grRender(Canvas& canvas) const {
DOTRACE("BitmapRep::grRender");
  itsRenderer->doRender(canvas,
								const_cast<unsigned char*>(&(itsBytes[0])),
								itsRasterX, itsRasterY,
								itsWidth, itsHeight,
								itsBitsPerPixel,
								itsByteAlignment,
								itsZoomX, itsZoomY);
}

void BitmapRep::grUnRender(Canvas& canvas) const {
DOTRACE("BitmapRep::grUnRender"); 
  Rect<double> world_rect;
  int border_pixels;
  grGetBoundingBox(world_rect, border_pixels);

  Rect<int> screen_pos = GLCanvas::theCanvas().getScreenFromWorld(world_rect);

  screen_pos.widenByStep(border_pixels + 1);
  screen_pos.heightenByStep(border_pixels + 1);

  itsRenderer->doUndraw( canvas,
								 screen_pos.left(),
								 screen_pos.bottom(),
								 screen_pos.width(),
								 abs(screen_pos.height()) );
}

///////////////
// accessors //
///////////////

bool BitmapRep::grGetBoundingBox(Rect<double>& bbox,
										int& border_pixels) const {
DOTRACE("BitmapRep::grGetBoundingBox");

  border_pixels = 2;

  // Object coordinates for the lower left corner
  bbox.left() = itsRasterX;
  bbox.bottom() = itsRasterY;

  // Get screen coordinates for the lower left corner
  Point<int> screen_point =
	 GLCanvas::theCanvas().getScreenFromWorld(Point<double>(itsRasterX, itsRasterY));

  if (itsZoomX < 0.0) {
	 screen_point.x() += int(itsWidth*itsZoomX);
  }
  if (itsZoomY < 0.0) {
	 screen_point.y() += int(itsHeight*itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(itsWidth*abs(itsZoomX),
										  itsHeight*abs(itsZoomY));

  bbox.setTopRight(GLCanvas::theCanvas().getWorldFromScreen(screen_point));

  return true;
}

int BitmapRep::byteCount() const {
DOTRACE("BitmapRep::byteCount");
  return bytesPerRow() * itsHeight;
}

int BitmapRep::bytesPerRow() const {
DOTRACE("BitmapRep::bytesPerRow");
  return ( (itsWidth*itsBitsPerPixel - 1)/8 + 1 );
}

int BitmapRep::width() const {
DOTRACE("BitmapRep::width");
  return itsWidth;
}

int BitmapRep::height() const {
DOTRACE("BitmapRep::height");
  return itsHeight; 
}

double BitmapRep::getRasterX() const {
DOTRACE("BitmapRep::getRasterX");
  return itsRasterX;
}

double BitmapRep::getRasterY() const {
DOTRACE("BitmapRep::getRasterY");
  return itsRasterY;
}

double BitmapRep::getZoomX() const {
DOTRACE("BitmapRep::getZoomX");
  return itsZoomX;
}

double BitmapRep::getZoomY() const {
DOTRACE("BitmapRep::getZoomY");
  return itsZoomY;
}

bool BitmapRep::getUsingZoom() const {
DOTRACE("BitmapRep::getUsingZoom");
  return itsUsingZoom; 
}

//////////////////
// manipulators //
//////////////////

void BitmapRep::setRasterX(double val) {
DOTRACE("BitmapRep::setRasterX");
  itsRasterX = val;
}

void BitmapRep::setRasterY(double val) {
DOTRACE("BitmapRep::setRasterY");
  itsRasterY = val;
}

void BitmapRep::setZoomX(double val) {
DOTRACE("BitmapRep::setZoomX");
  if (!itsUsingZoom) return;

  itsZoomX = val;
}

void BitmapRep::setZoomY(double val) {
DOTRACE("BitmapRep::setZoomY");
  itsZoomY = val;
}

void BitmapRep::setUsingZoom(bool val) {
DOTRACE("BitmapRep::setUsingZoom");
  itsUsingZoom = val; 

  if (!itsUsingZoom) {
	 itsZoomX = 1.0;
	 itsZoomY = 1.0;
  }
}

static const char vcid_bitmaprep_cc[] = "$Header$";
#endif // !BITMAPREP_CC_DEFINED
