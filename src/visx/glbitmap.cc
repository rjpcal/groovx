///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:17 1999
// written: Wed Nov 20 17:02:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "visx/glbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId GLBITMAP_SERIAL_VERSION_ID = 3;
}

GLBitmap* GLBitmap::make()
{
DOTRACE("GLBitmap::make");
  return new GLBitmap;
}

GLBitmap::GLBitmap() :
  Bitmap()
{
DOTRACE("GLBitmap::GLBitmap");
  setUsingZoom(true);
}

GLBitmap::~GLBitmap()
{
DOTRACE("GLBitmap::~GLBitmap");
}

IO::VersionId GLBitmap::serialVersionId() const
{
DOTRACE("GLBitmap::serialVersionId");
  return GLBITMAP_SERIAL_VERSION_ID;
}

void GLBitmap::readFrom(IO::Reader* reader)
{
DOTRACE("GLBitmap::readFrom");

  int svid = reader->ensureReadVersionId("GLBitmap", 2, "Try grsh0.8a4");

  if (svid <= 2)
    {
      bool val;
      reader->readValue("usingGlBitmap", val);
    }

  reader->readBaseClass("Bitmap", IO::makeProxy<Bitmap>(this));
}

void GLBitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("GLBitmap::writeTo");

  writer->ensureWriteVersionId("GLBitmap", GLBITMAP_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a4");

  writer->writeBaseClass("Bitmap", IO::makeConstProxy<Bitmap>(this));
}

static const char vcid_glbitmap_cc[] = "$Header$";
#endif // !GLBITMAP_CC_DEFINED
