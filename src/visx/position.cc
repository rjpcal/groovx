///////////////////////////////////////////////////////////////////////
// position.cc
// Rob Peters
// created: Wed Mar 10 21:33:15 1999
// written: Fri Mar 12 11:10:58 1999
static const char vcid[] = "$Id";
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include <iostream.h>
#include <GL/gl.h>

struct PositionImpl {
  float tr_x, tr_y, tr_z;		  // x,y,z coord shift
  float sc_x, sc_y, sc_z;		  // x,y,z scaling
  float rt_x, rt_y, rt_z;		  // vector of rotation axis
  float rt_ang;					  // angle in degrees of rotation around axis
};

Position::Position () : 
  itsImpl(new PositionImpl)
{
  // empty
}

Position::~Position () {
  delete itsImpl;
}

IOResult Position::serialize(ostream &os, IOFlag flag) const {
  char sep = '\t';
  os << itsImpl->tr_x << sep
     << itsImpl->tr_y << sep
     << itsImpl->tr_z << sep;
  os << itsImpl->sc_x << sep
     << itsImpl->sc_y << sep
     << itsImpl->sc_z << sep;
  os << itsImpl->rt_x << sep
	  << itsImpl->rt_y << sep
	  << itsImpl->rt_z << sep
	  << itsImpl->rt_ang << sep;
  return checkStream(os);
}

IOResult Position::deserialize (istream &is, IOFlag flag) {
  is >> itsImpl->tr_x;
  is >> itsImpl->tr_y;
  is >> itsImpl->tr_z;
  is >> itsImpl->sc_x;
  is >> itsImpl->sc_y;
  is >> itsImpl->sc_z;
  is >> itsImpl->rt_x;
  is >> itsImpl->rt_y;
  is >> itsImpl->rt_z;
  is >> itsImpl->rt_ang;
  return checkStream(is);
}

///////////////
// accessors //
///////////////

void Position::getRotate(float &a, float &x, float &y, float &z) const {
  a = itsImpl->rt_ang;
  x = itsImpl->rt_x;
  y = itsImpl->rt_y;
  z = itsImpl->rt_z;
}

void Position::getTranslate(float &x, float &y, float &z) const {
  x = itsImpl->tr_x;
  y = itsImpl->tr_y;
  z = itsImpl->tr_z;
}

void Position::getScale(float &x, float &y, float &z) const {
  x = itsImpl->sc_x;
  y = itsImpl->sc_y;
  z = itsImpl->sc_z;
}

//////////////////
// manipulators //
//////////////////

void Position::setAngle(float a) {
  itsImpl->rt_ang = a;
}

void Position::setRotate(float a, float x, float y, float z) {
  itsImpl->rt_ang = a;
  itsImpl->rt_x = x;
  itsImpl->rt_y = y;
  itsImpl->rt_z = z;
}

void Position::setScale(float x, float y, float z) {
  itsImpl->sc_x = x;
  itsImpl->sc_y = y;
  itsImpl->sc_z = z;
}

void Position::setTranslate(float x, float y, float z) {
  itsImpl->tr_x = x;
  itsImpl->tr_y = y;
  itsImpl->tr_z = z;
}

/////////////
// actions //
/////////////

void Position::translate() const {
  glTranslatef(itsImpl->tr_x, itsImpl->tr_y, itsImpl->tr_z);
}

void Position::scale() const {
  glScalef(itsImpl->sc_x, itsImpl->sc_y, itsImpl->sc_z);
}

void Position::rotate() const {
  glRotatef(itsImpl->rt_ang, itsImpl->rt_x, itsImpl->rt_y, itsImpl->rt_z);
}

#endif // !POSITION_CC_DEFINED
