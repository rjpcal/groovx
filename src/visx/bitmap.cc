///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Tue Nov 23 17:49:45 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cctype>
#include <cstring>				  // for memcpy

#include "error.h"
#include "pbm.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "Bitmap";
}

Bitmap::Bitmap() :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER)
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(const char* filename) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsFilename(filename)
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(istream& is, IOFlag flag) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER)
{
DOTRACE("Bitmap::Bitmap");
  init();
  deserialize(is, flag);
}

void Bitmap::init() {
DOTRACE("Bitmap::init");
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

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
}

void Bitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Bitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsFilename << '\t';
  os << itsRasterX << sep << itsRasterY << sep;
  os << itsZoomX << sep << itsZoomY << sep;
  os << itsUsingZoom << sep;
  os << itsContrastFlip << sep;
  os << itsVerticalFlip << endl;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void Bitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("Bitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

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

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }

  itsBytes.resize(1);
  bytesChangeHook(0, 0, 0, 1, 1);

  if ( !itsFilename.empty() ) {
	 loadPbmFile(itsFilename.c_str());
  }
}

int Bitmap::charCount() const {
DOTRACE("Bitmap::charCount");
  return 128 + GrObj::charCount();
}

void Bitmap::readFrom(Reader* reader) {
DOTRACE("Bitmap::readFrom");
  reader->readValue("filename", itsFilename);
  reader->readValue("rasterX", itsRasterX);
  reader->readValue("rasterY", itsRasterY);
  reader->readValue("zoomX", itsZoomX);
  reader->readValue("zoomY", itsZoomY);
  reader->readValue("usingZoom", itsUsingZoom);
  reader->readValue("contrastFlip", itsContrastFlip);
  reader->readValue("verticalFlip", itsVerticalFlip);

  GrObj::readFrom(reader);
}

void Bitmap::writeTo(Writer* writer) const {
DOTRACE("Bitmap::writeTo");
  writer->writeValue("filename", itsFilename);
  writer->writeValue("rasterX", itsRasterX);
  writer->writeValue("rasterY", itsRasterY);
  writer->writeValue("zoomX", itsZoomX);
  writer->writeValue("zoomY", itsZoomY);
  writer->writeValue("usingZoom", itsUsingZoom);
  writer->writeValue("contrastFlip", itsContrastFlip);
  writer->writeValue("verticalFlip", itsVerticalFlip);

  GrObj::writeTo(writer);
}

/////////////
// actions //
/////////////

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile(const char*)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(filename);

  // Grab ownership of the bitmap data from pbm into this object's itsBytes.
  pbm.grabBytes(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  itsFilename = filename;

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }

  bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
						itsBitsPerPixel, itsByteAlignment);
  
  sendStateChangeMsg();
}

void Bitmap::loadPbmFile(istream& is) {
DOTRACE("Bitmap::loadPbmFile(istream&)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(is);

  // Grab ownership of the bitmap data from pbm into this object's itsBytes.
  pbm.grabBytes(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  itsFilename = "(direct_from_stream)";

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }

  bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
						itsBitsPerPixel, itsByteAlignment);
  
  sendStateChangeMsg(); 
}

void Bitmap::writePbmFile(const char* filename) const {
DOTRACE("Bitmap::writePbmFile");
  Pbm pbm(itsBytes, itsWidth, itsHeight, itsBitsPerPixel);
  pbm.write(filename);
}

void Bitmap::grabScreenRect(int left, int top, int right, int bottom) {
  grabScreenRect(Rect<int>(left, top, right, bottom));
}

void Bitmap::grabScreenRect(const Rect<int>& rect) {
DOTRACE("Bitmap::grabScreenRect");
  itsHeight = rect.height();         DebugEvalNL(itsHeight);
  itsWidth = rect.width();           DebugEval(itsWidth);
  itsBitsPerPixel = 1;
  itsByteAlignment = 1;
  
  itsFilename = "";
  itsRasterY = itsRasterX = 0.0;
  itsZoomY = itsZoomX = 1.0;
  itsUsingZoom = false;
  itsContrastFlip = false;
  itsVerticalFlip = false;

  vector<unsigned char> newBytes( (itsWidth/8 + 1) * itsHeight + 1);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), itsWidth, itsHeight,
					GL_COLOR_INDEX, GL_BITMAP, &(newBytes[0]));

  itsBytes.swap(newBytes);
  bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
						itsBitsPerPixel, itsByteAlignment);

  sendStateChangeMsg();
}

void Bitmap::grabWorldRect(double left, double top,
									double right, double bottom) {
  grabWorldRect(Rect<double>(left, top, right, bottom));
}

void Bitmap::grabWorldRect(const Rect<double>& world_rect) {
DOTRACE("Bitmap::grabWorldRect");
  Rect<int> screen_rect = GrObj::getScreenFromWorld(world_rect);

  grabScreenRect(screen_rect);
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

  bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
						itsBitsPerPixel, itsByteAlignment);

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
  
  vector<unsigned char> new_bytes(num_bytes);
  
  for (int row = 0; row < itsHeight; ++row) {
	 int new_row = (itsHeight-1)-row;
	 memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
			  static_cast<void*> (&(itsBytes [row     * bytes_per_row])),
			  bytes_per_row);
  }
  
  itsBytes.swap(new_bytes);

  bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
						itsBitsPerPixel, itsByteAlignment);

  sendStateChangeMsg();
}

void Bitmap::center() {
DOTRACE("Bitmap::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble left_x, bottom_y, right_x, top_y;

  getWorldFromScreen(viewport[0], viewport[1], left_x, bottom_y);
  getWorldFromScreen(viewport[0]+itsWidth, viewport[1]+itsHeight,
							right_x, top_y, false);

  GLdouble screen_width = abs(right_x - left_x);
  GLdouble screen_height = abs(top_y - bottom_y);

  DebugEval(screen_width); DebugEvalNL(screen_height);

  itsRasterX = -screen_width/2.0;
  itsRasterY = -screen_height/2.0;
  
  itsRasterX *= abs(itsZoomX);
  itsRasterY *= abs(itsZoomY);

  sendStateChangeMsg();
}

void Bitmap::grRender() const {
DOTRACE("Bitmap::grRender");
  doRender(const_cast<unsigned char*>(&(itsBytes[0])),
			  itsRasterX, itsRasterY,
			  itsWidth, itsHeight,
			  itsBitsPerPixel,
			  itsByteAlignment,
			  itsZoomX, itsZoomY);
}

void Bitmap::grUnRender() const {
DOTRACE("Bitmap::grUnRender"); 
  Rect<double> world_rect;
  getBoundingBox(world_rect);

  Rect<int> screen_pos = GrObj::getScreenFromWorld(world_rect);

  screen_pos.widenByStep(1);
  screen_pos.heightenByStep(1);

  doUndraw( screen_pos.left(),
				screen_pos.bottom(),
				screen_pos.width(),
				screen_pos.height() );
}

void Bitmap::doUndraw(int /*winRasterX*/, int /*winRasterY*/,
							 int /*winWidthX*/, int /*winHeightY*/) const {
DOTRACE("Bitmap::doUndraw");
}

///////////////
// accessors //
///////////////

bool Bitmap::grGetBoundingBox(Rect<double>& bbox,
										int& border_pixels) const {
DOTRACE("Bitmap::grGetBoundingBox");

  border_pixels = 2;

  // Object coordinates for the lower left corner
  bbox.left() = itsRasterX;
  bbox.bottom() = itsRasterY;

  // Get screen coordinates for the lower left corner
  Point<int> screen_point =
	 getScreenFromWorld(Point<double>(itsRasterX, itsRasterY));

  if (itsZoomX < 0.0) {
	 screen_point.x() += int(itsWidth*itsZoomX);
  }
  if (itsZoomY < 0.0) {
	 screen_point.y() += int(itsHeight*itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(itsWidth*abs(itsZoomX),
										  itsHeight*abs(itsZoomY));

  bbox.setTopRight(getWorldFromScreen(screen_point));

  return true;
}

int Bitmap::byteCount() const {
DOTRACE("Bitmap::byteCount");
  return bytesPerRow() * itsHeight;
}

int Bitmap::bytesPerRow() const {
DOTRACE("Bitmap::bytesPerRow");
  return ( (itsWidth*itsBitsPerPixel - 1)/8 + 1 );
}

int Bitmap::width() const {
DOTRACE("Bitmap::width");
  return itsWidth;
}

int Bitmap::height() const {
DOTRACE("Bitmap::height");
  return itsHeight; 
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

bool Bitmap::getUsingZoom() const {
DOTRACE("Bitmap::getUsingZoom");
  return itsUsingZoom; 
}

//////////////////
// manipulators //
//////////////////

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
  if (!itsUsingZoom) return;

  itsZoomX = val;
  sendStateChangeMsg();
}

void Bitmap::setZoomY(double val) {
DOTRACE("Bitmap::setZoomY");
  itsZoomY = val;
  sendStateChangeMsg();
}

void Bitmap::setUsingZoom(bool val) {
DOTRACE("Bitmap::setUsingZoom");
  itsUsingZoom = val; 

  if (!itsUsingZoom) {
	 itsZoomX = 1.0;
	 itsZoomY = 1.0;
  }
}

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
