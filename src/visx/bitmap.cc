///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Mon Mar  6 11:57:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include "bitmaprep.h"
#include "util/pipe.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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

void Bitmap::loadPbmGzFile(const char* filename) {
DOTRACE("Bitmap::loadPbmFile");

  dynamic_string cmd("gunzip -c ");
  cmd += filename;

  Util::Pipe pipe(cmd.c_str(), "r");

  loadPbmFile(pipe.stream());

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

void Bitmap::grRender(Canvas& canvas) const
  { itsImpl->grRender(canvas); }

void Bitmap::grUnRender(Canvas& canvas) const
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
