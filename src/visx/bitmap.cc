///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Fri Aug 10 12:32:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include "bmaprenderer.h"
#include "bitmaprep.h"
#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/pointers.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  const IO::VersionId BITMAP_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Bitmap member definitions
//
///////////////////////////////////////////////////////////////////////

Bitmap::Bitmap(shared_ptr<BmapRenderer> renderer) :
  GrObj(GLCOMPILE, CLEAR_BOUNDING_BOX),
  itsImpl(new BitmapRep(renderer))
{
DOTRACE("Bitmap::Bitmap");
  setAlignmentMode(GrObj::CENTER_ON_CENTER);
}

Bitmap::~Bitmap()
{
DOTRACE("Bitmap::~Bitmap");
  delete itsImpl;
}

IO::VersionId Bitmap::serialVersionId() const
{
DOTRACE("Bitmap::serialVersionId");
  return BITMAP_SERIAL_VERSION_ID;
}

void Bitmap::readFrom(IO::Reader* reader)
{
DOTRACE("Bitmap::readFrom");

  itsImpl->readFrom(reader);

  reader->ensureReadVersionId("Bitmap", 2, "Try grsh0.8a4");

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Bitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("Bitmap::writeTo");

  itsImpl->writeTo(writer);

  writer->ensureWriteVersionId("Bitmap", BITMAP_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

/////////////
// actions //
/////////////

void Bitmap::loadPbmFile(const char* filename)
{
DOTRACE("Bitmap::loadPbmFile");
  itsImpl->loadPbmFile(filename);
  sendStateChangeMsg();
}

void Bitmap::savePbmFile(const char* filename) const
  { itsImpl->savePbmFile(filename); sendStateChangeMsg(); }

void Bitmap::grabScreenRect(const Rect<int>& rect)
  { itsImpl->grabScreenRect(rect); sendStateChangeMsg(); }

void Bitmap::grabWorldRect(const Rect<double>& world_rect)
  { itsImpl->grabWorldRect(world_rect); sendStateChangeMsg(); }

void Bitmap::flipContrast()
  { itsImpl->flipContrast(); sendStateChangeMsg(); }

void Bitmap::flipVertical()
  { itsImpl->flipVertical(); sendStateChangeMsg(); }

void Bitmap::grRender(Gfx::Canvas& canvas, DrawMode mode) const
{
  if (mode == DRAW)
    itsImpl->render(canvas);
}

///////////////
// accessors //
///////////////

Rect<double> Bitmap::grGetBoundingBox() const
  { return itsImpl->grGetBoundingBox(); }

Point<int> Bitmap::size() const
  { return itsImpl->size(); }

Point<double> Bitmap::getRasterPos() const
  { return itsImpl->getRasterPos(); }

Point<double> Bitmap::getZoom() const
  { return itsImpl->getZoom(); }

bool Bitmap::getUsingZoom() const
  { return itsImpl->getUsingZoom(); }

//////////////////
// manipulators //
//////////////////

void Bitmap::setRasterPos(Point<double> pos)
  { itsImpl->setRasterPos(pos); sendStateChangeMsg(); }

void Bitmap::setZoom(Point<double> zoom)
  { itsImpl->setZoom(zoom); sendStateChangeMsg(); }

void Bitmap::setUsingZoom(bool val)
  { itsImpl->setUsingZoom(val); sendStateChangeMsg(); }

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
