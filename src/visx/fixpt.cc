///////////////////////////////////////////////////////////////////////
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Sun Apr 25 13:19:31 1999
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

void FixPt::serialize(ostream &os, IOFlag flag) const {
  if (flag & BASES) { GrObj::serialize(os, flag); }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(FixPt).name() << sep; }

  os << itsLength << sep;
  os << itsWidth << endl;
  if (os.fail()) throw OutputError(typeid(FixPt));
}

void FixPt::deserialize(istream &is, IOFlag flag) {
  if (flag & BASES) { GrObj::deserialize(is, flag); }
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(FixPt).name()) { 
		throw InputError(typeid(FixPt));
	 }
  }
  is >> itsLength;
  is >> itsWidth;
  if (is.fail()) throw InputError(typeid(FixPt));
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
