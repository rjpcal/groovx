///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Tue Oct  3 16:32:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include "io/iolegacy.h"
#include "io/ioproxy.h"
#include "xbmaprenderer.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  XBmapRenderer* tempRenderer = 0;
}

//////////////
// creators //
//////////////

XBitmap::XBitmap() :
  Bitmap(tempRenderer = new XBmapRenderer()),
  itsRenderer(tempRenderer)
{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(const char* filename) :
  Bitmap(tempRenderer = new XBmapRenderer(), filename),
  itsRenderer(tempRenderer)

{
DOTRACE("XBitmap::XBitmap");
  init();
}

void XBitmap::init() {
DOTRACE("XBitmap::init");

  GrObj::setRenderMode(GROBJ_DIRECT_RENDER);
  GrObj::setUnRenderMode(GROBJ_CLEAR_BOUNDING_BOX);
  Bitmap::setUsingZoom(false);
}

XBitmap::~XBitmap() {
DOTRACE("XBitmap::~XBitmap");
  delete itsRenderer; 
}

void XBitmap::readFrom(IO::Reader* reader) {
DOTRACE("XBitmap::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  Bitmap::readFrom(reader);
}

void XBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("XBitmap::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  Bitmap::writeTo(writer);
}

void XBitmap::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("XBitmap::legacySrlz");
  IO::ConstIoProxy<Bitmap> baseclass(this);
  lwriter->writeBaseClass("Bitmap", &baseclass);
}

void XBitmap::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("XBitmap::legacyDesrlz");
  IO::IoProxy<Bitmap> baseclass(this);
  lreader->readBaseClass("Bitmap", &baseclass);
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
