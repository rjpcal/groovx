///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Sat Sep 23 15:32:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include "xbmaprenderer.h"

#include <cstring>
#include <iostream.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const char* ioTag = "XBitmap";

  XBmapRenderer* tempRenderer = 0;
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

XBitmap::XBitmap(STD_IO::istream& is, IO::IOFlag flag) :
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

void XBitmap::serialize(STD_IO::ostream& os, IO::IOFlag flag) const {
DOTRACE("XBitmap::serialize");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  if (os.fail()) throw IO::OutputError(ioTag);

  if (flag & IO::BASES) { Bitmap::serialize(os, flag | IO::TYPENAME); }
}

void XBitmap::deserialize(STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("XBitmap::deserialize");
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  if (is.fail()) throw IO::InputError(ioTag);

  if (flag & IO::BASES) { Bitmap::deserialize(is, flag | IO::TYPENAME); }
}

int XBitmap::charCount() const {
DOTRACE("XBitmap::charCount");
  return 128;
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
