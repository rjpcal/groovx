///////////////////////////////////////////////////////////////////////
//
// bitmaprep.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 20:18:32 1999
// written: Fri Aug 10 07:27:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_CC_DEFINED
#define BITMAPREP_CC_DEFINED

#include "bitmaprep.h"

#include "application.h"
#include "bmapdata.h"
#include "bmaprenderer.h"
#include "pbm.h"
#include "point.h"
#include "rect.h"

#include "io/io.h"
#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include "util/algo.h"
#include "util/pointers.h"
#include "util/strings.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cctype>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  Point<double> defaultZoom(1.0, 1.0);
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
    itsRasterPos(0.0, 0.0),
    itsZoom(0.0, 0.0),
    itsUsingZoom(false),
    itsContrastFlip(false),
    itsVerticalFlip(false),
    itsData()
    {}

  const Point<double>& getZoom() const
  {
    return itsUsingZoom ? itsZoom : defaultZoom;
  }

  shared_ptr<BmapRenderer> itsRenderer;

  fstring itsFilename;
  Point<double> itsRasterPos;
  Point<double> itsZoom;
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

  virtual void update(BmapData& update_me);

private:
  fstring itsFilename;
  fstring& itsOwnerFilename;
  bool itsFlipContrast;
  bool itsFlipVertical;
};

void PbmUpdater::update(BmapData& update_me)
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

void BitmapRep::init()
{
DOTRACE("BitmapRep::init");
  itsImpl->itsRasterPos.set(0.0, 0.0);
  itsImpl->itsZoom.set(1.0, 1.0);
  itsImpl->itsUsingZoom = false;
  itsImpl->itsContrastFlip = false;
  itsImpl->itsVerticalFlip = false;

  itsImpl->itsData.clear();
}

void BitmapRep::readFrom(IO::Reader* reader)
{
DOTRACE("BitmapRep::readFrom");

  reader->readValue("filename", itsImpl->itsFilename);
  reader->readValue("rasterX", itsImpl->itsRasterPos.x());
  reader->readValue("rasterY", itsImpl->itsRasterPos.y());
  reader->readValue("zoomX", itsImpl->itsZoom.x());
  reader->readValue("zoomY", itsImpl->itsZoom.y());
  reader->readValue("usingZoom", itsImpl->itsUsingZoom);
  reader->readValue("contrastFlip", itsImpl->itsContrastFlip);
  reader->readValue("verticalFlip", itsImpl->itsVerticalFlip);

  if ( itsImpl->itsFilename.empty() )
    {
      clearBytes();
    }
  else
    {
      queuePbmFile(itsImpl->itsFilename.c_str());
    }
}

void BitmapRep::writeTo(IO::Writer* writer) const
{
DOTRACE("BitmapRep::writeTo");

  writer->writeValue("filename", itsImpl->itsFilename);
  writer->writeValue("rasterX", itsImpl->itsRasterPos.x());
  writer->writeValue("rasterY", itsImpl->itsRasterPos.y());
  writer->writeValue("zoomX", itsImpl->itsZoom.x());
  writer->writeValue("zoomY", itsImpl->itsZoom.y());
  writer->writeValue("usingZoom", itsImpl->itsUsingZoom);
  writer->writeValue("contrastFlip", itsImpl->itsContrastFlip);
  writer->writeValue("verticalFlip", itsImpl->itsVerticalFlip);
}

/////////////
// actions //
/////////////

void BitmapRep::loadPbmFile(const char* filename)
{
DOTRACE("BitmapRep::loadPbmFile(const char*)");

  queuePbmFile(filename);

  itsImpl->itsData.updateIfNeeded();
}

void BitmapRep::queuePbmFile(const char* filename)
{
DOTRACE("BitmapRep::queuePbmFile");

  shared_ptr<BmapData::UpdateFunc> updater(
    new PbmUpdater(filename, itsImpl->itsFilename,
                   itsImpl->itsContrastFlip, itsImpl->itsVerticalFlip));

  itsImpl->itsData.queueUpdate(updater);

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::savePbmFile(const char* filename) const
{
DOTRACE("BitmapRep::savePbmFile");

  Pbm::save(filename, itsImpl->itsData);
}

void BitmapRep::grabScreenRect(const Rect<int>& rect)
{
DOTRACE("BitmapRep::grabScreenRect");
  DebugEval(rect.left()); DebugEval(rect.right());
  DebugEval(rect.bottom()); DebugEval(rect.top());

  itsImpl->itsFilename = "";

  init();

  BmapData newData( rect.extent().abs(), 1, 1 );

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(rect.left(), rect.bottom(), newData.width(), newData.height(),
               GL_COLOR_INDEX, GL_BITMAP, newData.bytesPtr());

  itsImpl->itsData.swap( newData );

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::grabWorldRect(const Rect<double>& world_rect)
{
DOTRACE("BitmapRep::grabWorldRect");
  GWT::Canvas& canvas = Application::theApp().getCanvas();

  Rect<int> screen_rect = canvas.getScreenFromWorld(world_rect);

  grabScreenRect(screen_rect);
}

void BitmapRep::flipContrast()
{
DOTRACE("BitmapRep::flipContrast");

  // Toggle itsImpl->itsContrastFlip so we keep track of whether the number of
  // flips has been even or odd.
  itsImpl->itsContrastFlip = !itsImpl->itsContrastFlip;

  itsImpl->itsData.flipContrast();

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::flipVertical()
{
DOTRACE("BitmapRep::flipVertical");

  itsImpl->itsVerticalFlip = !itsImpl->itsVerticalFlip;

  itsImpl->itsData.flipVertical();

  itsImpl->itsRenderer->notifyBytesChanged();
}

void BitmapRep::render(GWT::Canvas& canvas) const
{
DOTRACE("BitmapRep::render");

  itsImpl->itsRenderer->doRender(canvas,
                                 itsImpl->itsData,
                                 itsImpl->itsRasterPos,
                                 itsImpl->getZoom());
}

///////////////
// accessors //
///////////////

Rect<double> BitmapRep::grGetBoundingBox() const
{
DOTRACE("BitmapRep::grGetBoundingBox");

  // Get screen coordinates for the lower left corner
  GWT::Canvas& canvas = Application::theApp().getCanvas();

  Point<int> screen_point = canvas.getScreenFromWorld(itsImpl->itsRasterPos);

  // Move the point to the upper right corner
  screen_point += size() * itsImpl->getZoom();

  Rect<double> bbox;

  bbox.setBottomLeft(itsImpl->itsRasterPos);

  bbox.setTopRight(canvas.getWorldFromScreen(screen_point));

  return bbox;
}

Point<int> BitmapRep::size() const
{
DOTRACE("BitmapRep::size");
  return itsImpl->itsData.extent();
}

Point<double> BitmapRep::getRasterPos() const
{
DOTRACE("BitmapRep::getRasterPos");
  return itsImpl->itsRasterPos;
}

Point<double> BitmapRep::getZoom() const
{
DOTRACE("BitmapRep::getZoom");
  return itsImpl->getZoom();
}

bool BitmapRep::getUsingZoom() const
{
DOTRACE("BitmapRep::getUsingZoom");
  return itsImpl->itsUsingZoom;
}

//////////////////
// manipulators //
//////////////////

void BitmapRep::setRasterPos(Point<double> pos)
{
DOTRACE("BitmapRep::setRasterPos");
  itsImpl->itsRasterPos = pos;
}

void BitmapRep::setZoom(Point<double> zoom)
{
DOTRACE("BitmapRep::setZoomX");
  itsImpl->itsZoom = zoom;
}

void BitmapRep::setUsingZoom(bool val)
{
DOTRACE("BitmapRep::setUsingZoom");
  itsImpl->itsUsingZoom = val;
}

void BitmapRep::clearBytes()
{
DOTRACE("BitmapRep::clearBytes");
  itsImpl->itsData.clear();
  itsImpl->itsRenderer->notifyBytesChanged();
}

static const char vcid_bitmaprep_cc[] = "$Header$";
#endif // !BITMAPREP_CC_DEFINED
