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

#include "bmaprenderer.h"
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
  BitmapRep(BmapRenderer* renderer) :
	 itsRenderer(renderer)
	 { init(); }

  BitmapRep(BmapRenderer* renderer, const char* filename) :
	 itsRenderer(renderer),
	 itsFilename(filename)
  {
	 init();
  }

  ~BitmapRep() {}

private:
  void init();

public:

  ///
  void serialize(ostream& os, IO::IOFlag flag) const;
  ///
  void deserialize(istream& is, IO::IOFlag flag);

  ///
  int charCount() const;

  ///
  void readFrom(Reader* reader);
  ///
  void writeTo(Writer* writer) const;

  /////////////
  // actions //
  /////////////

public:
  ///
  void loadPbmFile(const char* filename);
  ///
  void loadPbmFile(istream& is);
  ///
  void writePbmFile(const char* filename) const;

  /** These routines grab pixels from a rectanglur area of the screen
		buffer into the Bitmap's pixel array. The coordinates of the
		rectangle may be specified in pixel values (ScreenRect) or in
		OpenGL coordinate values (WorldRect). */
  //@{
  ///
  void grabScreenRect(int left, int top, int right, int bottom);
  ///
  void grabScreenRect(const Rect<int>& rect);
  ///
  void grabWorldRect(double left, double top,
							double right, double bottom);
  ///
  void grabWorldRect(const Rect<double>& rect);
  //@}

  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. */
  void flipContrast();

  void doFlipContrast();

  /// Vertically inverts the image.
  void flipVertical();

  void doFlipVertical();

  /** Centers the image so that its center coincides with the origin
      in the graphics environment when it is rendered. */
  void center();

  /** Implements the rendering operation. This function delegates the
      work to itsRenderer. */
  void grRender() const;

  /** Implements the undrawing operation. This function delegates the
      work to itsRenderer. */
  void grUnRender() const;

  ///////////////
  // accessors //
  ///////////////

  ///
  bool grGetBoundingBox(Rect<double>& bounding_box,
								int& border_pixels) const;

  /// Get the number of bytes of image data.
  int byteCount() const;
  /// Get the number of bytes per row in the image data.
  int bytesPerRow() const;

  /// Get the image's width in pixels.
  int width() const;
  /// Get the image's height in pixels.
  int height() const;

  /** Manipulate the coordinates of the image's raster position (this
      specifies where the lower left corner of the image will
      fall). */
  //@{
  ///
  double getRasterX() const;
  ///
  double getRasterY() const;
  ///
  void setRasterX(double val);
  ///
  void setRasterY(double val);
  //@}

  /** Manipulate the zoom factors of the image. (NOTE: not all
      subclasses may be able to support zooming.) */
  //@{
  ///
  double getZoomX() const;
  ///
  double getZoomY() const;
  ///
  void setZoomX(double val);
  ///
  void setZoomY(double val);
  //@}

  /// Manipulate whether zooming should be used.
  //@{
  ///
  bool getUsingZoom() const;
  ///
  void setUsingZoom(bool val);
  //@}

  /// Return a pointer to the raw bitmap data.
  unsigned char* theBytes() const;

private:
  BmapRenderer* itsRenderer;

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

void BitmapRep::serialize(ostream& os, IO::IOFlag flag) const {
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

void BitmapRep::deserialize(istream& is, IO::IOFlag flag) {
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
  itsRenderer->bytesChangeHook(0, 0, 0, 1, 1);

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

  itsRenderer->bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
										 itsBitsPerPixel, itsByteAlignment);
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

  itsRenderer->bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
										 itsBitsPerPixel, itsByteAlignment);
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
  itsRenderer->bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
										 itsBitsPerPixel, itsByteAlignment);
}

void BitmapRep::grabWorldRect(double left, double top,
									double right, double bottom) {
  grabWorldRect(Rect<double>(left, top, right, bottom));
}

void BitmapRep::grabWorldRect(const Rect<double>& world_rect) {
DOTRACE("BitmapRep::grabWorldRect");
  Rect<int> screen_rect = GrObj::getScreenFromWorld(world_rect);

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

  itsRenderer->bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
										 itsBitsPerPixel, itsByteAlignment);
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

  itsRenderer->bytesChangeHook(&(itsBytes[0]), itsWidth, itsHeight,
										 itsBitsPerPixel, itsByteAlignment);
}

void BitmapRep::center() {
DOTRACE("BitmapRep::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble left_x, bottom_y, right_x, top_y;

  GrObj::getWorldFromScreen(viewport[0], viewport[1], left_x, bottom_y);
  GrObj::getWorldFromScreen(viewport[0]+itsWidth, viewport[1]+itsHeight,
									 right_x, top_y, false);

  GLdouble screen_width = abs(right_x - left_x);
  GLdouble screen_height = abs(top_y - bottom_y);

  DebugEval(screen_width); DebugEvalNL(screen_height);

  itsRasterX = -screen_width/2.0;
  itsRasterY = -screen_height/2.0;
  
  itsRasterX *= abs(itsZoomX);
  itsRasterY *= abs(itsZoomY);
}

void BitmapRep::grRender() const {
DOTRACE("BitmapRep::grRender");
  itsRenderer->doRender(const_cast<unsigned char*>(&(itsBytes[0])),
								itsRasterX, itsRasterY,
								itsWidth, itsHeight,
								itsBitsPerPixel,
								itsByteAlignment,
								itsZoomX, itsZoomY);
}

void BitmapRep::grUnRender() const {
DOTRACE("BitmapRep::grUnRender"); 
  Rect<double> world_rect;
  int border_pixels;
  grGetBoundingBox(world_rect, border_pixels);

  Rect<int> screen_pos = GrObj::getScreenFromWorld(world_rect);

  screen_pos.widenByStep(border_pixels + 1);
  screen_pos.heightenByStep(border_pixels + 1);

  itsRenderer->doUndraw( screen_pos.left(),
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
	 GrObj::getScreenFromWorld(Point<double>(itsRasterX, itsRasterY));

  if (itsZoomX < 0.0) {
	 screen_point.x() += int(itsWidth*itsZoomX);
  }
  if (itsZoomY < 0.0) {
	 screen_point.y() += int(itsHeight*itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(itsWidth*abs(itsZoomX),
										  itsHeight*abs(itsZoomY));

  bbox.setTopRight(GrObj::getWorldFromScreen(screen_point));

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

unsigned char* BitmapRep::theBytes() const
  { return const_cast<unsigned char*>(&(itsBytes[0])); }



///////////////////////////////////////////////////////////////////////
//
// Bitmap member definitions
//
///////////////////////////////////////////////////////////////////////

Bitmap::Bitmap(BmapRenderer* renderer) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(renderer))
{
DOTRACE("Bitmap::Bitmap");
}

Bitmap::Bitmap(BmapRenderer* renderer, const char* filename) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(renderer, filename))
{
DOTRACE("Bitmap::Bitmap");
}

Bitmap::Bitmap(BmapRenderer* renderer, istream& is, IOFlag flag) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(renderer))
{
DOTRACE("Bitmap::Bitmap");
  deserialize(is, flag);
}

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
  delete itsImpl;
}

void Bitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("Bitmap::serialize");

  itsImpl->serialize(os, flag);

  if (flag & IO::BASES) { GrObj::serialize(os, flag | IO::TYPENAME); }
}

void Bitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("Bitmap::deserialize");

  itsImpl->deserialize(is, flag);

  if (flag & IO::BASES) { GrObj::deserialize(is, flag | IO::TYPENAME); }
}

int Bitmap::charCount() const
  { return itsImpl->charCount() + GrObj::charCount(); }

void Bitmap::readFrom(Reader* reader) {
DOTRACE("Bitmap::readFrom");

  itsImpl->readFrom(reader);
  GrObj::readFrom(reader);
}

void Bitmap::writeTo(Writer* writer) const {
DOTRACE("Bitmap::writeTo");

  itsImpl->writeTo(writer);
  GrObj::writeTo(writer);
}

/////////////
// actions //
/////////////

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");
  itsImpl->loadPbmFile(filename);
  sendStateChangeMsg();
}

void Bitmap::loadPbmFile(istream& is) {
DOTRACE("Bitmap::loadPbmFile");
  itsImpl->loadPbmFile(is);
  sendStateChangeMsg(); 
}

void Bitmap::writePbmFile(const char* filename) const
  { itsImpl->writePbmFile(filename); sendStateChangeMsg(); }

void Bitmap::grabScreenRect(int left, int top, int right, int bottom)
  { itsImpl->grabScreenRect(left, top, right, bottom); sendStateChangeMsg(); }

void Bitmap::grabScreenRect(const Rect<int>& rect)
  { itsImpl->grabScreenRect(rect); sendStateChangeMsg(); }

void Bitmap::grabWorldRect(double left, double top,
									double right, double bottom)
  { itsImpl->grabWorldRect(left, top, right, bottom); sendStateChangeMsg(); }

void Bitmap::grabWorldRect(const Rect<double>& world_rect)
  { itsImpl->grabWorldRect(world_rect); sendStateChangeMsg(); }

void Bitmap::flipContrast()
  { itsImpl->flipContrast(); sendStateChangeMsg(); }

void Bitmap::doFlipContrast()
  { itsImpl->doFlipContrast(); sendStateChangeMsg(); }

void Bitmap::flipVertical()
  { itsImpl->flipVertical(); sendStateChangeMsg(); }

void Bitmap::doFlipVertical()
  { itsImpl->doFlipVertical(); sendStateChangeMsg(); }

void Bitmap::center()
  { itsImpl->center(); sendStateChangeMsg(); }

void Bitmap::grRender() const
  { itsImpl->grRender(); }

void Bitmap::grUnRender() const
  { itsImpl->grUnRender(); }

///////////////
// accessors //
///////////////

bool Bitmap::grGetBoundingBox(Rect<double>& bbox,
										int& border_pixels) const
  { return itsImpl->grGetBoundingBox(bbox, border_pixels); }

int Bitmap::byteCount() const
  { return itsImpl->byteCount(); }

int Bitmap::bytesPerRow() const
  { return itsImpl->bytesPerRow(); }

int Bitmap::width() const
  { return itsImpl->width(); }

int Bitmap::height() const
  { return itsImpl->height(); }

double Bitmap::getRasterX() const
  { return itsImpl->getRasterX(); }

double Bitmap::getRasterY() const
  { return itsImpl->getRasterY(); }

double Bitmap::getZoomX() const
  { return itsImpl->getZoomX(); }

double Bitmap::getZoomY() const
  { return itsImpl->getZoomY(); }

bool Bitmap::getUsingZoom() const
  { return itsImpl->getUsingZoom(); }

//////////////////
// manipulators //
//////////////////

void Bitmap::setRasterX(double val)
  { itsImpl->setRasterX(val); sendStateChangeMsg(); }

void Bitmap::setRasterY(double val)
  { itsImpl->setRasterY(val); sendStateChangeMsg(); }

void Bitmap::setZoomX(double val)
  { itsImpl->setZoomX(val); sendStateChangeMsg(); }

void Bitmap::setZoomY(double val)
  { itsImpl->setZoomY(val); sendStateChangeMsg(); }

void Bitmap::setUsingZoom(bool val)
  { itsImpl->setUsingZoom(val); sendStateChangeMsg(); }

unsigned char* Bitmap::theBytes() const
  { return itsImpl->theBytes(); sendStateChangeMsg(); }

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
