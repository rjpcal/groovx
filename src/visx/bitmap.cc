///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Fri May 18 17:05:01 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include "bmaprenderer.h"
#include "bitmaprep.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/pointers.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const IO::VersionId BITMAP_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Bitmap member definitions
//
///////////////////////////////////////////////////////////////////////

Bitmap::Bitmap(shared_ptr<BmapRenderer> renderer) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(renderer))
{
DOTRACE("Bitmap::Bitmap");
}

Bitmap::Bitmap(shared_ptr<BmapRenderer> renderer, const char* filename) :
  GrObj(GROBJ_GL_COMPILE, GROBJ_DIRECT_RENDER),
  itsImpl(new BitmapRep(renderer, filename))
{
DOTRACE("Bitmap::Bitmap");
}

Bitmap::~Bitmap() {
DOTRACE("Bitmap::~Bitmap");
  delete itsImpl;
}

IO::VersionId Bitmap::serialVersionId() const {
DOTRACE("Bitmap::serialVersionId");
  return BITMAP_SERIAL_VERSION_ID;
}

void Bitmap::readFrom(IO::Reader* reader) {
DOTRACE("Bitmap::readFrom");

  itsImpl->readFrom(reader);

  reader->ensureReadVersionId("Bitmap", 2, "Try grsh0.8a4");

  IO::IoProxy<GrObj> baseclass(this);
  reader->readBaseClass("GrObj", &baseclass);
}

void Bitmap::writeTo(IO::Writer* writer) const {
DOTRACE("Bitmap::writeTo");

  itsImpl->writeTo(writer);

  writer->ensureWriteVersionId("Bitmap", BITMAP_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  IO::ConstIoProxy<GrObj> baseclass(this);
  writer->writeBaseClass("GrObj", &baseclass);
}

/////////////
// actions //
/////////////

void Bitmap::loadPbmFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");
  itsImpl->loadPbmFile(filename);
  sendStateChangeMsg();
}

void Bitmap::loadPbmFile(STD_IO::istream& is) {
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

void Bitmap::grRender(GWT::Canvas& canvas) const
  { itsImpl->grRender(canvas); }

void Bitmap::grUnRender(GWT::Canvas& canvas) const
  { itsImpl->grUnRender(canvas); }

///////////////
// accessors //
///////////////

void Bitmap::grGetBoundingBox(Rect<double>& bbox,
										int& border_pixels) const
  { itsImpl->grGetBoundingBox(bbox, border_pixels); }

bool Bitmap::grHasBoundingBox() const
  { return itsImpl->grHasBoundingBox(); }

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

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
