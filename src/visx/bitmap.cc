///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Wed Nov 13 11:03:34 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "visx/bitmap.h"

#include "gfx/canvas.h"

#include "grsh/grsh.h"

#include "gx/bmapdata.h"
#include "gx/imgfile.h"
#include "gx/rect.h"
#include "gx/vec2.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/pointers.h"
#include "util/strings.h"
#include "util/pointers.h"

#include "visx/bmaprenderer.h"

#include "util/trace.h"

namespace
{
  Gfx::Vec2<double> defaultZoom(1.0, 1.0);

  const IO::VersionId BITMAP_SERIAL_VERSION_ID = 3;
}

///////////////////////////////////////////////////////////////////////
//
// ImageUpdater class definition
//
///////////////////////////////////////////////////////////////////////

class ImageUpdater : public Gfx::BmapData::UpdateFunc
{
public:
  ImageUpdater(const fstring& filename, fstring& owner_filename,
               bool contrast, bool vertical) :
    itsFilename(filename),
    itsOwnerFilename(owner_filename),
    itsFlipContrast(contrast),
    itsFlipVertical(vertical)
  {
    // If the first character of the new filename is '.', then we assume
    // it is a temp file, and therefore we don't save this filename in
    // itsOwnerFilename.
    if ( itsFilename.c_str()[0] != '.' )
      {
        itsOwnerFilename = itsFilename;
      }
    else
      {
        itsOwnerFilename = "";
      }
  }

  virtual void update(Gfx::BmapData& update_me);

private:
  fstring itsFilename;
  fstring& itsOwnerFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

void ImageUpdater::update(Gfx::BmapData& update_me)
{
DOTRACE("ImageUpdater::update");

  try
    {
      ImgFile::load(itsFilename.c_str(), update_me);
    }
  // If there was an error while reading the file, it means we should
  // forget about itsOwnerFilename
  catch (...)
    {
      itsOwnerFilename = "";
      throw;
    }

  if (itsFlipContrast) { update_me.flipContrast(); }
  if (itsFlipVertical) { update_me.flipVertical(); }
}

///////////////////////////////////////////////////////////////////////
//
// BitmapImpl class definition
//
///////////////////////////////////////////////////////////////////////

class BitmapImpl
{
public:
  BitmapImpl(shared_ptr<BmapRenderer> renderer) :
    itsRenderer(renderer),
    itsFilename(),
    itsZoom(1.0, 1.0),
    itsUsingZoom(false),
    itsContrastFlip(false),
    itsVerticalFlip(false),
    itsPurgeable(false),
    itsData()
  {}

  shared_ptr<BmapRenderer> itsRenderer;
  fstring itsFilename;
  Gfx::Vec2<double> itsZoom;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;
  bool itsPurgeable;
  mutable Gfx::BmapData itsData;

  void queueImage(const char* filename)
  {
    shared_ptr<Gfx::BmapData::UpdateFunc> updater
      (new ImageUpdater(filename,
                        itsFilename,
                        itsContrastFlip,
                        itsVerticalFlip));

    itsData.queueUpdate(updater);
  }

  void purge()
  {
    if (!itsFilename.is_empty())
      {
        // NOTE: it's important that this functionality be separate from
        // Bitmap's own version of queueImage(), since that function
        // immediately calls sigNodeChanged, which means that we notify
        // everyone else that the data have been purged, so the bitmap might
        // never have a chance to be drawn on the screen

        itsData.clear();

        queueImage(itsFilename.c_str());
      }
  }

private:
  BitmapImpl(const BitmapImpl&);
  BitmapImpl& operator=(const BitmapImpl&);
};

///////////////////////////////////////////////////////////////////////
//
// Bitmap member definitions
//
///////////////////////////////////////////////////////////////////////

Bitmap::Bitmap(shared_ptr<BmapRenderer> renderer) :
  GrObj(),
  itsImpl(new BitmapImpl(renderer))
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

  int svid = reader->ensureReadVersionId("Bitmap", 2, "Try grsh0.8a7");

  reader->readValue("filename", itsImpl->itsFilename);
  reader->readValue("zoomX", itsImpl->itsZoom.x());
  reader->readValue("zoomY", itsImpl->itsZoom.y());
  reader->readValue("usingZoom", itsImpl->itsUsingZoom);
  reader->readValue("contrastFlip", itsImpl->itsContrastFlip);
  reader->readValue("verticalFlip", itsImpl->itsVerticalFlip);

  if (svid > 2)
    reader->readValue("purgeable", itsImpl->itsPurgeable);

  if ( itsImpl->itsFilename.is_empty() )
    {
      itsImpl->itsData.clear();
    }
  else
    {
      queueImage(itsImpl->itsFilename.c_str());
    }

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Bitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("Bitmap::writeTo");

  writer->ensureWriteVersionId("Bitmap", BITMAP_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a7");

  writer->writeValue("filename", itsImpl->itsFilename);
  writer->writeValue("zoomX", itsImpl->itsZoom.x());
  writer->writeValue("zoomY", itsImpl->itsZoom.y());
  writer->writeValue("usingZoom", itsImpl->itsUsingZoom);
  writer->writeValue("contrastFlip", itsImpl->itsContrastFlip);
  writer->writeValue("verticalFlip", itsImpl->itsVerticalFlip);
  writer->writeValue("purgeable", itsImpl->itsPurgeable);

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

/////////////
// actions //
/////////////

void Bitmap::loadImage(const char* filename)
{
DOTRACE("Bitmap::loadImage");

  ImgFile::load(filename, itsImpl->itsData);

  itsImpl->itsFilename = filename;

  this->sigNodeChanged.emit();
}

void Bitmap::queueImage(const char* filename)
{
DOTRACE("Bitmap::queueImage");

  itsImpl->queueImage(filename);

  this->sigNodeChanged.emit();
}

void Bitmap::saveImage(const char* filename) const
{
DOTRACE("Bitmap::saveImage");

  ImgFile::save(filename, itsImpl->itsData);
}

void Bitmap::grabScreenRect(const Gfx::Rect<int>& rect)
{
DOTRACE("Bitmap::grabScreenRect");

  Gfx::Canvas& canvas = Grsh::canvas();

  canvas.grabPixels(rect, itsImpl->itsData);

  itsImpl->itsFilename = "";

  itsImpl->itsContrastFlip = false;
  itsImpl->itsVerticalFlip = false;
  itsImpl->itsZoom = defaultZoom;

  this->sigNodeChanged.emit();
}

void Bitmap::grabWorldRect(const Gfx::Rect<double>& world_rect)
{
DOTRACE("Bitmap::grabWorldRect");

  Gfx::Canvas& canvas = Grsh::canvas();

  Gfx::Rect<int> screen_rect = canvas.screenFromWorld(world_rect);

  grabScreenRect(screen_rect);

  this->sigNodeChanged.emit();
}

void Bitmap::flipContrast()
{
DOTRACE("Bitmap::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsImpl->itsContrastFlip = !(itsImpl->itsContrastFlip);
  itsImpl->itsData.flipContrast();

  this->sigNodeChanged.emit();
}

void Bitmap::flipVertical()
{
DOTRACE("Bitmap::flipVertical");

  itsImpl->itsVerticalFlip = !(itsImpl->itsVerticalFlip);
  itsImpl->itsData.flipVertical();

  this->sigNodeChanged.emit();
}

void Bitmap::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Bitmap::grRender");

  itsImpl->itsRenderer->doRender(canvas,
                                 itsImpl->itsData,
                                 Gfx::Vec2<double>(),
                                 getZoom());

  if (isPurgeable())
    {
      // This const_cast is OK because we aren't changing the observable
      // state; we're just re-queuing the current filename
      const_cast<BitmapImpl*>(itsImpl)->purge();
    }
}

///////////////
// accessors //
///////////////

Gfx::Rect<double> Bitmap::grGetBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("Bitmap::grGetBoundingBox");

  // Get the corners in screen coordinates

  Gfx::Vec2<int> bottom_left = canvas.screenFromWorld(Gfx::Vec2<double>());
  Gfx::Vec2<int> top_right = bottom_left + (size() * getZoom());

  Gfx::Rect<double> bbox;

  bbox.setBottomLeft(Gfx::Vec2<double>());

  bbox.setTopRight(canvas.worldFromScreen(top_right));

  return bbox;
}

Gfx::Vec2<int> Bitmap::size() const
  { return itsImpl->itsData.size(); }

Gfx::Vec2<double> Bitmap::getZoom() const
  { return itsImpl->itsUsingZoom ? itsImpl->itsZoom : defaultZoom; }

bool Bitmap::getUsingZoom() const
  { return itsImpl->itsUsingZoom; }

bool Bitmap::isPurgeable() const
  { return itsImpl->itsPurgeable; }

const char* Bitmap::filename() const
  { return itsImpl->itsFilename.c_str(); }

//////////////////
// manipulators //
//////////////////

void Bitmap::setZoom(Gfx::Vec2<double> zoom)
  { itsImpl->itsZoom = zoom; this->sigNodeChanged.emit(); }

void Bitmap::setUsingZoom(bool val)
  { itsImpl->itsUsingZoom = val; this->sigNodeChanged.emit(); }

void Bitmap::setPurgeable(bool val)
  { itsImpl->itsPurgeable = val; this->sigNodeChanged.emit(); }

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
