///////////////////////////////////////////////////////////////////////
//
// glbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:17 1999
// written: Wed Sep 27 11:48:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_CC_DEFINED
#define GLBITMAP_CC_DEFINED

#include "glbitmap.h"

#include "glbmaprenderer.h"

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>
#include <cstring>

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

void GLBitmap::legacySrlz(IO::Writer* writer) const {
DOTRACE("GLBitmap::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();

	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 os << itsRenderer->getUsingGlBitmap() << sep;

	 if (os.fail()) throw IO::OutputError(ioTag);

	 if (lwriter->flags() & IO::BASES) {
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		Bitmap::legacySrlz(writer);
	 }
  }
}

void GLBitmap::legacyDesrlz(IO::Reader* reader) {
DOTRACE("GLBitmap::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

	 int val;
	 is >> val;
	 itsRenderer->setUsingGlBitmap(bool(val));

	 if (is.fail()) throw IO::InputError(ioTag);

	 if (lreader->flags() & IO::BASES) {
		IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::TYPENAME);
		Bitmap::legacyDesrlz(reader);
	 }
  }
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
