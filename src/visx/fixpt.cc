///////////////////////////////////////////////////////////////////////
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Tue Mar 16 19:43:52 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <GL/gl.h>

///////////////////////////////////////////////////////////////////////
// FixPt member functions
///////////////////////////////////////////////////////////////////////

FixPt::FixPt(float len, int wid) : 
  itsLength(len), itsWidth(wid) {}

FixPt::FixPt(istream &is, IOFlag flag) {
  deserialize(is, flag);
}

FixPt::~FixPt() {}

IOResult FixPt::serialize(ostream &os, IOFlag flag) const {
  if (flag & IO::BASES) { GrObj::serialize(os, flag); }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(FixPt).name() << sep; }

  os << itsLength << sep;
  os << itsWidth << endl;
  return checkStream(os);
}

IOResult FixPt::deserialize(istream &is, IOFlag flag) {
  if (flag & IO::BASES) { GrObj::deserialize(is, flag); }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != string(typeid(FixPt).name())) { return IO_ERROR; }
  }
  is >> itsLength;
  is >> itsWidth;
  return checkStream(is);
}

void FixPt::grRecompile() const {
  // dump old display list and get new one
  grNewList();

  glNewList(grDisplayList(), GL_COMPILE);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(itsWidth);

    glBegin(GL_LINES);
    glVertex3f(0.0, -itsLength/2.0, 0.0);
    glVertex3f(0.0, itsLength/2.0, 0.0);
    glVertex3f(-itsLength/2.0, 0.0, 0.0);
    glVertex3f(itsLength/2.0, 0.0, 0.0);
    glEnd();
    
    glPopAttrib();
  glEndList();

  grPostCompiled();
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
