///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:17 1999
// written: Thu Mar 30 09:50:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include "glbmaprenderer.h"

#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>
#include <cstring>
#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const char* ioTag = "GLBitmap";

  GLBmapRenderer* tempRenderer = 0;
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

GLBitmap::GLBitmap(istream& is, IO::IOFlag flag) :
  Bitmap(tempRenderer = new GLBmapRenderer()),
  itsRenderer(tempRenderer)
{
DOTRACE("GLBitmap::GLBitmap");
  deserialize(is, flag);
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

void GLBitmap::serialize(ostream& os, IO::IOFlag flag) const {
DOTRACE("GLBitmap::serialize");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  os << itsRenderer->getUsingGlBitmap() << sep;

  if (os.fail()) throw IO::OutputError(ioTag);

  if (flag & IO::BASES) { Bitmap::serialize(os, flag | IO::TYPENAME); }
}

void GLBitmap::deserialize(istream& is, IO::IOFlag flag) {
DOTRACE("GLBitmap::deserialize");
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  int val;
  is >> val;
  itsRenderer->setUsingGlBitmap(bool(val));

  if (is.fail()) throw IO::InputError(ioTag);

  if (flag & IO::BASES) { Bitmap::deserialize(is, flag | IO::TYPENAME); }
}

int GLBitmap::charCount() const {
DOTRACE("GLBitmap::charCount");
  return 128;
}

void GLBitmap::readFrom(IO::Reader* reader) {
DOTRACE("GLBitmap::readFrom");
  bool val;
  reader->readValue("usingGlBitmap", val); 
  itsRenderer->setUsingGlBitmap(val);

  Bitmap::readFrom(reader);
}

void GLBitmap::writeTo(IO::Writer* writer) const {
DOTRACE("GLBitmap::writeTo");
  writer->writeValue("usingGlBitmap", itsRenderer->getUsingGlBitmap());

  Bitmap::writeTo(writer);
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
