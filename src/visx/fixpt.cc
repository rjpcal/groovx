///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Thu Mar  9 15:38:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include "reader.h"
#include "rect.h"
#include "writer.h"

#include <iostream.h>
#include <GL/gl.h>

#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "FixPt";

  const FixPt::PInfo PINFOS[] = {
		FixPt::PInfo("length", &FixPt::length, 0.0, 10.0, 0.1, true),
		FixPt::PInfo("width", &FixPt::width, 0, 100, 1)
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

FixPt::FixPt(istream &is, IOFlag flag) :
  length(0.1), width(1)
{
  deserialize(is, flag);
}

FixPt::~FixPt() {}

void FixPt::serialize(ostream &os, IOFlag flag) const {
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << length() << sep;
  os << width() << endl;
  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void FixPt::deserialize(istream &is, IOFlag flag) {
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> length();
  is >> width();
  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }
}

int FixPt::charCount() const {
  return (strlen(ioTag) + 1
			 + gCharCount<double>(length()) + 1
			 + gCharCount<int>(width()) + 1
			 + GrObj::charCount()
			 + 1);// fudge factor
}

void FixPt::readFrom(Reader* reader) {
DOTRACE("FixPt::readFrom");
  reader->readValue("length", length());
  reader->readValue("width", width());

  DebugEval(length());  DebugEvalNL(width());

  GrObj::readFrom(reader);
}

void FixPt::writeTo(Writer* writer) const {
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
  bbox.left()  = bbox.bottom() = -length()/2.0;
  bbox.right() = bbox.top()    =  length()/2.0;
  
  border_pixels = 4;
}

bool FixPt::grHasBoundingBox() const {
DOTRACE("FixPt::grHasBoundingBox");
  return true;
}

void FixPt::grRender(Canvas&) const {
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(width());
  
  glBegin(GL_LINES);
  glVertex3f(0.0, -length()/2.0, 0.0);
  glVertex3f(0.0, length()/2.0, 0.0);
  glVertex3f(-length()/2.0, 0.0, 0.0);
  glVertex3f(length()/2.0, 0.0, 0.0);
  glEnd();
  
  glPopAttrib();
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
