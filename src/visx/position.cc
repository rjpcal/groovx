///////////////////////////////////////////////////////////////////////
// position.cc
// Rob Peters
// created: Wed Mar 10 21:33:15 1999
// written: Mon Apr 26 15:09:42 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <GL/gl.h>

#define NO_TRACE
#include "trace.h"
#define INVARIANT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// PositionImpl structure 
///////////////////////////////////////////////////////////////////////

struct PositionImpl {
  PositionImpl(float tx = 0.0, float ty = 0.0, float tz = 0.0,
               float sx = 1.0, float sy = 1.0, float sz = 1.0,
               float rx = 0.0, float ry = 0.0, float rz = 1.0, float ra = 0.0,
					float r = 1.0, float g = 1.0, float b = 1.0) :
  tr_x(tx), tr_y(ty), tr_z(tz),
  sc_x(sx), sc_y(sy), sc_z(sz),
  rt_x(rx), rt_y(ry), rt_z(rz), rt_ang(ra),
  red(r), green(g), blue(b) {}

  float tr_x, tr_y, tr_z;       // x,y,z coord shift
  float sc_x, sc_y, sc_z;       // x,y,z scaling
  float rt_x, rt_y, rt_z;       // vector of rotation axis
  float rt_ang;                 // angle in degrees of rotation around axis
  float red, blue, green;		  // RGB color values ( 0.0 <= val <= 1.0 )
};

///////////////////////////////////////////////////////////////////////
// Position member functions
///////////////////////////////////////////////////////////////////////

Position::Position() : 
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position()");
#ifdef LOCAL_DEBUG
  DUMP_VAL2((void *) itsImpl);
#endif
  // empty
  Invariant(check());
}

Position::Position (istream &is, IOFlag flag) : 
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position(istream &is)");
  deserialize(is, flag);
}

Position::~Position() {
DOTRACE("Position::~Position");
  delete itsImpl;
}

void Position::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Position::serialize");
  Invariant(check());
  if (flag & BASES) { /* there are no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(Position).name() << sep; }

  os << itsImpl->tr_x << sep
     << itsImpl->tr_y << sep
     << itsImpl->tr_z << sep;
  os << itsImpl->sc_x << sep
     << itsImpl->sc_y << sep
     << itsImpl->sc_z << sep;
  os << itsImpl->rt_x << sep
     << itsImpl->rt_y << sep
     << itsImpl->rt_z << sep
     << itsImpl->rt_ang << endl;
  if (os.fail()) throw OutputError(typeid(Position));
}

void Position::deserialize(istream &is, IOFlag flag) {
DOTRACE("Position::deserialize");
  Invariant(check());
#ifdef LOCAL_DEBUG
  DUMP_VAL2(flag);
#endif
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) {
    string name;
    is >> name;
#ifdef LOCAL_DEBUG
    DUMP_VAL2(name);
#endif
    if (name != typeid(Position).name()) { 
		throw InputError(typeid(Position));
	 }
  }

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
#ifdef LOCAL_DEBUG
  DUMP_VAL1(itsImpl->tr_x);
  DUMP_VAL2(itsImpl->rt_ang);
#endif
  if (is.fail()) throw InputError(typeid(Position));
}

///////////////
// accessors //
///////////////

void Position::getRotate(float &a, float &x, float &y, float &z) const {
DOTRACE("Position::getRotate");
  Invariant(check());
  a = itsImpl->rt_ang;
  x = itsImpl->rt_x;
  y = itsImpl->rt_y;
  z = itsImpl->rt_z;
}

void Position::getTranslate(float &x, float &y, float &z) const {
DOTRACE("Position::getTranslate");
  Invariant(check());
  x = itsImpl->tr_x;
  y = itsImpl->tr_y;
  z = itsImpl->tr_z;
}

void Position::getScale(float &x, float &y, float &z) const {
DOTRACE("Position::getScale");
  Invariant(check());
  x = itsImpl->sc_x;
  y = itsImpl->sc_y;
  z = itsImpl->sc_z;
}

//////////////////
// manipulators //
//////////////////

void Position::setAngle(float a) {
DOTRACE("Position::setAngle");
  Invariant(check());
  itsImpl->rt_ang = a;
}

void Position::setRotate(float a, float x, float y, float z) {
DOTRACE("Position::setRotate");
  Invariant(check());
  itsImpl->rt_ang = a;
  itsImpl->rt_x = x;
  itsImpl->rt_y = y;
  itsImpl->rt_z = z;
}

void Position::setScale(float x, float y, float z) {
DOTRACE("Position::setScale");
  Invariant(check());
  itsImpl->sc_x = x;
  itsImpl->sc_y = y;
  itsImpl->sc_z = z;
}

void Position::setTranslate(float x, float y, float z) {
DOTRACE("Position::setTranslate");
  Invariant(check());
  itsImpl->tr_x = x;
  itsImpl->tr_y = y;
  itsImpl->tr_z = z;
}

/////////////
// actions //
/////////////

void Position::translate() const {
DOTRACE("Position::translate");
#ifdef LOCAL_DEBUG
  DUMP_VAL2((void *) itsImpl);
#endif
  Invariant(check());
  glTranslatef(itsImpl->tr_x, itsImpl->tr_y, itsImpl->tr_z);
}

void Position::scale() const {
DOTRACE("Position::scale");
  Invariant(check());
  glScalef(itsImpl->sc_x, itsImpl->sc_y, itsImpl->sc_z);
}

void Position::rotate() const {
DOTRACE("Position::rotate");
  Invariant(check());
  glRotatef(itsImpl->rt_ang, itsImpl->rt_x, itsImpl->rt_y, itsImpl->rt_z);
}

void Position::go() const {
DOTRACE("Position::go");
  Invariant(check());
  translate();
  scale();
  rotate();
}

bool Position::check() const {
DOTRACE("Position::check");
  return ( (itsImpl != 0) &&
			  (0.0 <= itsImpl->red)   && (itsImpl->red <= 1.0) &&
			  (0.0 <= itsImpl->green) && (itsImpl->green <= 1.0) &&
			  (0.0 <= itsImpl->blue)  && (itsImpl->blue <= 1.0) );
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
