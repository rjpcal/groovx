///////////////////////////////////////////////////////////////////////
//
// xbitmap.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:37:04 1999
// written: Wed Sep 27 11:48:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_CC_DEFINED
#define XBITMAP_CC_DEFINED

#include "xbitmap.h"

#include "io/iolegacy.h"
#include "xbmaprenderer.h"

#include <cstring>

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

void XBitmap::legacySrlz(IO::Writer* writer) const {
DOTRACE("XBitmap::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();
	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 if (os.fail()) throw IO::OutputError(ioTag);

	 if (lwriter->flags() & IO::BASES) {
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		Bitmap::legacySrlz(writer);
	 }
  }
}

void XBitmap::legacyDesrlz(IO::Reader* reader) {
DOTRACE("XBitmap::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

	 if (is.fail()) throw IO::InputError(ioTag);

	 if (lreader->flags() & IO::BASES) {
		IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::TYPENAME);
		Bitmap::legacyDesrlz(reader);
	 }
  }
}

static const char vcid_xbitmap_cc[] = "$Header$";
#endif // !XBITMAP_CC_DEFINED
