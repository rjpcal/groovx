///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:17 1999
// written: Sun Mar  5 16:15:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include "glbmaprenderer.h"
#include "reader.h"
#include "writer.h"

#include <GL/gl.h>
#include <cstring>
#include <iostream.h>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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

GLBitmap::GLBitmap(istream& is, IOFlag flag) :
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

void GLBitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("GLBitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsRenderer->getUsingGlBitmap() << sep;

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { Bitmap::serialize(os, flag | TYPENAME); }
}

void GLBitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("GLBitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  int val;
  is >> val;
  itsRenderer->setUsingGlBitmap(bool(val));

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { Bitmap::deserialize(is, flag | TYPENAME); }
}

int GLBitmap::charCount() const {
DOTRACE("GLBitmap::charCount");
  return 128;
}

void GLBitmap::readFrom(Reader* reader) {
DOTRACE("GLBitmap::readFrom");
  bool val;
  reader->readValue("usingGlBitmap", val); 
  itsRenderer->setUsingGlBitmap(val);

  Bitmap::readFrom(reader);
}

void GLBitmap::writeTo(Writer* writer) const {
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
