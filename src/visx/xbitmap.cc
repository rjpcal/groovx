///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:37:04 1999
// written: Wed Nov 13 12:56:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "visx/xbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId XBITMAP_SERIAL_VERSION_ID = 2;
}

//////////////
// creators //
//////////////

XBitmap* XBitmap::make()
{
DOTRACE("XBitmap::make");
  return new XBitmap;
}

XBitmap::XBitmap() :
  XRHolder(make_shared(new XBmapRenderer())),
  Bitmap(itsRenderer)
{
DOTRACE("XBitmap::XBitmap");
  setUsingZoom(false);
}

XBitmap::~XBitmap()
{
DOTRACE("XBitmap::~XBitmap");
}

IO::VersionId XBitmap::serialVersionId() const
{
DOTRACE("XBitmap::serialVersionId");
  return XBITMAP_SERIAL_VERSION_ID;
}

void XBitmap::readFrom(IO::Reader* reader)
{
DOTRACE("XBitmap::readFrom");

  reader->ensureReadVersionId("XBitmap", 2, "Try grsh0.8a4");

  reader->readBaseClass("Bitmap", IO::makeProxy<Bitmap>(this));
}

void XBitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("XBitmap::writeTo");

  writer->ensureWriteVersionId("XBitmap", XBITMAP_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writer->writeBaseClass("Bitmap", IO::makeConstProxy<Bitmap>(this));
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
