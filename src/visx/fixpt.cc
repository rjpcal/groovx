///////////////////////////////////////////////////////////////////////
// fixpt.cc
// Rob Peters
// created: Jan-99
// written: Tue Mar  9 21:33:48 1999
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include <GL/gl.h>

///////////////////////////////////////////////////////////////////////
// FixPt member functions
///////////////////////////////////////////////////////////////////////

FixPt::FixPt(float len, int wid) : 
  Scaleable(), itsLength(len), itsWidth(wid) {}

FixPt::FixPt(istream &is) : Scaleable(is) {
}

FixPt::~FixPt() {}

IOResult FixPt::serialize(ostream &os, IOFlag flag) const {
  Scaleable::serialize(os);
  return checkStream(os);
}

void FixPt::grRecompile() const {
  // dump old display list and get new one
  grNewList();

  glNewList(grDisplayList(), GL_COMPILE);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(itsWidth);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	 doTranslate();
	 doScale();

    glBegin(GL_LINES);
    glVertex3f(0.0, -itsLength/2.0, 0.0);
    glVertex3f(0.0, itsLength/2.0, 0.0);
    glVertex3f(-itsLength/2.0, 0.0, 0.0);
    glVertex3f(itsLength/2.0, 0.0, 0.0);
    glEnd();
    
    glPopMatrix();
    glPopAttrib();
  glEndList();

  grPostCompiled();
}

#endif // !FIXPT_CC_DEFINED


