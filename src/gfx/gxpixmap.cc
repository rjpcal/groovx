///////////////////////////////////////////////////////////////////////
//
// gxpixmap.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 15 11:30:24 1999 (as bitmap.cc)
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXPIXMAP_CC_DEFINED
#define GXPIXMAP_CC_DEFINED

#include "gxpixmap.h"

#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxaligner.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "media/bmapdata.h"
#include "media/imgfile.h"
#include "media/pnmparser.h"

#include "util/bytearray.h"
#include "util/cstrstream.h"
#include "util/fstring.h"
#include "util/mappedfile.h"
#include "util/sharedptr.h"

#include "util/trace.h"

using geom::recti;
using geom::rectd;
using geom::vec3d;
using rutz::fstring;

namespace
{
  const geom::vec2<double> defaultZoom(1.0, 1.0);

  const IO::VersionId BITMAP_SVID = 5;
}

///////////////////////////////////////////////////////////////////////
//
// ImageUpdater class definition
//
///////////////////////////////////////////////////////////////////////

class ImageUpdater : public media::bmap_data::update_func
{
public:
  ImageUpdater(const fstring& filename,
               fstring& owner_filename,
               bool contrast, bool vertical) :
    itsFilename(filename),
    itsOwnerFilename(owner_filename),
    itsFlipContrast(contrast),
    itsFlipVertical(vertical)
  {
    // If the first character of the new filename is '.', then we
    // assume it is a temp file, and therefore we don't save this
    // filename in itsOwnerFilename.
    if ( itsFilename.c_str()[0] != '.' )
      {
        itsOwnerFilename = itsFilename;
      }
    else
      {
        itsOwnerFilename = "";
      }
  }

  virtual void update(media::bmap_data& update_me);

private:
  fstring itsFilename;
  fstring& itsOwnerFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

void ImageUpdater::update(media::bmap_data& update_me)
{
DOTRACE("ImageUpdater::update");

  try
    {
      media::load_image(itsFilename.c_str(), update_me);
    }
  // If there was an error while reading the file, it means we should
  // forget about itsOwnerFilename
  catch (...)
    {
      itsOwnerFilename = "";
      throw;
    }

  if (itsFlipContrast) { update_me.flip_contrast(); }
  if (itsFlipVertical) { update_me.flip_vertical(); }
}

///////////////////////////////////////////////////////////////////////
//
// GxPixmapImpl class definition
//
///////////////////////////////////////////////////////////////////////

class GxPixmapImpl
{
public:
  GxPixmapImpl() :
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
  geom::vec2<double> itsZoom;
  mutable media::bmap_data itsData;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;
  bool itsPurgeable;
  bool itsAsBitmap;

  void queueImage(const char* filename)
  {
    rutz::shared_ptr<media::bmap_data::update_func> updater
      (new ImageUpdater(filename,
                        itsFilename,
                        itsContrastFlip,
                        itsVerticalFlip));

    itsData.queue_update(updater);
  }

  void purge()
  {
    if (!itsFilename.is_empty())
      {
        // NOTE: it's important that this functionality be separate
        // from GxPixmap's own version of queueImage(), since that
        // function immediately calls sigNodeChanged, which means that
        // we notify everyone else that the data have been purged, so
        // the bitmap might never have a chance to be drawn on the
        // screen

        itsData.clear();

        queueImage(itsFilename.c_str());
      }
  }

private:
  GxPixmapImpl(const GxPixmapImpl&);
  GxPixmapImpl& operator=(const GxPixmapImpl&);
};

///////////////////////////////////////////////////////////////////////
//
// GxPixmap member definitions
//
///////////////////////////////////////////////////////////////////////

GxPixmap::GxPixmap() :
  GxShapeKit(),
  rep(new GxPixmapImpl)
{
DOTRACE("GxPixmap::GxPixmap");
  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setPercentBorder(0);
}

GxPixmap* GxPixmap::make()
{
DOTRACE("GxPixmap::make");
  return new GxPixmap;
}

GxPixmap::~GxPixmap() throw()
{
DOTRACE("GxPixmap::~GxPixmap");
  delete rep;
}

IO::VersionId GxPixmap::serialVersionId() const
{
DOTRACE("GxPixmap::serialVersionId");
  return BITMAP_SVID;
}

void GxPixmap::readFrom(IO::Reader& reader)
{
DOTRACE("GxPixmap::readFrom");

  int svid = reader.ensureReadVersionId("GxPixmap", 4,
                                        "Try cvs tag xml_conversion_20040526",
                                        SRC_POS);

  reader.readValue("filename", rep->itsFilename);
  reader.readValue("zoomX", rep->itsZoom.x());
  reader.readValue("zoomY", rep->itsZoom.y());
  reader.readValue("usingZoom", rep->itsUsingZoom);
  reader.readValue("contrastFlip", rep->itsContrastFlip);
  reader.readValue("verticalFlip", rep->itsVerticalFlip);
  reader.readValue("purgeable", rep->itsPurgeable);
  reader.readValue("asBitmap", rep->itsAsBitmap);

  if ( rep->itsFilename.is_empty() )
    {
      rep->itsData.clear();
    }
  else
    {
      queueImage(rep->itsFilename.c_str());
    }

  if (svid >= 5)
    {
#if 0
      rutz::byte_array imgdata;
      reader.readRawData("image", imgdata);
      if (imgdata.vec.size() > 0)
        {
          rutz::imemstream s(reinterpret_cast<const char*>(&imgdata.vec[0]),
                             imgdata.vec.size());
          media::load_pnm(s, rep->itsData);
        }
#endif
    }

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void GxPixmap::writeTo(IO::Writer& writer) const
{
DOTRACE("GxPixmap::writeTo");

  writer.ensureWriteVersionId("GxPixmap", BITMAP_SVID, 5,
                              "Try groovx0.8a7", SRC_POS);

  writer.writeValue("filename", rep->itsFilename);
  writer.writeValue("zoomX", rep->itsZoom.x());
  writer.writeValue("zoomY", rep->itsZoom.y());
  writer.writeValue("usingZoom", rep->itsUsingZoom);
  writer.writeValue("contrastFlip", rep->itsContrastFlip);
  writer.writeValue("verticalFlip", rep->itsVerticalFlip);
  writer.writeValue("purgeable", rep->itsPurgeable);
  writer.writeValue("asBitmap", rep->itsAsBitmap);

#if 0
  if ( !rep->itsFilename.is_empty() )
    {
      rutz::mapped_file m(rep->itsFilename.c_str());
      writer.writeRawData("image",
                          static_cast<const unsigned char*>(m.memory()),
                          m.length());
    }
  else
    {
      writer.writeRawData("image", 0, 0);
    }
#endif

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}

/////////////
// actions //
/////////////

void GxPixmap::loadImage(const char* filename)
{
DOTRACE("GxPixmap::loadImage");

  media::load_image(filename, rep->itsData);

  rep->itsFilename = filename;

  this->sigNodeChanged.emit();
}

void GxPixmap::loadImageStream(std::istream& ist)
{
DOTRACE("GxPixmap::loadImageStream");

  media::load_pnm(ist, rep->itsData);
  rep->itsFilename = "";
  this->sigNodeChanged.emit();
}

void GxPixmap::reload()
{
DOTRACE("GxPixmap::reload");

  media::load_image(rep->itsFilename.c_str(), rep->itsData);

  this->sigNodeChanged.emit();
}

void GxPixmap::queueImage(const char* filename)
{
DOTRACE("GxPixmap::queueImage");

  rep->queueImage(filename);

  this->sigNodeChanged.emit();
}

void GxPixmap::saveImage(const char* filename) const
{
DOTRACE("GxPixmap::saveImage");

  media::save_image(filename, rep->itsData);
}

void GxPixmap::grabScreenRect(Nub::SoftRef<Gfx::Canvas> canvas,
                              const recti& rect)
{
DOTRACE("GxPixmap::grabScreenRect");

  canvas->grabPixels(rect, rep->itsData);

  rep->itsFilename = "";

  rep->itsContrastFlip = false;
  rep->itsVerticalFlip = false;
  rep->itsZoom = defaultZoom;

  this->sigNodeChanged.emit();
}

void GxPixmap::grabScreen(Nub::SoftRef<Gfx::Canvas> canvas)
{
DOTRACE("GxPixmap::grabScreen");

  recti bounds = canvas->getScreenViewport();

  grabScreenRect(canvas, bounds);
}

void GxPixmap::grabWorldRect(Nub::SoftRef<Gfx::Canvas> canvas,
                             const rectd& world_rect)
{
DOTRACE("GxPixmap::grabWorldRect");

  recti screen_rect = canvas->screenBoundsFromWorldRect(world_rect);

  grabScreenRect(canvas, screen_rect);

  this->sigNodeChanged.emit();
}

void GxPixmap::flipContrast()
{
DOTRACE("GxPixmap::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  rep->itsContrastFlip = !(rep->itsContrastFlip);
  rep->itsData.flip_contrast();

  this->sigNodeChanged.emit();
}

void GxPixmap::flipVertical()
{
DOTRACE("GxPixmap::flipVertical");

  rep->itsVerticalFlip = !(rep->itsVerticalFlip);
  rep->itsData.flip_vertical();

  this->sigNodeChanged.emit();
}

void GxPixmap::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("GxPixmap::grRender");

  const vec3d world_pos = vec3d::zeros();

  if (rep->itsData.bits_per_pixel() == 1 && rep->itsAsBitmap)
    {
      canvas.drawBitmap(rep->itsData, world_pos);
    }
  else
    {
      canvas.drawPixels(rep->itsData, world_pos, getZoom());
    }

  if (isPurgeable())
    {
      // This const_cast is OK because we aren't changing the
      // observable state; we're just re-queuing the current filename
      const_cast<GxPixmapImpl*>(rep)->purge();
    }
}

///////////////
// accessors //
///////////////

void GxPixmap::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("GxPixmap::grGetBoundingBox");

  bbox.drawScreenRect(vec3d::zeros(),
                      this->size(),
                      getZoom());
}

geom::vec2<int> GxPixmap::size() const
  { return rep->itsData.size(); }

geom::vec2<double> GxPixmap::getZoom() const
  { return rep->itsUsingZoom ? rep->itsZoom : defaultZoom; }

bool GxPixmap::getUsingZoom() const
  { return rep->itsUsingZoom; }

bool GxPixmap::isPurgeable() const
  { return rep->itsPurgeable; }

const char* GxPixmap::filename() const
  { return rep->itsFilename.c_str(); }

bool GxPixmap::getAsBitmap() const
{
DOTRACE("GxPixmap::getAsBitmap");
  return rep->itsAsBitmap;
}

long int GxPixmap::checkSum() const
  { return rep->itsData.bytes_sum(); }

//////////////////
// manipulators //
//////////////////

media::bmap_data& GxPixmap::data()
{
DOTRACE("GxPixmap::data");
  return rep->itsData;
}

void GxPixmap::setZoom(geom::vec2<double> zoom)
{
DOTRACE("GxPixmap::setZoom");
  rep->itsZoom = zoom; this->sigNodeChanged.emit();
}

void GxPixmap::zoomTo(geom::vec2<int> sz)
{
DOTRACE("GxPixmap::zoomTo");
  double x_ratio = double(sz.x()) / rep->itsData.width();
  double y_ratio = double(sz.y()) / rep->itsData.height();
  double ratio = rutz::min(x_ratio, y_ratio);
  setUsingZoom(true);
  setZoom(geom::vec2<double>(ratio, ratio));
}

void GxPixmap::setUsingZoom(bool val)
{
DOTRACE("GxPixmap::setUsingZoom");
  rep->itsUsingZoom = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsUsingZoom)
    rep->itsAsBitmap = false;

  this->sigNodeChanged.emit();
}

void GxPixmap::setPurgeable(bool val)
{
DOTRACE("GxPixmap::setPurgeable");
  rep->itsPurgeable = val; this->sigNodeChanged.emit();
}

void GxPixmap::setAsBitmap(bool val)
{
DOTRACE("GxPixmap::setAsBitmap");
  rep->itsAsBitmap = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsAsBitmap)
    rep->itsUsingZoom = false;

  this->sigNodeChanged.emit();
}

void GxPixmap::scramble(int numsubcols, int numsubrows, int seed,
                        bool allowMoveSubparts,
                        bool allowFlipLeftRight,
                        bool allowFlipTopBottom)
{
DOTRACE("GxPixmap::scramble");

  rutz::shared_ptr<media::bmap_data> newdata =
    rep->itsData.make_scrambled(numsubcols, numsubrows, seed,
                                allowMoveSubparts,
                                allowFlipLeftRight,
                                allowFlipTopBottom);

  rep->itsData.swap(*newdata);

  this->sigNodeChanged.emit();
}

static const char vcid_gxpixmap_cc[] = "$Id$ $URL$";
#endif // !GXPIXMAP_CC_DEFINED
