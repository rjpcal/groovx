///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:37:04 1999
// written: Fri Jun  8 18:46:38 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
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
  XRHolder(make_shared(new XBmapRenderer())),
  Bitmap(
#ifndef ACC_COMPILER
			itsRenderer
#else
			shared_ptr<BmapRenderer>(itsRenderer)
#endif
			)  
{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(const char* filename) :
  XRHolder(make_shared(new XBmapRenderer())),
  Bitmap(
#ifndef ACC_COMPILER
			itsRenderer
#else
			shared_ptr<BmapRenderer>(itsRenderer)
#endif
			, filename)
{
DOTRACE("XBitmap::XBitmap");
  init();
}

void XBitmap::init() {
DOTRACE("XBitmap::init");

  GrObj::setRenderMode(DIRECT_RENDER);
  GrObj::setUnRenderMode(CLEAR_BOUNDING_BOX);
  Bitmap::setUsingZoom(false);
}

XBitmap::~XBitmap() {
DOTRACE("XBitmap::~XBitmap");
}

IO::VersionId XBitmap::serialVersionId() const {
DOTRACE("XBitmap::serialVersionId");
  return XBITMAP_SERIAL_VERSION_ID;
}

void XBitmap::readFrom(IO::Reader* reader) {
DOTRACE("XBitmap::readFrom");

  reader->ensureReadVersionId("XBitmap", 2, "Try grsh0.8a4");

  reader->readBaseClass("Bitmap", IO::makeProxy<Bitmap>(this));
}

void XBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("XBitmap::writeTo");

  writer->ensureWriteVersionId("XBitmap", XBITMAP_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  writer->writeBaseClass("Bitmap", IO::makeConstProxy<Bitmap>(this));
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
