///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Thu Aug 16 11:03:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "bitmap.h"

#include "bmaprenderer.h"
#include "bitmaprep.h"

#include "gfx/rect.h"

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
  GrObj(),
  itsImpl(new BitmapRep(renderer))
{
DOTRACE("Bitmap::Bitmap");
  setAlignmentMode(Gmodes::CENTER_ON_CENTER);
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

void Bitmap::grabScreenRect(const Gfx::Rect<int>& rect)
  { itsImpl->grabScreenRect(rect); sendStateChangeMsg(); }

void Bitmap::grabWorldRect(const Gfx::Rect<double>& world_rect)
  { itsImpl->grabWorldRect(world_rect); sendStateChangeMsg(); }

void Bitmap::flipContrast()
  { itsImpl->flipContrast(); sendStateChangeMsg(); }

void Bitmap::flipVertical()
  { itsImpl->flipVertical(); sendStateChangeMsg(); }

void Bitmap::grRender(Gfx::Canvas& canvas) const
{
  itsImpl->render(canvas);
}

///////////////
// accessors //
///////////////

Gfx::Rect<double> Bitmap::grGetBoundingBox() const
  { return itsImpl->grGetBoundingBox(); }

Gfx::Vec2<int> Bitmap::size() const
  { return itsImpl->size(); }

Gfx::Vec2<double> Bitmap::getZoom() const
  { return itsImpl->getZoom(); }

bool Bitmap::getUsingZoom() const
  { return itsImpl->getUsingZoom(); }

//////////////////
// manipulators //
//////////////////

void Bitmap::setZoom(Gfx::Vec2<double> zoom)
  { itsImpl->setZoom(zoom); sendStateChangeMsg(); }

void Bitmap::setUsingZoom(bool val)
  { itsImpl->setUsingZoom(val); sendStateChangeMsg(); }

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
