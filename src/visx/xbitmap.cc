///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:37:04 1999
// written: Fri Nov 10 17:03:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include "xbmaprenderer.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  XBmapRenderer* tempRenderer = 0;

  const IO::VersionId XBITMAP_SERIAL_VERSION_ID = 2;
}

//////////////
// creators //
//////////////

XBitmap* XBitmap::make() {
DOTRACE("XBitmap::make");
  return new XBitmap;
}

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

IO::VersionId XBitmap::serialVersionId() const {
DOTRACE("XBitmap::serialVersionId");
  return XBITMAP_SERIAL_VERSION_ID;
}

void XBitmap::readFrom(IO::Reader* reader) {
DOTRACE("XBitmap::readFrom");

  IO::VersionId svid = reader->readSerialVersionId();
  if (svid < 2)
	 Bitmap::readFrom(reader);
  else if (svid == 2)
	 {
		IO::IoProxy<Bitmap> baseclass(this);
		reader->readBaseClass("Bitmap", &baseclass);
	 }
}

void XBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("XBitmap::writeTo");

  if (XBITMAP_SERIAL_VERSION_ID < 2)
	 Bitmap::writeTo(writer);
  else if (XBITMAP_SERIAL_VERSION_ID == 2)
	 {
		IO::ConstIoProxy<Bitmap> baseclass(this);
		writer->writeBaseClass("Bitmap", &baseclass);
	 }
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
