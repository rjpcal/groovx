///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:17 1999
// written: Thu Nov 21 11:49:40 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "visx/glbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"

#include "util/strings.h"

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

fstring GLBitmap::ioTypename() const
{
DOTRACE("GLBitmap::ioTypename");

  return fstring("Bitmap");
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

static const char vcid_glbitmap_cc[] = "$Header$";
#endif // !GLBITMAP_CC_DEFINED
