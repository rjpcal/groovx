///////////////////////////////////////////////////////////////////////
//
// bitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Mon Jan 21 12:09:40 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_CC_DEFINED
#define BITMAP_CC_DEFINED

#include "visx/bitmap.h"

#include "visx/bmaprenderer.h"
// #include "visx/bitmaprep.h"

#include "gfx/canvas.h"

#include "gx/bmapdata.h"
#include "gx/pbm.h"
#include "gx/rect.h"
#include "gx/vec2.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/pointers.h"
#include "util/strings.h"
#include "util/pointers.h"

#include "visx/application.h"
#include "visx/bmaprenderer.h"

#include <cctype>

#include "util/trace.h"
#include "util/debug.h"

#if 1

namespace
{
  Gfx::Vec2<double> defaultZoom(1.0, 1.0);
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BitmapRep maintains data associated with a bitmap. \c BitmapRep
 * loosely implements the \c GrObj and \c IO interfaces, but does not
 * actually inherit from these classes. In fact, \c BitmapRep is used
 * as an implementation class for \c Bitmap, which delegates most of
 * its functionality to \c BitmapRep. \c BitmapRep objects must be
 * initialized with a \c BmapRenderer object, which handles the actual
 * rendering of the bitmao data onto the screen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BitmapRep
{
public:
  /// Construct with empty bitmap data.
  BitmapRep(shared_ptr<BmapRenderer> renderer);

  /// Non-virtual destructor implies this class is not polymorphic.
  ~BitmapRep();

public:

  /// Conforms to the \c IO interface.
  void readFrom(IO::Reader* reader);
  /// Conforms to the \c IO interface.
  void writeTo(IO::Writer* writer) const;

  /////////////
  // actions //
  /////////////

public:
  /// Loads PBM bitmap data from the PBM file \a filename.
  void loadPbmFile(const char* filename);

  /** Queues the PBM file \a filename for loading. The PBM bitmap data
      will not actually be retrieved from the file until it is
      needed. */
  void queuePbmFile(const char* filename);

  /// Writes PBM bitmap data to the file \a filename.
  void savePbmFile(const char* filename) const;

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in pixel values. */
  void grabScreenRect(const Gfx::Rect<int>& rect);

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in OpenGL coordinates. */
  void grabWorldRect(const Gfx::Rect<double>& rect);

  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. The polarity of the
      contrast relative to its original value is computed and stored,
      so that it can be maintained throughout IO operations. */
  void flipContrast();

  /** Vertically inverts the image. The polarity of the orientation
      relative to its original value is computed and stored, so that
      it can be maintained throughout IO operations. */
  void flipVertical();

  /** Implements the rendering operation. This function delegates the
      work to itsRenderer. */
  void render(Gfx::Canvas& canvas) const;

  ///////////////
  // accessors //
  ///////////////

  /// Conforms to the \c GrObj interface.
  Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

  /// Get the image's size (x-width, y-height) in pixels.
  Gfx::Vec2<int> size() const;

  /// Get the (x,y) factors by which the bitmap will be scaled.
  Gfx::Vec2<double> getZoom() const;

  /// Query whether zooming is currently to be used.
  bool getUsingZoom() const;

  /// Query whether the image data are purgeable.
  /** If the image data are purgeable, they will be unloaded after every
      render, and then re-queued. */
  bool isPurgeable() const;

  //////////////////
  // manipulators //
  //////////////////

  /** Change the (x,y) factors by which the bitmap will be scaled. */
  void setZoom(Gfx::Vec2<double> zoom);

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

  /// Change whether the image data are purgeable.
  void setPurgeable(bool val);

  class Impl
  {
  private:
    Impl(const Impl&);
    Impl& operator=(const Impl&);

  public:
    Impl(shared_ptr<BmapRenderer> renderer) :
      itsRenderer(renderer),
      itsFilename(),
      itsZoom(1.0, 1.0),
      itsUsingZoom(false),
      itsContrastFlip(false),
      itsVerticalFlip(false),
      itsPurgeable(false),
      itsData()
    {}

    const Gfx::Vec2<double>& getZoom() const
    {
      return itsUsingZoom ? itsZoom : defaultZoom;
    }

    shared_ptr<BmapRenderer> itsRenderer;

    fstring itsFilename;
    Gfx::Vec2<double> itsZoom;
    bool itsUsingZoom;
    bool itsContrastFlip;
    bool itsVerticalFlip;
    bool itsPurgeable;

    mutable Gfx::BmapData itsData;
  };

  class Impl;
  Impl pimpl;

private:
  BitmapRep(const BitmapRep&);
  BitmapRep& operator=(const BitmapRep&);
};

#endif

namespace
{
  const IO::VersionId BITMAP_SERIAL_VERSION_ID = 3;
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
  this->sigNodeChanged.emit();
}

void Bitmap::queuePbmFile(const char* filename)
  { itsImpl->queuePbmFile(filename); this->sigNodeChanged.emit(); }

void Bitmap::savePbmFile(const char* filename) const
  { itsImpl->savePbmFile(filename); this->sigNodeChanged.emit(); }

void Bitmap::grabScreenRect(const Gfx::Rect<int>& rect)
  { itsImpl->grabScreenRect(rect); this->sigNodeChanged.emit(); }

void Bitmap::grabWorldRect(const Gfx::Rect<double>& world_rect)
  { itsImpl->grabWorldRect(world_rect); this->sigNodeChanged.emit(); }

void Bitmap::flipContrast()
  { itsImpl->flipContrast(); this->sigNodeChanged.emit(); }

void Bitmap::flipVertical()
  { itsImpl->flipVertical(); this->sigNodeChanged.emit(); }

void Bitmap::grRender(Gfx::Canvas& canvas) const
{
  itsImpl->render(canvas);
}

///////////////
// accessors //
///////////////

Gfx::Rect<double> Bitmap::grGetBoundingBox(Gfx::Canvas& canvas) const
  { return itsImpl->grGetBoundingBox(canvas); }

Gfx::Vec2<int> Bitmap::size() const
  { return itsImpl->size(); }

Gfx::Vec2<double> Bitmap::getZoom() const
  { return itsImpl->getZoom(); }

bool Bitmap::getUsingZoom() const
  { return itsImpl->getUsingZoom(); }

bool Bitmap::isPurgeable() const
  { return itsImpl->isPurgeable(); }

//////////////////
// manipulators //
//////////////////

void Bitmap::setZoom(Gfx::Vec2<double> zoom)
  { itsImpl->setZoom(zoom); this->sigNodeChanged.emit(); }

void Bitmap::setUsingZoom(bool val)
  { itsImpl->setUsingZoom(val); this->sigNodeChanged.emit(); }

void Bitmap::setPurgeable(bool val)
  { itsImpl->setPurgeable(val); this->sigNodeChanged.emit(); }

#if 1

///////////////////////////////////////////////////////////////////////
//
// BitmapRep::Impl class definition
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
// PbmUpdater class definition
//
///////////////////////////////////////////////////////////////////////

class PbmUpdater : public Gfx::BmapData::UpdateFunc
{
public:
  PbmUpdater(const fstring& filename, fstring& owner_filename,
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

void PbmUpdater::update(Gfx::BmapData& update_me)
{
DOTRACE("PbmUpdater::update");

  try
    {
      Pbm::load(itsFilename.c_str(), update_me);
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
// BitmapRep member definitions
//
///////////////////////////////////////////////////////////////////////

BitmapRep::BitmapRep(shared_ptr<BmapRenderer> renderer) :
  pimpl(renderer)
{
DOTRACE("BitmapRep::BitmapRep");
}

BitmapRep::~BitmapRep()
{
DOTRACE("BitmapRep::~BitmapRep");
}

void BitmapRep::readFrom(IO::Reader* reader)
{
DOTRACE("BitmapRep::readFrom");

  int svid = reader->ensureReadVersionId("BitmapRep", 2, "Try grsh0.8a7");

  reader->readValue("filename", pimpl.itsFilename);
  reader->readValue("zoomX", pimpl.itsZoom.x());
  reader->readValue("zoomY", pimpl.itsZoom.y());
  reader->readValue("usingZoom", pimpl.itsUsingZoom);
  reader->readValue("contrastFlip", pimpl.itsContrastFlip);
  reader->readValue("verticalFlip", pimpl.itsVerticalFlip);

  if (svid > 2)
    reader->readValue("purgeable", pimpl.itsPurgeable);

  if ( pimpl.itsFilename.empty() )
    {
      pimpl.itsData.clear();
    }
  else
    {
      queuePbmFile(pimpl.itsFilename.c_str());
    }
}

void BitmapRep::writeTo(IO::Writer* writer) const
{
DOTRACE("BitmapRep::writeTo");

  writer->writeValue("filename", pimpl.itsFilename);
  writer->writeValue("zoomX", pimpl.itsZoom.x());
  writer->writeValue("zoomY", pimpl.itsZoom.y());
  writer->writeValue("usingZoom", pimpl.itsUsingZoom);
  writer->writeValue("contrastFlip", pimpl.itsContrastFlip);
  writer->writeValue("verticalFlip", pimpl.itsVerticalFlip);
  writer->writeValue("purgeable", pimpl.itsPurgeable);
}

/////////////
// actions //
/////////////

void BitmapRep::loadPbmFile(const char* filename)
{
DOTRACE("BitmapRep::loadPbmFile(const char*)");

  queuePbmFile(filename);

  pimpl.itsData.updateIfNeeded();
}

void BitmapRep::queuePbmFile(const char* filename)
{
DOTRACE("BitmapRep::queuePbmFile");

  shared_ptr<Gfx::BmapData::UpdateFunc> updater(
    new PbmUpdater(filename, pimpl.itsFilename,
                   pimpl.itsContrastFlip, pimpl.itsVerticalFlip));

  pimpl.itsData.queueUpdate(updater);
}

void BitmapRep::savePbmFile(const char* filename) const
{
DOTRACE("BitmapRep::savePbmFile");

  Pbm::save(filename, pimpl.itsData);
}

void BitmapRep::grabScreenRect(const Gfx::Rect<int>& rect)
{
DOTRACE("BitmapRep::grabScreenRect");
  Gfx::Canvas& canvas = Application::theApp().getCanvas();

  canvas.grabPixels(rect, pimpl.itsData);

  pimpl.itsFilename = "";

  pimpl.itsContrastFlip = false;
  pimpl.itsVerticalFlip = false;
  pimpl.itsZoom = defaultZoom;
}

void BitmapRep::grabWorldRect(const Gfx::Rect<double>& world_rect)
{
DOTRACE("BitmapRep::grabWorldRect");
  Gfx::Canvas& canvas = Application::theApp().getCanvas();

  Gfx::Rect<int> screen_rect = canvas.screenFromWorld(world_rect);

  grabScreenRect(screen_rect);
}

void BitmapRep::flipContrast()
{
DOTRACE("BitmapRep::flipContrast");

  // Toggle pimpl.itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  pimpl.itsContrastFlip = !pimpl.itsContrastFlip;

  pimpl.itsData.flipContrast();
}

void BitmapRep::flipVertical()
{
DOTRACE("BitmapRep::flipVertical");

  pimpl.itsVerticalFlip = !pimpl.itsVerticalFlip;

  pimpl.itsData.flipVertical();
}

void BitmapRep::render(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapRep::render");

  pimpl.itsRenderer->doRender(canvas,
                                 pimpl.itsData,
                                 Gfx::Vec2<double>(),
                                 pimpl.getZoom());

  if (pimpl.itsPurgeable)
    {
      pimpl.itsData.clear();

      // This const_cast is OK because we aren't changing the observable
      // state; we're just re-queuing the current filename
      const_cast<BitmapRep*>(this)->
        queuePbmFile(pimpl.itsFilename.c_str());
    }
}

///////////////
// accessors //
///////////////

Gfx::Rect<double> BitmapRep::grGetBoundingBox(Gfx::Canvas& canvas) const
{
DOTRACE("BitmapRep::grGetBoundingBox");

  // Get the corners in screen coordinates

  Gfx::Vec2<int> bottom_left = canvas.screenFromWorld(Gfx::Vec2<double>());
  Gfx::Vec2<int> top_right = bottom_left + (size() * pimpl.getZoom());

  Gfx::Rect<double> bbox;

  bbox.setBottomLeft(Gfx::Vec2<double>());

  bbox.setTopRight(canvas.worldFromScreen(top_right));

  return bbox;
}

Gfx::Vec2<int> BitmapRep::size() const
{
DOTRACE("BitmapRep::size");
  return pimpl.itsData.extent();
}

Gfx::Vec2<double> BitmapRep::getZoom() const
{
DOTRACE("BitmapRep::getZoom");
  return pimpl.getZoom();
}

bool BitmapRep::getUsingZoom() const
{
DOTRACE("BitmapRep::getUsingZoom");
  return pimpl.itsUsingZoom;
}

bool BitmapRep::isPurgeable() const
{
DOTRACE("BitmapRep::isPurgeable");
  return pimpl.itsPurgeable;
}

//////////////////
// manipulators //
//////////////////

void BitmapRep::setZoom(Gfx::Vec2<double> zoom)
{
DOTRACE("BitmapRep::setZoomX");
  pimpl.itsZoom = zoom;
}

void BitmapRep::setUsingZoom(bool val)
{
DOTRACE("BitmapRep::setUsingZoom");
  pimpl.itsUsingZoom = val;
}

void BitmapRep::setPurgeable(bool val)
{
DOTRACE("BitmapRep::setPurgeable");
  pimpl.itsPurgeable = val;
}

#endif

static const char vcid_bitmap_cc[] = "$Header$";
#endif // !BITMAP_CC_DEFINED
