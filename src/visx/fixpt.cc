///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Fri Sep 29 16:07:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include "rect.h"

#include "io/iolegacy.h"
#include "io/ioproxy.h"
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

void FixPt::legacySrlz(IO::LegacyWriter* lwriter) const {

  lwriter->writeDouble("length", length());
  lwriter->setFieldSeparator('\n');
  lwriter->writeInt("width", width());

  IO::ConstIoProxy<GrObj> baseclass(this);
  lwriter->writeBaseClass("GrObj", &baseclass);
}

void FixPt::legacyDesrlz(IO::LegacyReader* lreader) {

  lreader->readValue("length", length());
  lreader->readValue("width", width());

  IO::IoProxy<GrObj> baseclass(this);
  lreader->readBaseClass("GrObj", &baseclass);
}

void FixPt::readFrom(IO::Reader* reader) {
DOTRACE("FixPt::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  reader->readValue("length", length());
  reader->readValue("width", width());

  DebugEval(length());  DebugEvalNL(width());

  GrObj::readFrom(reader);
}

void FixPt::writeTo(IO::Writer* writer) const {
DOTRACE("FixPt::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

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
