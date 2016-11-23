/** @file gfx/gxpixmap.cc GxShapeKit subclass for bitmap images */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun 15 11:30:24 1999 (as bitmap.cc)
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "rutz/bytearray.h"
#include "rutz/cstrstream.h"
#include "rutz/fstring.h"
#include "rutz/mappedfile.h"

#include <memory>

#include "rutz/trace.h"

using geom::recti;
using geom::rectd;
using geom::vec3d;
using rutz::fstring;

namespace
{
  const geom::vec2<double> defaultZoom(1.0, 1.0);

  const io::version_id BITMAP_SVID = 5;
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

  virtual media::bmap_data update();

private:
  fstring itsFilename;
  fstring& itsOwnerFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

media::bmap_data ImageUpdater::update()
{
GVX_TRACE("ImageUpdater::update");

  try
    {
      media::bmap_data result = media::load_image(itsFilename.c_str());
      if (itsFlipContrast) { result.flip_contrast(); }
      if (itsFlipVertical) { result.flip_vertical(); }
      return result;
    }
  // If there was an error while reading the file, it means we should
  // forget about itsOwnerFilename
  catch (...)
    {
      itsOwnerFilename = "";
      throw;
    }

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
    std::shared_ptr<media::bmap_data::update_func> updater
      (std::make_shared<ImageUpdater>(filename,
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
GVX_TRACE("GxPixmap::GxPixmap");
  setAlignmentMode(GxAligner::CENTER_ON_CENTER);
  setPercentBorder(0);
}

GxPixmap* GxPixmap::make()
{
GVX_TRACE("GxPixmap::make");
  return new GxPixmap;
}

GxPixmap::~GxPixmap() noexcept
{
GVX_TRACE("GxPixmap::~GxPixmap");
  delete rep;
}

io::version_id GxPixmap::class_version_id() const
{
GVX_TRACE("GxPixmap::class_version_id");
  return BITMAP_SVID;
}

void GxPixmap::read_from(io::reader& reader)
{
GVX_TRACE("GxPixmap::read_from");

  const io::version_id svid =
    reader.ensure_version_id("GxPixmap", 4,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  reader.read_value("filename", rep->itsFilename);
  reader.read_value("zoomX", rep->itsZoom.x());
  reader.read_value("zoomY", rep->itsZoom.y());
  reader.read_value("usingZoom", rep->itsUsingZoom);
  reader.read_value("contrastFlip", rep->itsContrastFlip);
  reader.read_value("verticalFlip", rep->itsVerticalFlip);
  reader.read_value("purgeable", rep->itsPurgeable);
  reader.read_value("asBitmap", rep->itsAsBitmap);

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
      rutz::byte_array imgdata = reader.read_byte_array("image");
      if (imgdata.vec.size() > 0)
        {
          rutz::imemstream s(reinterpret_cast<const char*>(&imgdata.vec[0]),
                             imgdata.vec.size());
          media::load_pnm(s, rep->itsData);
        }
#endif
    }

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void GxPixmap::write_to(io::writer& writer) const
{
GVX_TRACE("GxPixmap::write_to");

  writer.ensure_output_version_id("GxPixmap", BITMAP_SVID, 5,
                              "Try groovx0.8a7", SRC_POS);

  writer.write_value("filename", rep->itsFilename);
  writer.write_value("zoomX", rep->itsZoom.x());
  writer.write_value("zoomY", rep->itsZoom.y());
  writer.write_value("usingZoom", rep->itsUsingZoom);
  writer.write_value("contrastFlip", rep->itsContrastFlip);
  writer.write_value("verticalFlip", rep->itsVerticalFlip);
  writer.write_value("purgeable", rep->itsPurgeable);
  writer.write_value("asBitmap", rep->itsAsBitmap);

#if 0
  if ( !rep->itsFilename.is_empty() )
    {
      rutz::mapped_file m(rep->itsFilename.c_str());
      writer.write_byte_array("image",
                          static_cast<const unsigned char*>(m.memory()),
                          m.length());
    }
  else
    {
      writer.write_byte_array("image", 0, 0);
    }
#endif

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

/////////////
// actions //
/////////////

void GxPixmap::loadImage(const char* filename)
{
GVX_TRACE("GxPixmap::loadImage");

  rep->itsData = media::load_image(filename);

  rep->itsFilename = filename;

  this->sigNodeChanged.emit();
}

void GxPixmap::loadImageStream(std::istream& ist)
{
GVX_TRACE("GxPixmap::loadImageStream");

  rep->itsData = media::load_pnm(ist);
  rep->itsFilename = "";
  this->sigNodeChanged.emit();
}

void GxPixmap::reload()
{
GVX_TRACE("GxPixmap::reload");

  rep->itsData = media::load_image(rep->itsFilename.c_str());

  this->sigNodeChanged.emit();
}

void GxPixmap::queueImage(const char* filename)
{
GVX_TRACE("GxPixmap::queueImage");

  rep->queueImage(filename);

  this->sigNodeChanged.emit();
}

void GxPixmap::saveImage(const char* filename) const
{
GVX_TRACE("GxPixmap::saveImage");

  media::save_image(filename, rep->itsData);
}

void GxPixmap::grabScreenRect(nub::soft_ref<Gfx::Canvas> canvas,
                              const recti& rect)
{
GVX_TRACE("GxPixmap::grabScreenRect");

  canvas->grabPixels(rect, rep->itsData);

  rep->itsFilename = "";

  rep->itsContrastFlip = false;
  rep->itsVerticalFlip = false;
  rep->itsZoom = defaultZoom;

  this->sigNodeChanged.emit();
}

void GxPixmap::grabScreen(nub::soft_ref<Gfx::Canvas> canvas)
{
GVX_TRACE("GxPixmap::grabScreen");

  recti bounds = canvas->getScreenViewport();

  grabScreenRect(canvas, bounds);
}

void GxPixmap::grabWorldRect(nub::soft_ref<Gfx::Canvas> canvas,
                             const rectd& world_rect)
{
GVX_TRACE("GxPixmap::grabWorldRect");

  recti screen_rect = canvas->screenBoundsFromWorldRect(world_rect);

  grabScreenRect(canvas, screen_rect);

  this->sigNodeChanged.emit();
}

void GxPixmap::flipContrast()
{
GVX_TRACE("GxPixmap::flipContrast");

  // Toggle itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  rep->itsContrastFlip = !(rep->itsContrastFlip);
  rep->itsData.flip_contrast();

  this->sigNodeChanged.emit();
}

void GxPixmap::flipVertical()
{
GVX_TRACE("GxPixmap::flipVertical");

  rep->itsVerticalFlip = !(rep->itsVerticalFlip);
  rep->itsData.flip_vertical();

  this->sigNodeChanged.emit();
}

void GxPixmap::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("GxPixmap::grRender");

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
GVX_TRACE("GxPixmap::grGetBoundingBox");

  bbox.drawScreenRect(vec3d::zeros(),
                      this->size(),
                      getZoom());
}

geom::vec2<size_t> GxPixmap::size() const
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
GVX_TRACE("GxPixmap::getAsBitmap");
  return rep->itsAsBitmap;
}

long int GxPixmap::checkSum() const
  { return rep->itsData.bytes_sum(); }

//////////////////
// manipulators //
//////////////////

void GxPixmap::setZoom(geom::vec2<double> zoom)
{
GVX_TRACE("GxPixmap::setZoom");
  rep->itsZoom = zoom; this->sigNodeChanged.emit();
}

void GxPixmap::zoomTo(geom::vec2<size_t> sz)
{
GVX_TRACE("GxPixmap::zoomTo");
  const double x_ratio = double(sz.x()) / rep->itsData.width();
  const double y_ratio = double(sz.y()) / rep->itsData.height();
  const double ratio = rutz::min(x_ratio, y_ratio);
  setUsingZoom(true);
  setZoom(geom::vec2<double>(ratio, ratio));
}

void GxPixmap::setUsingZoom(bool val)
{
GVX_TRACE("GxPixmap::setUsingZoom");
  rep->itsUsingZoom = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsUsingZoom)
    rep->itsAsBitmap = false;

  this->sigNodeChanged.emit();
}

void GxPixmap::setPurgeable(bool val)
{
GVX_TRACE("GxPixmap::setPurgeable");
  rep->itsPurgeable = val; this->sigNodeChanged.emit();
}

void GxPixmap::setAsBitmap(bool val)
{
GVX_TRACE("GxPixmap::setAsBitmap");
  rep->itsAsBitmap = val;

  // glPixelZoom() does not work with glBitmap()
  if (rep->itsAsBitmap)
    rep->itsUsingZoom = false;

  this->sigNodeChanged.emit();
}

void GxPixmap::scramble(unsigned int numsubcols, unsigned int numsubrows,
                        unsigned long seed,
                        bool allowMoveSubparts,
                        bool allowFlipLeftRight,
                        bool allowFlipTopBottom)
{
GVX_TRACE("GxPixmap::scramble");

  media::bmap_data newdata =
    rep->itsData.make_scrambled(numsubcols, numsubrows, seed,
                                allowMoveSubparts,
                                allowFlipLeftRight,
                                allowFlipTopBottom);

  rep->itsData.swap(newdata);

  this->sigNodeChanged.emit();
}
