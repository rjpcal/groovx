///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:37:04 1999
// written: Thu Nov 21 11:49:19 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "visx/xbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"

#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId XBITMAP_SERIAL_VERSION_ID = 2;
}

XBitmap* XBitmap::make()
{
DOTRACE("XBitmap::make");
  return new XBitmap;
}

XBitmap::XBitmap() :
  Bitmap()
{
DOTRACE("XBitmap::XBitmap");
  setUsingZoom(false);
}

XBitmap::~XBitmap()
{
DOTRACE("XBitmap::~XBitmap");
}

fstring XBitmap::ioTypename() const
{
DOTRACE("XBitmap::ioTypename");

  return fstring("Bitmap");
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

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
