///////////////////////////////////////////////////////////////////////
//
// bitmaprep.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 20:18:32 1999
// written: Thu Jan 20 02:11:32 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_CC_DEFINED
#define BITMAPREP_CC_DEFINED

#include "bitmaprep.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cctype>
#include <cmath>					  // for abs

#include "application.h"
#include "experiment.h"
#include "bmaprenderer.h"
#include "canvas.h"
#include "error.h"
#include "io.h"
#include "pbm.h"
#include "reader.h"
#include "rect.h"
#include "writer.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class PbmUpdater : public BmapData::UpdateFunc {
public:
  PbmUpdater(const char* filename,
				  bool contrast = false, bool vertical = false) :
	 itsFilename(filename),
	 itsFlipContrast(false),
	 itsFlipVertical(false)
	 {}

  virtual void update(BmapData& update_me)
	 {
		// Create a Pbm object by reading pbm data from 'filename'.
		Pbm pbm(itsFilename.c_str());

		// Grab ownership of the bitmap data from pbm into this's itsData
		pbm.swapInto( update_me );

		if (itsFlipContrast) { update_me.flipContrast(); }
		if (itsFlipVertical) { update_me.flipVertical(); }
	 }

private:
  string itsFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

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
  itsContrastFlip = false;
  itsVerticalFlip = false;

  itsData.clear();
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

  if ( itsFilename.empty() ) {
	 clearBytes();
  }
  else {
	 queuePbmFile(itsFilename.c_str());
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

  if ( itsFilename.empty() ) {
	 clearBytes();
  }
  else {
	 queuePbmFile(itsFilename.c_str());
  }
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

  queuePbmFile(filename);

  itsData.updateIfNeeded();
}

void BitmapRep::queuePbmFile(const char* filename) {
DOTRACE("BitmapRep::queuePbmFile");

  auto_ptr<BmapData::UpdateFunc> updater(new PbmUpdater(filename,
																		  itsContrastFlip,
																		  itsVerticalFlip));

  itsData.queueUpdate(updater);

  // If the first character of the new filename is '.', then we assume
  // it is a temp file, and therefore we don't save this filename in
  // itsFilename.
  if ( filename[0] != '.' ) {
	 itsFilename = filename;
  }
  else {
	 itsFilename = "";
  }

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::loadPbmFile(istream& is) {
DOTRACE("BitmapRep::loadPbmFile(istream&)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(is);

  // Grab ownership of the bitmap data from pbm into this object's itsBytes.
  pbm.swapInto( this->itsData );

  itsFilename = "";

  if (itsContrastFlip) { doFlipContrast(); }
  if (itsVerticalFlip) { doFlipVertical(); }

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::writePbmFile(const char* filename) const {
DOTRACE("BitmapRep::writePbmFile");
  Pbm pbm(itsData.bytesVec(), itsData.width(), itsData.height(),
			 itsData.bitsPerPixel());

  pbm.write(filename);
}

void BitmapRep::grabScreenRect(int left, int top, int right, int bottom) {
  grabScreenRect(Rect<int>(left, top, right, bottom));
}

void BitmapRep::grabScreenRect(const Rect<int>& rect) {
DOTRACE("BitmapRep::grabScreenRect");
  DebugEval(rect.left()); DebugEval(rect.right());
  DebugEval(rect.bottom()); DebugEval(rect.top());

  itsFilename = "";

  init();

  BmapData newData( rect.width(), abs(rect.height()), 1, 1 );

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), newData.width(), newData.height(),
					GL_COLOR_INDEX, GL_BITMAP, newData.bytesPtr());

  itsData.swap( newData );

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::grabWorldRect(double left, double top,
									double right, double bottom) {
  grabWorldRect(Rect<double>(left, top, right, bottom));
}

void BitmapRep::grabWorldRect(const Rect<double>& world_rect) {
DOTRACE("BitmapRep::grabWorldRect");
  Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Rect<int> screen_rect = canvas->getScreenFromWorld(world_rect);

  grabScreenRect(screen_rect);
}

void BitmapRep::flipContrast() {
DOTRACE("BitmapRep::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsContrastFlip = !itsContrastFlip;

  itsData.flipContrast();

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::flipVertical() {
DOTRACE("BitmapRep::flipVertical");

  itsVerticalFlip = !itsVerticalFlip;

  itsData.flipVertical();

  itsRenderer->notifyBytesChanged();
}

void BitmapRep::center() {
DOTRACE("BitmapRep::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Rect<int> screen_pos;
  screen_pos.left() = viewport[0];
  screen_pos.right() = viewport[0] + itsData.width();
  screen_pos.bottom() = viewport[1];
  screen_pos.top() = viewport[1] + itsData.height();

  Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Rect<double> world_pos = canvas->getWorldFromScreen(screen_pos);

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
								itsData.bytesPtr(),
								itsRasterX, itsRasterY,
								itsData.width(), itsData.height(),
								itsData.bitsPerPixel(),
								itsData.byteAlignment(),
								itsZoomX, itsZoomY);
}

void BitmapRep::grUnRender(Canvas& canvas) const {
DOTRACE("BitmapRep::grUnRender"); 
  Rect<double> world_rect;
  int border_pixels;
  grGetBoundingBox(world_rect, border_pixels);

  Rect<int> screen_pos = canvas.getScreenFromWorld(world_rect);

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
  Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Point<int> screen_point =
	 canvas->getScreenFromWorld(Point<double>(itsRasterX, itsRasterY));

  if (itsZoomX < 0.0) {
	 screen_point.x() += int(width()*itsZoomX);
  }
  if (itsZoomY < 0.0) {
	 screen_point.y() += int(height()*itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(width()*abs(itsZoomX),
										  height()*abs(itsZoomY));

  bbox.setTopRight(canvas->getWorldFromScreen(screen_point));

  return true;
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

void BitmapRep::clearBytes() {
DOTRACE("BitmapRep::clearBytes");
  itsData.clear(); 
  itsRenderer->notifyBytesChanged();
}

static const char vcid_bitmaprep_cc[] = "$Header$";
#endif // !BITMAPREP_CC_DEFINED
