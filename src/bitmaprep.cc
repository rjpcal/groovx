///////////////////////////////////////////////////////////////////////
//
// bitmaprep.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 20:18:32 1999
// written: Fri Jun  1 14:26:57 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_CC_DEFINED
#define BITMAPREP_CC_DEFINED

#include "bitmaprep.h"

#include "application.h"
#include "bmapdata.h"
#include "experiment.h"
#include "bmaprenderer.h"
#include "pbm.h"
#include "rect.h"

#include "io/io.h"
#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include "util/pointers.h"
#include "util/strings.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cctype>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  template <class T>
  inline T abs(const T& val)
	 { return (val < 0) ? -val : val; }
}

///////////////////////////////////////////////////////////////////////
//
// BitmapRep::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class BitmapRep::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(shared_ptr<BmapRenderer> renderer, const char* filename="") :
	 itsRenderer(renderer),
	 itsFilename(filename),
	 itsRasterX(0.0), itsRasterY(0.0),
	 itsZoomX(0.0), itsZoomY(0.0),
	 itsUsingZoom(false),
	 itsContrastFlip(false),
	 itsVerticalFlip(false),
	 itsData()
	 {}

  shared_ptr<BmapRenderer> itsRenderer;

  fixed_string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;

  BmapData itsData;
};

///////////////////////////////////////////////////////////////////////
//
// PbmUpdater class definition
//
///////////////////////////////////////////////////////////////////////

class PbmUpdater : public BmapData::UpdateFunc {
public:
  PbmUpdater(const char* filename,
				  bool contrast = false, bool vertical = false) :
	 itsFilename(filename),
	 itsFlipContrast(contrast),
	 itsFlipVertical(vertical)
	 {}

  virtual void update(BmapData& update_me);

private:
  fixed_string itsFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

void PbmUpdater::update(BmapData& update_me) {
DOTRACE("PbmUpdater::update");

  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(itsFilename.c_str());

  // Grab ownership of the bitmap data from pbm into this's itsData
  pbm.swapInto( update_me );

  if (itsFlipContrast) { update_me.flipContrast(); }
  if (itsFlipVertical) { update_me.flipVertical(); }
}

///////////////////////////////////////////////////////////////////////
//
// BitmapRep member definitions
//
///////////////////////////////////////////////////////////////////////

BitmapRep::BitmapRep(shared_ptr<BmapRenderer> renderer) :
  itsImpl(new Impl(renderer))
{
DOTRACE("BitmapRep::BitmapRep");
  init();
}

BitmapRep::BitmapRep(shared_ptr<BmapRenderer> renderer, const char* filename) :
  itsImpl(new Impl(renderer, filename))
{
DOTRACE("BitmapRep::BitmapRep");
  init();
  loadPbmFile(filename);
}

BitmapRep::~BitmapRep() 
{
DOTRACE("BitmapRep::~BitmapRep");
  delete itsImpl;
}

void BitmapRep::init() {
DOTRACE("BitmapRep::init");
  itsImpl->itsRasterX = itsImpl->itsRasterY = 0.0;
  itsImpl->itsZoomX = itsImpl->itsZoomY = 1.0;
  itsImpl->itsUsingZoom = false;
  itsImpl->itsContrastFlip = false;
  itsImpl->itsVerticalFlip = false;

  itsImpl->itsData.clear();
}

void BitmapRep::readFrom(IO::Reader* reader) {
DOTRACE("BitmapRep::readFrom");

  reader->readValue("filename", itsImpl->itsFilename);
  reader->readValue("rasterX", itsImpl->itsRasterX);
  reader->readValue("rasterY", itsImpl->itsRasterY);
  reader->readValue("zoomX", itsImpl->itsZoomX);
  reader->readValue("zoomY", itsImpl->itsZoomY);
  reader->readValue("usingZoom", itsImpl->itsUsingZoom);
  reader->readValue("contrastFlip", itsImpl->itsContrastFlip);
  reader->readValue("verticalFlip", itsImpl->itsVerticalFlip);

  if ( itsImpl->itsFilename.empty() ) {
	 clearBytes();
  }
  else {
	 queuePbmFile(itsImpl->itsFilename.c_str());
  }
}

void BitmapRep::writeTo(IO::Writer* writer) const {
DOTRACE("BitmapRep::writeTo");

  writer->writeValue("filename", itsImpl->itsFilename);
  writer->writeValue("rasterX", itsImpl->itsRasterX);
  writer->writeValue("rasterY", itsImpl->itsRasterY);
  writer->writeValue("zoomX", itsImpl->itsZoomX);
  writer->writeValue("zoomY", itsImpl->itsZoomY);
  writer->writeValue("usingZoom", itsImpl->itsUsingZoom);
  writer->writeValue("contrastFlip", itsImpl->itsContrastFlip);
  writer->writeValue("verticalFlip", itsImpl->itsVerticalFlip);
}

/////////////
// actions //
/////////////

void BitmapRep::loadPbmFile(const char* filename) {
DOTRACE("BitmapRep::loadPbmFile(const char*)");

  queuePbmFile(filename);

  try {
	 itsImpl->itsData.updateIfNeeded();
  }
  // If there was a PbmError, it means we couldn't read the file
  // 'filename' so we should forget about 'filename' locally
  catch (PbmError&) {
	 itsImpl->itsFilename = "";
	 throw;
  }
}

void BitmapRep::queuePbmFile(const char* filename) {
DOTRACE("BitmapRep::queuePbmFile");

  shared_ptr<BmapData::UpdateFunc> updater(
    new PbmUpdater(filename, itsImpl->itsContrastFlip,
						 itsImpl->itsVerticalFlip));

  itsImpl->itsData.queueUpdate(updater);

  // If the first character of the new filename is '.', then we assume
  // it is a temp file, and therefore we don't save this filename in
  // itsImpl->itsFilename.
  if ( filename[0] != '.' ) {
	 itsImpl->itsFilename = filename;
  }
  else {
	 itsImpl->itsFilename = "";
  }

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::loadPbmFile(STD_IO::istream& is) {
DOTRACE("BitmapRep::loadPbmFile(STD_IO::istream&)");
  // Create a Pbm object by reading pbm data from 'filename'.
  Pbm pbm(is);

  // Grab ownership of the bitmap data from pbm into this object's itsImpl->itsBytes.
  pbm.swapInto( this->itsImpl->itsData );

  itsImpl->itsFilename = "";

  if (itsImpl->itsContrastFlip) { doFlipContrast(); }
  if (itsImpl->itsVerticalFlip) { doFlipVertical(); }

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::writePbmFile(const char* filename) const {
DOTRACE("BitmapRep::writePbmFile");
  Pbm pbm(itsImpl->itsData);

  pbm.write(filename);
}

void BitmapRep::grabScreenRect(int left, int top, int right, int bottom) {
  grabScreenRect(Rect<int>(left, top, right, bottom));
}

void BitmapRep::grabScreenRect(const Rect<int>& rect) {
DOTRACE("BitmapRep::grabScreenRect");
  DebugEval(rect.left()); DebugEval(rect.right());
  DebugEval(rect.bottom()); DebugEval(rect.top());

  itsImpl->itsFilename = "";

  init();

  BmapData newData( rect.width(), abs(rect.height()), 1, 1 );

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), newData.width(), newData.height(),
					GL_COLOR_INDEX, GL_BITMAP, newData.bytesPtr());

  itsImpl->itsData.swap( newData );

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::grabWorldRect(double left, double top,
									double right, double bottom) {
  grabWorldRect(Rect<double>(left, top, right, bottom));
}

void BitmapRep::grabWorldRect(const Rect<double>& world_rect) {
DOTRACE("BitmapRep::grabWorldRect");
  GWT::Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Rect<int> screen_rect = canvas->getScreenFromWorld(world_rect);

  grabScreenRect(screen_rect);
}

void BitmapRep::flipContrast() {
DOTRACE("BitmapRep::flipContrast");

  // Toggle itsImpl->itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsImpl->itsContrastFlip = !itsImpl->itsContrastFlip;

  itsImpl->itsData.flipContrast();

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::doFlipContrast() {
DOTRACE("BitmapRep::doFlipContrast");
  itsImpl->itsData.flipVertical();
}

void BitmapRep::flipVertical() {
DOTRACE("BitmapRep::flipVertical");

  itsImpl->itsVerticalFlip = !itsImpl->itsVerticalFlip;

  itsImpl->itsData.flipVertical();

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::doFlipVertical() {
DOTRACE("BitmapRep::doFlipVertical");
  itsImpl->itsData.flipVertical();
}

void BitmapRep::center() {
DOTRACE("BitmapRep::center");
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Rect<int> screen_pos;
  screen_pos.left() = viewport[0];
  screen_pos.right() = viewport[0] + itsImpl->itsData.width();
  screen_pos.bottom() = viewport[1];
  screen_pos.top() = viewport[1] + itsImpl->itsData.height();

  GWT::Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Rect<double> world_pos = canvas->getWorldFromScreen(screen_pos);

  GLdouble screen_width = abs(world_pos.width());
  GLdouble screen_height = abs(world_pos.height());

  DebugEval(screen_width); DebugEvalNL(screen_height);

  itsImpl->itsRasterX = -screen_width/2.0;
  itsImpl->itsRasterY = -screen_height/2.0;
  
  itsImpl->itsRasterX *= abs(itsImpl->itsZoomX);
  itsImpl->itsRasterY *= abs(itsImpl->itsZoomY);
}

void BitmapRep::render(GWT::Canvas& canvas) const {
DOTRACE("BitmapRep::render");

  itsImpl->itsRenderer->doRender(canvas,
								itsImpl->itsData.bytesPtr(),
								itsImpl->itsRasterX, itsImpl->itsRasterY,
								itsImpl->itsData.width(), itsImpl->itsData.height(),
								itsImpl->itsData.bitsPerPixel(),
								itsImpl->itsData.byteAlignment(),
								itsImpl->itsZoomX, itsImpl->itsZoomY);
}

void BitmapRep::unRender(GWT::Canvas& canvas) const {
DOTRACE("BitmapRep::unRender");

  Rect<double> world_rect;
  int border_pixels;
  grGetBoundingBox(world_rect, border_pixels);

  Rect<int> screen_pos = canvas.getScreenFromWorld(world_rect);

  screen_pos.widenByStep(border_pixels + 1);
  screen_pos.heightenByStep(border_pixels + 1);

  itsImpl->itsRenderer->doUndraw( canvas,
											 screen_pos.left(),
											 screen_pos.bottom(),
											 screen_pos.width(),
											 abs(screen_pos.height()) );
}

///////////////
// accessors //
///////////////

void BitmapRep::grGetBoundingBox(Rect<double>& bbox,
											int& border_pixels) const {
DOTRACE("BitmapRep::grGetBoundingBox");

  border_pixels = 2;

  // Object coordinates for the lower left corner
  bbox.left() = itsImpl->itsRasterX;
  bbox.bottom() = itsImpl->itsRasterY;

  // Get screen coordinates for the lower left corner
  GWT::Canvas* canvas = Application::theApp().getExperiment()->getCanvas();

  Point<int> screen_point =
	 canvas->getScreenFromWorld(Point<double>(itsImpl->itsRasterX, itsImpl->itsRasterY));

  if (itsImpl->itsZoomX < 0.0) {
	 screen_point.x() += int(width()*itsImpl->itsZoomX);
  }
  if (itsImpl->itsZoomY < 0.0) {
	 screen_point.y() += int(height()*itsImpl->itsZoomY);
  }

  // Move the point to the upper right corner
  screen_point += Point<double>(width()*abs(itsImpl->itsZoomX),
										  height()*abs(itsImpl->itsZoomY));

  bbox.setTopRight(canvas->getWorldFromScreen(screen_point));
}

bool BitmapRep::grHasBoundingBox() const {
DOTRACE("BitmapRep::grHasBoundingBox");
  return true;
}

int BitmapRep::byteCount() const {
DOTRACE("BitmapRep::byteCount");
  return itsImpl->itsData.byteCount();
}

int BitmapRep::bytesPerRow() const {
DOTRACE("BitmapRep::bytesPerRow");
  return itsImpl->itsData.bytesPerRow();
}

int BitmapRep::width() const {
DOTRACE("BitmapRep::width");
  return itsImpl->itsData.width();
}

int BitmapRep::height() const {
DOTRACE("BitmapRep::height");
  return itsImpl->itsData.height();
}

double BitmapRep::getRasterX() const {
DOTRACE("BitmapRep::getRasterX");
  return itsImpl->itsRasterX;
}

double BitmapRep::getRasterY() const {
DOTRACE("BitmapRep::getRasterY");
  return itsImpl->itsRasterY;
}

double BitmapRep::getZoomX() const {
DOTRACE("BitmapRep::getZoomX");
  return itsImpl->itsZoomX;
}

double BitmapRep::getZoomY() const {
DOTRACE("BitmapRep::getZoomY");
  return itsImpl->itsZoomY;
}

bool BitmapRep::getUsingZoom() const {
DOTRACE("BitmapRep::getUsingZoom");
  return itsImpl->itsUsingZoom; 
}

//////////////////
// manipulators //
//////////////////

void BitmapRep::setRasterX(double val) {
DOTRACE("BitmapRep::setRasterX");
  itsImpl->itsRasterX = val;
}

void BitmapRep::setRasterY(double val) {
DOTRACE("BitmapRep::setRasterY");
  itsImpl->itsRasterY = val;
}

void BitmapRep::setZoomX(double val) {
DOTRACE("BitmapRep::setZoomX");
  if (!itsImpl->itsUsingZoom) return;

  itsImpl->itsZoomX = val;
}

void BitmapRep::setZoomY(double val) {
DOTRACE("BitmapRep::setZoomY");
  itsImpl->itsZoomY = val;
}

void BitmapRep::setUsingZoom(bool val) {
DOTRACE("BitmapRep::setUsingZoom");
  itsImpl->itsUsingZoom = val; 

  if (!itsImpl->itsUsingZoom) {
	 itsImpl->itsZoomX = 1.0;
	 itsImpl->itsZoomY = 1.0;
  }
}

void BitmapRep::clearBytes() {
DOTRACE("BitmapRep::clearBytes");
  itsImpl->itsData.clear(); 
  itsImpl->itsRenderer->notifyBytesChanged();
}

static const char vcid_bitmaprep_cc[] = "$Header$";
#endif // !BITMAPREP_CC_DEFINED
