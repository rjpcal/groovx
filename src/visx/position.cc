///////////////////////////////////////////////////////////////////////
// position.cc
// Rob Peters
// created: Wed Mar 10 21:33:15 1999
// written: Tue Mar 16 19:36:05 1999
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
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// PositionImpl structure 
///////////////////////////////////////////////////////////////////////

struct PositionImpl {
  PositionImpl(float tx = 0.0, float ty = 0.0, float tz = 0.0,
               float sx = 1.0, float sy = 1.0, float sz = 1.0,
               float rx = 0.0, float ry = 0.0, float rz = 1.0, float ra = 0.0) :
  tr_x(tx), tr_y(ty), tr_z(tz),
  sc_x(sx), sc_y(sy), sc_z(sz),
  rt_x(rx), rt_y(ry), rt_z(rz), rt_ang(ra) {}

  float tr_x, tr_y, tr_z;       // x,y,z coord shift
  float sc_x, sc_y, sc_z;       // x,y,z scaling
  float rt_x, rt_y, rt_z;       // vector of rotation axis
  float rt_ang;                 // angle in degrees of rotation around axis
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

IOResult Position::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Position::serialize");
  Assert(itsImpl!=0);
  if (flag & IO::BASES) { /* there are no bases to serialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Position).name() << sep; }

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
  return checkStream(os);
}

IOResult Position::deserialize(istream &is, IOFlag flag) {
DOTRACE("Position::deserialize");
  Assert(itsImpl!=0);
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
#ifdef LOCAL_DEBUG
    DUMP_VAL2(name);
#endif
    if (name != string(typeid(Position).name())) { return IO_ERROR; }
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
  return checkStream(is);
}

///////////////
// accessors //
///////////////

void Position::getRotate(float &a, float &x, float &y, float &z) const {
DOTRACE("Position::getRotate");
  Assert(itsImpl!=0);
  a = itsImpl->rt_ang;
  x = itsImpl->rt_x;
  y = itsImpl->rt_y;
  z = itsImpl->rt_z;
}

void Position::getTranslate(float &x, float &y, float &z) const {
DOTRACE("Position::getTranslate");
  Assert(itsImpl!=0);
  x = itsImpl->tr_x;
  y = itsImpl->tr_y;
  z = itsImpl->tr_z;
}

void Position::getScale(float &x, float &y, float &z) const {
DOTRACE("Position::getScale");
  Assert(itsImpl!=0);
  x = itsImpl->sc_x;
  y = itsImpl->sc_y;
  z = itsImpl->sc_z;
}

//////////////////
// manipulators //
//////////////////

void Position::setAngle(float a) {
DOTRACE("Position::setAngle");
  Assert(itsImpl!=0);
  itsImpl->rt_ang = a;
}

void Position::setRotate(float a, float x, float y, float z) {
DOTRACE("Position::setRotate");
  Assert(itsImpl!=0);
  itsImpl->rt_ang = a;
  itsImpl->rt_x = x;
  itsImpl->rt_y = y;
  itsImpl->rt_z = z;
}

void Position::setScale(float x, float y, float z) {
DOTRACE("Position::setScale");
  Assert(itsImpl!=0);
  itsImpl->sc_x = x;
  itsImpl->sc_y = y;
  itsImpl->sc_z = z;
}

void Position::setTranslate(float x, float y, float z) {
DOTRACE("Position::setTranslate");
  Assert(itsImpl!=0);
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
  Assert(itsImpl!=0);
  glTranslatef(itsImpl->tr_x, itsImpl->tr_y, itsImpl->tr_z);
}

void Position::scale() const {
DOTRACE("Position::scale");
  Assert(itsImpl!=0);
  glScalef(itsImpl->sc_x, itsImpl->sc_y, itsImpl->sc_z);
}

void Position::rotate() const {
DOTRACE("Position::rotate");
  Assert(itsImpl!=0);
  glRotatef(itsImpl->rt_ang, itsImpl->rt_x, itsImpl->rt_y, itsImpl->rt_z);
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
