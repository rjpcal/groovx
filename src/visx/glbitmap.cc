///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:17 1999
// written: Sat May 19 08:21:58 2001
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

namespace {
  const IO::VersionId GLBITMAP_SERIAL_VERSION_ID = 2;
}

GLBitmap* GLBitmap::make() {
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
  init();
}

GLBitmap::GLBitmap(const char* filename) :
  GLRHolder(make_shared(new GLBmapRenderer())),
  Bitmap(
#ifndef ACC_COMPILER
			itsRenderer
#else
			shared_ptr<BmapRenderer>(itsRenderer)
#endif
			, filename)
{
DOTRACE("GLBitmap::GLBitmap");
  init(); 
}

void GLBitmap::init() {
DOTRACE("GLBitmap::init");
  GrObj::setRenderMode(GROBJ_GL_COMPILE);
  GrObj::setUnRenderMode(GROBJ_CLEAR_BOUNDING_BOX);
  setUsingZoom(true);
}

GLBitmap::~GLBitmap() {
DOTRACE("GLBitmap::~GLBitmap");
}

IO::VersionId GLBitmap::serialVersionId() const {
DOTRACE("GLBitmap::serialVersionId");
  return GLBITMAP_SERIAL_VERSION_ID;
}

void GLBitmap::readFrom(IO::Reader* reader) {
DOTRACE("GLBitmap::readFrom");

  reader->ensureReadVersionId("GLBitmap", 2, "Try grsh0.8a4");

  bool val;
  reader->readValue("usingGlBitmap", val); 
  itsRenderer->setUsingGlBitmap(val);

  IdItem<IO::IoObject> baseclass(IO::makeProxy<Bitmap>(this));
  reader->readBaseClass("Bitmap", baseclass.get());
}

void GLBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("GLBitmap::writeTo");

  writer->ensureWriteVersionId("GLBitmap", GLBITMAP_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  writer->writeValue("usingGlBitmap", itsRenderer->getUsingGlBitmap());

  IdItem<IO::IoObject> baseclass(IO::makeConstProxy<Bitmap>(this));
  writer->writeBaseClass("Bitmap", baseclass.get());
}

bool GLBitmap::getUsingGlBitmap() const {
DOTRACE("GLBitmap::getUsingGlBitmap");
  return itsRenderer->getUsingGlBitmap();
}

void GLBitmap::setUsingGlBitmap(bool val) {
DOTRACE("GLBitmap::setUsingGlBitmap");
  itsRenderer->setUsingGlBitmap(val);

  // glPixelZoom() does not work with glBitmap()
  if (itsRenderer->getUsingGlBitmap()) {
	 setUsingZoom(false);
  }

  sendStateChangeMsg();
}

static const char vcid_glbitmap_cc[] = "$Header$";
#endif // !GLBITMAP_CC_DEFINED
