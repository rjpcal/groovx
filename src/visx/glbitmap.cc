///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:17 1999
// written: Fri Oct 20 17:24:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include "glbmaprenderer.h"

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
  GLBmapRenderer* tempRenderer = 0;

  const IO::VersionId GLBITMAP_SERIAL_VERSION_ID = 2;
}

GLBitmap* GLBitmap::make() {
DOTRACE("GLBitmap::make");
  return new GLBitmap;
}

GLBitmap::GLBitmap() :
  Bitmap(tempRenderer = new GLBmapRenderer()),
  itsRenderer(tempRenderer)
{
DOTRACE("GLBitmap::GLBitmap");
  init();
}

GLBitmap::GLBitmap(const char* filename) :
  Bitmap(tempRenderer = new GLBmapRenderer(), filename),
  itsRenderer(tempRenderer)
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
  delete itsRenderer; 
}

IO::VersionId GLBitmap::serialVersionId() const {
DOTRACE("GLBitmap::serialVersionId");
  return GLBITMAP_SERIAL_VERSION_ID;
}

void GLBitmap::readFrom(IO::Reader* reader) {
DOTRACE("GLBitmap::readFrom");

  bool val;
  reader->readValue("usingGlBitmap", val); 
  itsRenderer->setUsingGlBitmap(val);

  IO::VersionId svid = reader->readSerialVersionId();
  if (svid < 2)
	 Bitmap::readFrom(reader);
  else if (svid == 2)
	 {
		IO::IoProxy<Bitmap> baseclass(this);
		reader->readBaseClass("Bitmap", &baseclass);
	 }
}

void GLBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("GLBitmap::writeTo");

  writer->writeValue("usingGlBitmap", itsRenderer->getUsingGlBitmap());

  if (GLBITMAP_SERIAL_VERSION_ID < 2)
	 Bitmap::writeTo(writer);
  else if (GLBITMAP_SERIAL_VERSION_ID == 2)
	 {
		IO::ConstIoProxy<Bitmap> baseclass(this);
		writer->writeBaseClass("Bitmap", &baseclass);
	 }
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
