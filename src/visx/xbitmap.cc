///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Wed Nov 24 12:09:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include <string>

#include "toglconfig.h"
#include "xbmaprenderer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  const string ioTag = "XBitmap";

  XBmapRenderer* tempRenderer = 0;
}

void XBitmap::initClass(const ToglConfig* config) {
DOTRACE("XBitmap::initClass");
  XBmapRenderer::initClass(config);
}

//////////////
// creators //
//////////////

XBitmap::XBitmap() :
  Bitmap(tempRenderer = new XBmapRenderer()),
  itsRenderer(tempRenderer)
{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(const char* filename) :
  Bitmap(tempRenderer = new XBmapRenderer(), filename),
  itsRenderer(tempRenderer)

{
DOTRACE("XBitmap::XBitmap");
  init();
}

XBitmap::XBitmap(istream& is, IOFlag flag) :
  Bitmap(tempRenderer = new XBmapRenderer()),
  itsRenderer(tempRenderer)

{
DOTRACE("XBitmap::XBitmap");
  init();
  deserialize(is, flag);
}

void XBitmap::init() {
DOTRACE("XBitmap::init");

  GrObj::setRenderMode(GROBJ_DIRECT_RENDER);
  GrObj::setUnRenderMode(GROBJ_CLEAR_BOUNDING_BOX);
  Bitmap::setUsingZoom(false);
}

XBitmap::~XBitmap() {
DOTRACE("XBitmap::~XBitmap");
  delete itsRenderer; 
}

void XBitmap::serialize(ostream& os, IOFlag flag) const {
DOTRACE("XBitmap::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { Bitmap::serialize(os, flag | TYPENAME); }
}

void XBitmap::deserialize(istream& is, IOFlag flag) {
DOTRACE("XBitmap::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { Bitmap::deserialize(is, flag | TYPENAME); }
}

int XBitmap::charCount() const {
DOTRACE("XBitmap::charCount");
  return 128;
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
