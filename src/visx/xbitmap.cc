///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Tue Sep 26 19:12:26 2000
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

#ifdef LEGACY
XBitmap::XBitmap(STD_IO::istream& is, IO::IOFlag flag) :
  Bitmap(tempRenderer = new XBmapRenderer()),
  itsRenderer(tempRenderer)

{
DOTRACE("XBitmap::XBitmap");
  init();
  legacyDesrlz(is, flag);
}
#endif

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

void XBitmap::legacySrlz(IO::Writer* writer, STD_IO::ostream& os, IO::IOFlag flag) const {
DOTRACE("XBitmap::legacySrlz");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  if (os.fail()) throw IO::OutputError(ioTag);

  if (flag & IO::BASES) { Bitmap::legacySrlz(writer, os, flag | IO::TYPENAME); }
}

void XBitmap::legacyDesrlz(IO::Reader* reader, STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("XBitmap::legacyDesrlz");
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  if (is.fail()) throw IO::InputError(ioTag);

  if (flag & IO::BASES) { Bitmap::legacyDesrlz(reader, is, flag | IO::TYPENAME); }
}

int XBitmap::legacyCharCount() const {
DOTRACE("XBitmap::legacyCharCount");
  return 128;
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
