///////////////////////////////////////////////////////////////////////
//
// gxpixmap.cc
//
// Copyright (c) 1999-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999 (as bitmap.cc)
// written: Thu Nov 21 12:05:51 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXPIXMAP_CC_DEFINED
#define GXPIXMAP_CC_DEFINED

#include "gxpixmap.h"

#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "gx/bbox.h"
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

#include "util/trace.h"

namespace
{
  Gfx::Vec2<double> defaultZoom(1.0, 1.0);

  const IO::VersionId BITMAP_SERIAL_VERSION_ID = 4;
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
  BitmapImpl() :
    itsFilename(),
    itsZoom(1.0, 1.0),
    itsData(),
    itsUsingZoom(false),
    itsContrastFlip(false),
    itsVerticalFlip(false),
    itsPurgeable(false),
    itsAsBitmap(false)
  {}

  fstring itsFilename;
  Gfx::Vec2<double> itsZoom;
  mutable Gfx::BmapData itsData;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;
  bool itsPurgeable;
  bool itsAsBitmap;

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

Bitmap::Bitmap() :
  GxShapeKit(),
  rep(new BitmapImpl)
{
DOTRACE("Bitmap::Bitmap");
  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
}

Bitmap* Bitmap::make()
{
DOTRACE("Bitmap::make");
  return new Bitmap;
}

Bitmap::~Bitmap()
{
DOTRACE("Bitmap::~Bitmap");
  delete rep;
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

  reader->readValue("filename", rep->itsFilename);
  reader->readValue("zoomX", rep->itsZoom.x());
  reader->readValue("zoomY", rep->itsZoom.y());
  reader->readValue("usingZoom", rep->itsUsingZoom);
  reader->readValue("contrastFlip", rep->itsContrastFlip);
  reader->readValue("verticalFlip", rep->itsVerticalFlip);

  if (svid >= 3)
    reader->readValue("purgeable", rep->itsPurgeable);

  if (svid >= 4)
    reader->readValue("asBitmap", rep->itsAsBitmap);

  if ( rep->itsFilename.is_empty() )
    {
      rep->itsData.clear();
    }
  else
    {
      queueImage(rep->itsFilename.c_str());
    }

  if (svid >= 4)
    reader->readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
  else
    reader->readBaseClass("GrObj", IO::makeProxy<GxShapeKit>(this));
}

void Bitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("Bitmap::writeTo");

  writer->ensureWriteVersionId("Bitmap", BITMAP_SERIAL_VERSION_ID, 4,
                               "Try grsh0.8a7");

  writer->writeValue("filename", rep->itsFilename);
  writer->writeValue("zoomX", rep->itsZoom.x());
  writer->writeValue("zoomY", rep->itsZoom.y());
  writer->writeValue("usingZoom", rep->itsUsingZoom);
  writer->writeValue("contrastFlip", rep->itsContrastFlip);
  writer->writeValue("verticalFlip", rep->itsVerticalFlip);
  writer->writeValue("purgeable", rep->itsPurgeable);
  writer->writeValue("asBitmap", rep->itsAsBitmap);

  writer->writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}

/////////////
// actions //
/////////////

void Bitmap::loadImage(const char* filename)
{
DOTRACE("Bitmap::loadImage");

  ImgFile::load(filename, rep->itsData);

  rep->itsFilename = filename;

  this->sigNodeChanged.emit();
}

void Bitmap::queueImage(const char* filename)
{
DOTRACE("Bitmap::queueImage");

  rep->queueImage(filename);

  this->sigNodeChanged.emit();
}

void Bitmap::saveImage(const char* filename) const
{
DOTRACE("Bitmap::saveImage");

  ImgFile::save(filename, rep->itsData);
}

void Bitmap::grabScreenRect(const Gfx::Rect<int>& rect)
{
DOTRACE("Bitmap::grabScreenRect");

  Gfx::Canvas& canvas = Gfx::Canvas::current();

  canvas.grabPixels(rect, rep->itsData);

  rep->itsFilename = "";

  rep->itsContrastFlip = false;
  rep->itsVerticalFlip = false;
  rep->itsZoom = defaultZoom;

  this->sigNodeChanged.emit();
}

void Bitmap::grabWorldRect(const Gfx::Rect<double>& world_rect)
{
DOTRACE("Bitmap::grabWorldRect");

  Gfx::Canvas& canvas = Gfx::Canvas::current();

  Gfx::Rect<int> screen_rect = canvas.screenFromWorld(world_rect);

  grabScreenRect(screen_rect);

  this->sigNodeChanged.emit();
}

void Bitmap::flipContrast()
{
DOTRACE("Bitmap::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  rep->itsContrastFlip = !(rep->itsContrastFlip);
  rep->itsData.flipContrast();

  this->sigNodeChanged.emit();
}

void Bitmap::flipVertical()
{
DOTRACE("Bitmap::flipVertical");

  rep->itsVerticalFlip = !(rep->itsVerticalFlip);
  rep->itsData.flipVertical();

  this->sigNodeChanged.emit();
}

void Bitmap::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Bitmap::grRender");

  Gfx::Vec2<double> world_pos;

  if (rep->itsData.bitsPerPixel() == 1 && rep->itsAsBitmap)
    {
      canvas.drawBitmap(rep->itsData, world_pos);
    }
  else
    {
      canvas.drawPixels(rep->itsData, world_pos, getZoom());
    }

  if (isPurgeable())
    {
      // This const_cast is OK because we aren't changing the observable
      // state; we're just re-queuing the current filename
      const_cast<BitmapImpl*>(rep)->purge();
    }
}

///////////////
// accessors //
///////////////

void Bitmap::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("Bitmap::grGetBoundingBox");

  // Get the corners in screen coordinates

  Gfx::Vec2<int> bottom_left = bbox.screenFromWorld(Gfx::Vec2<double>());
  Gfx::Vec2<int> top_right = bottom_left + (size() * getZoom());

  bbox.vertex2(Gfx::Vec2<double>());

  bbox.vertex2(bbox.worldFromScreen(top_right));
}

Gfx::Vec2<int> Bitmap::size() const
  { return rep->itsData.size(); }

Gfx::Vec2<double> Bitmap::getZoom() const
  { return rep->itsUsingZoom ? rep->itsZoom : defaultZoom; }

bool Bitmap::getUsingZoom() const
  { return rep->itsUsingZoom; }

bool Bitmap::isPurgeable() const
  { return rep->itsPurgeable; }

const char* Bitmap::filename() const
  { return rep->itsFilename.c_str(); }

bool Bitmap::getAsBitmap() const
{
DOTRACE("Bitmap::getAsBitmap");
  return rep->itsAsBitmap;
}

//////////////////
// manipulators //
//////////////////

void Bitmap::setZoom(Gfx::Vec2<double> zoom)
  { rep->itsZoom = zoom; this->sigNodeChanged.emit(); }

void Bitmap::setUsingZoom(bool val)
{
  rep->itsUsingZoom = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsUsingZoom)
    rep->itsAsBitmap = false;

  this->sigNodeChanged.emit();
}

void Bitmap::setPurgeable(bool val)
  { rep->itsPurgeable = val; this->sigNodeChanged.emit(); }

void Bitmap::setAsBitmap(bool val)
{
DOTRACE("Bitmap::setAsBitmap");
  rep->itsAsBitmap = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsAsBitmap)
    rep->itsUsingZoom = false;

  this->sigNodeChanged.emit();
}

static const char vcid_gxpixmap_cc[] = "$Header$";
#endif // !GXPIXMAP_CC_DEFINED
