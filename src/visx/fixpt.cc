///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Wed Sep 27 11:48:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include "rect.h"

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

#ifdef MIPSPRO_COMPILER
#  define SGI_IDIOT_CAST(to, from) reinterpret_cast<to>(from)
#else
#  define SGI_IDIOT_CAST(to, from) (from)
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "FixPt";

  const FixPt::PInfo PINFOS[] = {
		FixPt::PInfo("length", SGI_IDIOT_CAST(Property FixPt::*, &FixPt::length),
						 0.0, 10.0, 0.1, true),
		FixPt::PInfo("width", SGI_IDIOT_CAST(Property FixPt::*, &FixPt::width),
						 0, 100, 1)
  };

  const unsigned int NUM_PINFOS = sizeof(PINFOS)/sizeof(FixPt::PInfo);
}

///////////////////////////////////////////////////////////////////////
//
// FixPt member functions
//
///////////////////////////////////////////////////////////////////////

FixPt::FixPt(double len, int wid) : 
  length(len), width(wid) {}

FixPt::~FixPt() {}

void FixPt::legacySrlz(IO::Writer* writer) const {
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();

	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 os << length() << sep;
	 os << width() << endl;
	 if (os.fail()) throw IO::OutputError(ioTag);

	 if (lwriter->flags() & IO::BASES) {
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		GrObj::legacySrlz(writer);
	 }
  }
}

void FixPt::legacyDesrlz(IO::Reader* reader) {
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

	 is >> length();
	 is >> width();
	 if (is.fail()) throw IO::InputError(ioTag);

	 if (lreader->flags() & IO::BASES) {
		IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::TYPENAME);
		GrObj::legacyDesrlz(reader);
	 }
  }
}

void FixPt::readFrom(IO::Reader* reader) {
DOTRACE("FixPt::readFrom");
  reader->readValue("length", length());
  reader->readValue("width", width());

  DebugEval(length());  DebugEvalNL(width());

  GrObj::readFrom(reader);
}

void FixPt::writeTo(IO::Writer* writer) const {
DOTRACE("FixPt::writeTo");
  writer->writeDouble("length", length());
  writer->writeInt("width", width());

  GrObj::writeTo(writer);
}

unsigned int FixPt::numPropertyInfos() {
DOTRACE("FixPt::numPropertyInfos");
  return NUM_PINFOS;
}

const FixPt::PInfo& FixPt::getPropertyInfo(unsigned int i) {
DOTRACE("FixPt::getPropertyInfo");
  return PINFOS[i];
}

void FixPt::grGetBoundingBox(Rect<double>& bbox,
									  int& border_pixels) const {
DOTRACE("FixPt::grGetBoundingBox");
  bbox.left()  = bbox.bottom() = -length()/2.0;
  bbox.right() = bbox.top()    =  length()/2.0;
  
  border_pixels = 4;
}

bool FixPt::grHasBoundingBox() const {
DOTRACE("FixPt::grHasBoundingBox");
  return true;
}

void FixPt::grRender(GWT::Canvas&) const {
DOTRACE("FixPt::grRender");
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(width());

  glBegin(GL_LINES);
  glVertex3f(0.0, -length()/2.0, 0.0);
  glVertex3f(0.0, length()/2.0, 0.0);
  glEnd();

  glFlush();

  glBegin(GL_LINES);
  glVertex3f(-length()/2.0, 0.0, 0.0);
  glVertex3f(length()/2.0, 0.0, 0.0);
  glEnd();
  
  glPopAttrib();
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
