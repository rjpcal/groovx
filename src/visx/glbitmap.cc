///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:17 1999
// written: Thu Aug 16 10:54:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>
#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  const IO::VersionId GLBITMAP_SERIAL_VERSION_ID = 2;
}

GLBitmap* GLBitmap::make()
{
DOTRACE("GLBitmap::make");
  return new GLBitmap;
}

GLBitmap::GLBitmap() :
  GLRHolder(make_shared(new GLBmapRenderer())),
  Bitmap(
#ifndef ACC_COMPILER
         itsRenderer
#else
         shared_ptr<BmapRenderer>(itsRenderer)
#endif
         )
{
DOTRACE("GLBitmap::GLBitmap");
  setRenderMode(Gmodes::GLCOMPILE);
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

  reader->ensureReadVersionId("GLBitmap", 2, "Try grsh0.8a4");

  bool val;
  reader->readValue("usingGlBitmap", val);
  itsRenderer->setUsingGlBitmap(val);

  reader->readBaseClass("Bitmap", IO::makeProxy<Bitmap>(this));
}

void GLBitmap::writeTo(IO::Writer* writer) const
{
DOTRACE("GLBitmap::writeTo");

  writer->ensureWriteVersionId("GLBitmap", GLBITMAP_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writer->writeValue("usingGlBitmap", itsRenderer->getUsingGlBitmap());

  writer->writeBaseClass("Bitmap", IO::makeConstProxy<Bitmap>(this));
}

bool GLBitmap::getUsingGlBitmap() const
{
DOTRACE("GLBitmap::getUsingGlBitmap");
  return itsRenderer->getUsingGlBitmap();
}

void GLBitmap::setUsingGlBitmap(bool val)
{
DOTRACE("GLBitmap::setUsingGlBitmap");
  itsRenderer->setUsingGlBitmap(val);

  // glPixelZoom() does not work with glBitmap()
  if (itsRenderer->getUsingGlBitmap())
    {
      setUsingZoom(false);
    }

  sendStateChangeMsg();
}

static const char vcid_glbitmap_cc[] = "$Header$";
#endif // !GLBITMAP_CC_DEFINED
