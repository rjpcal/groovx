///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Wed Dec  1 09:54:37 1999
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
#include <cmath>					  // for abs

#include "error.h"
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
// Bitmap::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class BitmapRep {
public:
  BitmapRep() {}

  BitmapRep(const char* filename) :
	 itsFilename(filename)
	 {}

  ~BitmapRep() {}

  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;

  vector<unsigned char> itsBytes;
  int itsHeight;
  int itsWidth;
  int itsBitsPerPixel;
  int itsByteAlignment;
};

///////////////////////////////////////////////////////////////////////
//
// Bitmap member definitions
//
///////////////////////////////////////////////////////////////////////

Bitmap::Bitmap() :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep())
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(const char* filename) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(filename))
{
DOTRACE("Bitmap::Bitmap");
  init();
}

Bitmap::Bitmap(istream& is, IOFlag flag) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep())
{
DOTRACE("Bitmap::Bitmap");
  init();
  deserialize(is, flag);
}

void Bitmap::init() {
DOTRACE("Bitmap::init");
  itsImpl->itsRasterX = itsImpl->itsRasterY = 0.0;
  itsImpl->itsZoomX = itsImpl->itsZoomY = 1.0;
  itsImpl->itsUsingZoom = false;
  itsImpl->itsBytes.resize(1);
  itsImpl->itsContrastFlip = false;
  itsImpl->itsVerticalFlip = false;

  itsImpl->itsHeight = 1;
  itsImpl->itsWidth = 1;
  itsImpl->itsBitsPerPixel = 1;
  itsImpl->itsByteAlignment = 1;
}

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
  delete itsImpl;
}

void Bitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Bitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsImpl->itsFilename << '\t';
  os << itsImpl->itsRasterX << sep << itsImpl->itsRasterY << sep;
  os << itsImpl->itsZoomX << sep << itsImpl->itsZoomY << sep;
  os << itsImpl->itsUsingZoom << sep;
  os << itsImpl->itsContrastFlip << sep;
  os << itsImpl->itsVerticalFlip << endl;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void Bitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("Bitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  IO::eatWhitespace(is);
  getline(is, itsImpl->itsFilename, '\t');

  is >> itsImpl->itsRasterX >> itsImpl->itsRasterY;
  is >> itsImpl->itsZoomX >> itsImpl->itsZoomY;

  int val;
  is >> val;
  itsImpl->itsUsingZoom = bool(val);

  is >> val;
  itsImpl->itsContrastFlip = bool(val);

  is >> val;
  itsImpl->itsVerticalFlip = bool(val);

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }

  itsImpl->itsBytes.resize(1);
  bytesChangeHook(0, 0, 0, 1, 1);

  if ( !itsImpl->itsFilename.empty() ) {
	 loadPbmFile(itsImpl->itsFilename.c_str());
  }
}

int Bitmap::charCount() const {
DOTRACE("Bitmap::charCount");
  return 128 + GrObj::charCount();
}

void Bitmap::readFrom(Reader* reader) {
DOTRACE("Bitmap::readFrom");
  reader->readValue("filename", itsImpl->itsFilename);
  reader->readValue("rasterX", itsImpl->itsRasterX);
  reader->readValue("rasterY", itsImpl->itsRasterY);
  reader->readValue("zoomX", itsImpl->itsZoomX);
  reader->readValue("zoomY", itsImpl->itsZoomY);
  reader->readValue("usingZoom", itsImpl->itsUsingZoom);
  reader->readValue("contrastFlip", itsImpl->itsContrastFlip);
  reader->readValue("verticalFlip", itsImpl->itsVerticalFlip);

  GrObj::readFrom(reader);
}

void Bitmap::writeTo(Writer* writer) const {
DOTRACE("Bitmap::writeTo");
  writer->writeValue("filename", itsImpl->itsFilename);
  writer->writeValue("rasterX", itsImpl->itsRasterX);
  writer->writeValue("rasterY", itsImpl->itsRasterY);
  writer->writeValue("zoomX", itsImpl->itsZoomX);
  writer->writeValue("zoomY", itsImpl->itsZoomY);
  writer->writeValue("usingZoom", itsImpl->itsUsingZoom);
  writer->writeValue("contrastFlip", itsImpl->itsContrastFlip);
  writer->writeValue("verticalFlip", itsImpl->itsVerticalFlip);

  GrObj::writeTo(writer);
}

/////////////
// actions //
/////////////

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile(const char*)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(filename);

  // Grab ownership of the bitmap data from pbm into this object's itsImpl->itsBytes.
  pbm.grabBytes(itsImpl->itsBytes, itsImpl->itsWidth, itsImpl->itsHeight, itsImpl->itsBitsPerPixel);
  itsImpl->itsFilename = filename;

  if (itsImpl->itsContrastFlip) { doFlipContrast(); }
  if (itsImpl->itsVerticalFlip) { doFlipVertical(); }

  bytesChangeHook(&(itsImpl->itsBytes[0]), itsImpl->itsWidth, itsImpl->itsHeight,
						itsImpl->itsBitsPerPixel, itsImpl->itsByteAlignment);
  
  sendStateChangeMsg();
}

void Bitmap::loadPbmFile(istream& is) {
DOTRACE("Bitmap::loadPbmFile(istream&)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(is);

  // Grab ownership of the bitmap data from pbm into this object's itsImpl->itsBytes.
  pbm.grabBytes(itsImpl->itsBytes, itsImpl->itsWidth, itsImpl->itsHeight, itsImpl->itsBitsPerPixel);
  itsImpl->itsFilename = "(direct_from_stream)";

  if (itsImpl->itsContrastFlip) { doFlipContrast(); }
  if (itsImpl->itsVerticalFlip) { doFlipVertical(); }

  bytesChangeHook(&(itsImpl->itsBytes[0]), itsImpl->itsWidth, itsImpl->itsHeight,
						itsImpl->itsBitsPerPixel, itsImpl->itsByteAlignment);
  
  sendStateChangeMsg(); 
}

void Bitmap::writePbmFile(const char* filename) const {
DOTRACE("Bitmap::writePbmFile");
  Pbm pbm(itsImpl->itsBytes, itsImpl->itsWidth, itsImpl->itsHeight, itsImpl->itsBitsPerPixel);
  pbm.write(filename);
}

void Bitmap::grabScreenRect(int left, int top, int right, int bottom) {
  grabScreenRect(Rect<int>(left, top, right, bottom));
}

void Bitmap::grabScreenRect(const Rect<int>& rect) {
DOTRACE("Bitmap::grabScreenRect");
  DebugEval(rect.left()); DebugEval(rect.right());
  DebugEval(rect.bottom()); DebugEval(rect.top());

  itsImpl->itsHeight = abs(rect.height());    DebugEvalNL(itsImpl->itsHeight); 
  itsImpl->itsWidth = rect.width();           DebugEval(itsImpl->itsWidth);
  itsImpl->itsBitsPerPixel = 1;
  itsImpl->itsByteAlignment = 1;
  
  itsImpl->itsFilename = "";
  itsImpl->itsRasterY = itsImpl->itsRasterX = 0.0;
  itsImpl->itsZoomY = itsImpl->itsZoomX = 1.0;
  itsImpl->itsUsingZoom = false;
  itsImpl->itsContrastFlip = false;
  itsImpl->itsVerticalFlip = false;

  int num_new_bytes = (itsImpl->itsWidth/8 + 1) * itsImpl->itsHeight + 1;
  Assert(num_new_bytes>0);
  vector<unsigned char> newBytes( num_new_bytes );

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), itsImpl->itsWidth, itsImpl->itsHeight,
					GL_COLOR_INDEX, GL_BITMAP, &(newBytes[0]));

  itsImpl->itsBytes.swap(newBytes);
  bytesChangeHook(&(itsImpl->itsBytes[0]), itsImpl->itsWidth, itsImpl->itsHeight,
						itsImpl->itsBitsPerPixel, itsImpl->itsByteAlignment);

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

void Bitmap::bytesChangeHook(unsigned char* /* theBytes */,
									  int /* width */,
									  int /* height */,
									  int /* bits_per_pixel */,
									  int /* byte_alignment */)
{
DOTRACE("Bitmap::bytesChangeHook");
}

void Bitmap::flipContrast() {
DOTRACE("Bitmap::flipContrast");

  // Toggle itsImpl->itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsImpl->itsContrastFlip = !itsImpl->itsContrastFlip;

  doFlipContrast();
}

void Bitmap::doFlipContrast() {
DOTRACE("Bitmap::doFlipContrast");
  int num_bytes = byteCount();

  // In this case we want to flip each bit
  if (itsImpl->itsBitsPerPixel == 1) {
	 for (int i = 0; i < num_bytes; ++i) {
		itsImpl->itsBytes[i] ^= 0xff;
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsImpl->itsBytes[i] = 0xff - itsImpl->itsBytes[i];
	 }
  }

  bytesChangeHook(&(itsImpl->itsBytes[0]), itsImpl->itsWidth, itsImpl->itsHeight,
						itsImpl->itsBitsPerPixel, itsImpl->itsByteAlignment);

  sendStateChangeMsg();
}

void Bitmap::flipVertical() {
DOTRACE("Bitmap::flipVertical");

  itsImpl->itsVerticalFlip = !itsImpl->itsVerticalFlip;

  doFlipVertical();
}

void Bitmap::doFlipVertical() {
DOTRACE("Bitmap::doFlipVertical");

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  vector<unsigned char> new_bytes(num_bytes);
  
  for (int row = 0; row < itsImpl->itsHeight; ++row) {
	 int new_row = (itsImpl->itsHeight-1)-row;
	 memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
			  static_cast<void*> (&(itsImpl->itsBytes [row     * bytes_per_row])),
			  bytes_per_row);
  }
  
  itsImpl->itsBytes.swap(new_bytes);

  bytesChangeHook(&(itsImpl->itsBytes[0]), itsImpl->itsWidth, itsImpl->itsHeight,
						itsImpl->itsBitsPerPixel, itsImpl->itsByteAlignment);

  sendStateChangeMsg();
}

void Bitmap::center() {
DOTRACE("Bitmap::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble left_x, bottom_y, right_x, top_y;

  getWorldFromScreen(viewport[0], viewport[1], left_x, bottom_y);
  getWorldFromScreen(viewport[0]+itsImpl->itsWidth, viewport[1]+itsImpl->itsHeight,
							right_x, top_y, false);

  GLdouble screen_width = abs(right_x - left_x);
  GLdouble screen_height = abs(top_y - bottom_y);

  DebugEval(screen_width); DebugEvalNL(screen_height);

  itsImpl->itsRasterX = -screen_width/2.0;
  itsImpl->itsRasterY = -screen_height/2.0;
  
  itsImpl->itsRasterX *= abs(itsImpl->itsZoomX);
  itsImpl->itsRasterY *= abs(itsImpl->itsZoomY);

  sendStateChangeMsg();
}

void Bitmap::grRender() const {
DOTRACE("Bitmap::grRender");
  doRender(const_cast<unsigned char*>(&(itsImpl->itsBytes[0])),
			  itsImpl->itsRasterX, itsImpl->itsRasterY,
			  itsImpl->itsWidth, itsImpl->itsHeight,
			  itsImpl->itsBitsPerPixel,
			  itsImpl->itsByteAlignment,
			  itsImpl->itsZoomX, itsImpl->itsZoomY);
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
				abs(screen_pos.height()) );
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
  bbox.left() = itsImpl->itsRasterX;
  bbox.bottom() = itsImpl->itsRasterY;

  // Get screen coordinates for the lower left corner
  Point<int> screen_point =
	 getScreenFromWorld(Point<double>(itsImpl->itsRasterX, itsImpl->itsRasterY));

  if (itsImpl->itsZoomX < 0.0) {
	 screen_point.x() += int(itsImpl->itsWidth*itsImpl->itsZoomX);
  }
  if (itsImpl->itsZoomY < 0.0) {
	 screen_point.y() += int(itsImpl->itsHeight*itsImpl->itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(itsImpl->itsWidth*abs(itsImpl->itsZoomX),
										  itsImpl->itsHeight*abs(itsImpl->itsZoomY));

  bbox.setTopRight(getWorldFromScreen(screen_point));

  return true;
}

int Bitmap::byteCount() const {
DOTRACE("Bitmap::byteCount");
  return bytesPerRow() * itsImpl->itsHeight;
}

int Bitmap::bytesPerRow() const {
DOTRACE("Bitmap::bytesPerRow");
  return ( (itsImpl->itsWidth*itsImpl->itsBitsPerPixel - 1)/8 + 1 );
}

int Bitmap::width() const {
DOTRACE("Bitmap::width");
  return itsImpl->itsWidth;
}

int Bitmap::height() const {
DOTRACE("Bitmap::height");
  return itsImpl->itsHeight; 
}

double Bitmap::getRasterX() const {
DOTRACE("Bitmap::getRasterX");
  return itsImpl->itsRasterX;
}

double Bitmap::getRasterY() const {
DOTRACE("Bitmap::getRasterY");
  return itsImpl->itsRasterY;
}

double Bitmap::getZoomX() const {
DOTRACE("Bitmap::getZoomX");
  return itsImpl->itsZoomX;
}

double Bitmap::getZoomY() const {
DOTRACE("Bitmap::getZoomY");
  return itsImpl->itsZoomY;
}

bool Bitmap::getUsingZoom() const {
DOTRACE("Bitmap::getUsingZoom");
  return itsImpl->itsUsingZoom; 
}

//////////////////
// manipulators //
//////////////////

void Bitmap::setRasterX(double val) {
DOTRACE("Bitmap::setRasterX");
  itsImpl->itsRasterX = val;
  sendStateChangeMsg();
}

void Bitmap::setRasterY(double val) {
DOTRACE("Bitmap::setRasterY");
  itsImpl->itsRasterY = val;
  sendStateChangeMsg();
}

void Bitmap::setZoomX(double val) {
DOTRACE("Bitmap::setZoomX");
  if (!itsImpl->itsUsingZoom) return;

  itsImpl->itsZoomX = val;
  sendStateChangeMsg();
}

void Bitmap::setZoomY(double val) {
DOTRACE("Bitmap::setZoomY");
  itsImpl->itsZoomY = val;
  sendStateChangeMsg();
}

void Bitmap::setUsingZoom(bool val) {
DOTRACE("Bitmap::setUsingZoom");
  itsImpl->itsUsingZoom = val; 

  if (!itsImpl->itsUsingZoom) {
	 itsImpl->itsZoomX = 1.0;
	 itsImpl->itsZoomY = 1.0;
  }
}

unsigned char* Bitmap::theBytes() const
  { return const_cast<unsigned char*>(&(itsImpl->itsBytes[0])); }

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
