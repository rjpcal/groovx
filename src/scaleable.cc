///////////////////////////////////////////////////////////////////////
// scaleable.cc
// Rob Peters
// created: Tue Mar  2 17:17:29 1999
// written: Fri Mar 12 11:26:40 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef SCALEABLE_CC_DEFINED
#define SCALEABLE_CC_DEFINED

#include "scaleable.h"

#include <GL/gl.h>
#include <iostream.h>

#define NO_TRACE
#include "trace.h"
#include "debug.h"

Scaleable::Scaleable () :
  GrObj(),
  itsTranslate_x(0.0), itsTranslate_y(0.0), itsTranslate_z(0.0),
  itsScale_x(1.0), itsScale_y(1.0), itsScale_z(1.0), 
  itsRotAxis_x(0.0), itsRotAxis_y(0.0), itsRotAxis_z(1.0),
  itsRotAngle(0.0) {
DOTRACE("Scaleable::Scaleable");

}

Scaleable::Scaleable (istream &is) : GrObj(is) {
DOTRACE("Scaleable::Scaleable(istream &is)");
  is >> itsTranslate_x;
  is >> itsTranslate_y;
  is >> itsTranslate_z;
  is >> itsScale_x;
  is >> itsScale_y;
  is >> itsScale_z;
  is >> itsRotAxis_x;
  is >> itsRotAxis_y;
  is >> itsRotAxis_z;
  is >> itsRotAngle;
}

Scaleable::~Scaleable () {
DOTRACE("Scaleable::~Scaleable");
}

IOResult Scaleable::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Scaleable::serialize");
  GrObj::serialize(os);
  char sep = '\t';
  os << itsTranslate_x << sep
     << itsTranslate_y << sep
     << itsTranslate_z << sep;
  os << itsScale_x << sep
     << itsScale_y << sep
     << itsScale_z << sep;
  os << itsRotAxis_x << sep
	  << itsRotAxis_y << sep
	  << itsRotAxis_z << sep
	  << itsRotAngle << sep;
  return checkStream(os);
}

void Scaleable::doTranslate() const {
  glTranslatef(itsTranslate_x, itsTranslate_y, itsTranslate_z);
}

void Scaleable::doScale() const {
  glScalef(itsScale_x, itsScale_y, itsScale_z);
}

void Scaleable::doRotate() const {
  glRotatef(itsRotAngle, itsRotAxis_x, itsRotAxis_y, itsRotAxis_z);
}

#endif // !SCALEABLE_CC_DEFINED
