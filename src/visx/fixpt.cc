///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Thu Sep 23 14:21:25 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include <iostream.h>
#include <string>
#include <GL/gl.h>

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "FixPt";
}

///////////////////////////////////////////////////////////////////////
//
// FixPt member functions
//
///////////////////////////////////////////////////////////////////////

FixPt::FixPt(double len, int wid) : 
  itsLength(len), itsWidth(wid) {}

FixPt::FixPt(istream &is, IOFlag flag) {
  deserialize(is, flag);
}

FixPt::~FixPt() {}

void FixPt::serialize(ostream &os, IOFlag flag) const {
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsLength << sep;
  os << itsWidth << endl;
  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag); }
}

void FixPt::deserialize(istream &is, IOFlag flag) {
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> itsLength;
  is >> itsWidth;
  if (is.fail()) throw InputError(ioTag);

  if (flag & BASES) { GrObj::deserialize(is, flag); }
}

int FixPt::charCount() const {
  return (ioTag.length() + 1
			 + gCharCount<double>(itsLength) + 1
			 + gCharCount<int>(itsWidth) + 1
			 + 1);// fudge factor
}

bool FixPt::grGetBoundingBox(double& left, double& top,
									  double& right, double& bottom,
									  int& border_pixels) const {
  left  = bottom = -itsLength/2.0;
  right = top    =  itsLength/2.0;
  
  border_pixels = itsWidth+2;

  return true;
}

void FixPt::grRender() const {
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(itsWidth);
  
  glBegin(GL_LINES);
  glVertex3f(0.0, -itsLength/2.0, 0.0);
  glVertex3f(0.0, itsLength/2.0, 0.0);
  glVertex3f(-itsLength/2.0, 0.0, 0.0);
  glVertex3f(itsLength/2.0, 0.0, 0.0);
  glEnd();
  
  glPopAttrib();
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
