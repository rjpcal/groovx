///////////////////////////////////////////////////////////////////////
//
// position.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:15 1999
// written: Tue Nov 28 18:17:21 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include "io/reader.h"
#include "io/writer.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

struct PositionData {
  PositionData() :
	 tr_x(0.0), tr_y(0.0), tr_z(0.0),
	 sc_x(1.0), sc_y(1.0), sc_z(1.0),
	 rt_x(1.0), rt_y(1.0), rt_z(1.0), rt_ang(0.0)
	 {}

  double tr_x, tr_y, tr_z;		  // x,y,z coord shift
  double sc_x, sc_y, sc_z;		  // x,y,z scaling
  double rt_x, rt_y, rt_z;		  // vector of rotation axis
  double rt_ang;					  // angle in degrees of rotation around axis
};

///////////////////////////////////////////////////////////////////////
//
// PositionImpl structure 
//
///////////////////////////////////////////////////////////////////////

struct PositionImpl {
  PositionData cur;
  PositionData last;
};

///////////////////////////////////////////////////////////////////////
//
// Position member functions
//
///////////////////////////////////////////////////////////////////////

Position* Position::make() {
DOTRACE("Position::make");
  return new Position;
}

Position::Position() : 
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position()");

  DebugEvalNL((void *) itsImpl);

  // empty
  Invariant(check());
}

Position::~Position() {
DOTRACE("Position::~Position");
  delete itsImpl;
}

void Position::readFrom(IO::Reader* reader) {
DOTRACE("Position::readFrom");

  reader->readValue("transX", itsImpl->cur.tr_x);
  reader->readValue("transY", itsImpl->cur.tr_y);
  reader->readValue("transZ", itsImpl->cur.tr_z);

  reader->readValue("scaleX", itsImpl->cur.sc_x);
  reader->readValue("scaleY", itsImpl->cur.sc_y);
  reader->readValue("scaleZ", itsImpl->cur.sc_z);

  reader->readValue("rotateX", itsImpl->cur.rt_x);
  reader->readValue("rotateY", itsImpl->cur.rt_y);
  reader->readValue("rotateZ", itsImpl->cur.rt_z);
  reader->readValue("rotateAngle", itsImpl->cur.rt_ang);
}

void Position::writeTo(IO::Writer* writer) const {
DOTRACE("Position::writeTo");

  writer->writeValue("transX", itsImpl->cur.tr_x);
  writer->writeValue("transY", itsImpl->cur.tr_y);
  writer->writeValue("transZ", itsImpl->cur.tr_z);

  writer->writeValue("scaleX", itsImpl->cur.sc_x);
  writer->writeValue("scaleY", itsImpl->cur.sc_y);
  writer->writeValue("scaleZ", itsImpl->cur.sc_z);

  writer->writeValue("rotateX", itsImpl->cur.rt_x);
  writer->writeValue("rotateY", itsImpl->cur.rt_y);
  writer->writeValue("rotateZ", itsImpl->cur.rt_z);
  writer->writeValue("rotateAngle", itsImpl->cur.rt_ang);
}

///////////////
// accessors //
///////////////

void Position::getRotate(double &a, double &x, double &y, double &z) const {
DOTRACE("Position::getRotate");
  Invariant(check());
  a = itsImpl->cur.rt_ang;
  x = itsImpl->cur.rt_x;
  y = itsImpl->cur.rt_y;
  z = itsImpl->cur.rt_z;
}

void Position::getTranslate(double &x, double &y, double &z) const {
DOTRACE("Position::getTranslate");
  Invariant(check());
  x = itsImpl->cur.tr_x;
  y = itsImpl->cur.tr_y;
  z = itsImpl->cur.tr_z;
}

void Position::getScale(double &x, double &y, double &z) const {
DOTRACE("Position::getScale");
  Invariant(check());
  x = itsImpl->cur.sc_x;
  y = itsImpl->cur.sc_y;
  z = itsImpl->cur.sc_z;
}

//////////////////
// manipulators //
//////////////////

void Position::setAngle(double a) {
DOTRACE("Position::setAngle");
  Invariant(check());
  itsImpl->cur.rt_ang = a;
}

void Position::setRotate(double a, double x, double y, double z) {
DOTRACE("Position::setRotate");
  Invariant(check());
  itsImpl->cur.rt_ang = a;
  itsImpl->cur.rt_x = x;
  itsImpl->cur.rt_y = y;
  itsImpl->cur.rt_z = z;
}

void Position::setScale(double x, double y, double z) {
DOTRACE("Position::setScale");
  Invariant(check());
  itsImpl->cur.sc_x = x;
  itsImpl->cur.sc_y = y;
  itsImpl->cur.sc_z = z;
}

void Position::setTranslate(double x, double y, double z) {
DOTRACE("Position::setTranslate");
  Invariant(check());
  itsImpl->cur.tr_x = x;
  itsImpl->cur.tr_y = y;
  itsImpl->cur.tr_z = z;
}

/////////////
// actions //
/////////////

void Position::translate() const {
DOTRACE("Position::translate");
  DebugEvalNL((void *) itsImpl);

  Invariant(check());
  glTranslatef(itsImpl->cur.tr_x, itsImpl->cur.tr_y, itsImpl->cur.tr_z);
}

void Position::scale() const {
DOTRACE("Position::scale");
  Invariant(check());
  glScalef(itsImpl->cur.sc_x, itsImpl->cur.sc_y, itsImpl->cur.sc_z);
}

void Position::rotate() const {
DOTRACE("Position::rotate");
  Invariant(check());
  glRotatef(itsImpl->cur.rt_ang,
				itsImpl->cur.rt_x, itsImpl->cur.rt_y, itsImpl->cur.rt_z);
}

void Position::draw(GWT::Canvas&) const {
DOTRACE("Position::draw");
  Invariant(check());
  translate();
  scale();
  rotate();

  itsImpl->last = itsImpl->cur;
}

void Position::undraw(GWT::Canvas& canvas) const {
DOTRACE("Position::undraw");

  glTranslatef(itsImpl->last.tr_x, itsImpl->last.tr_y, itsImpl->last.tr_z);
  glScalef(itsImpl->last.sc_x, itsImpl->last.sc_y, itsImpl->last.sc_z);
  glRotatef(itsImpl->last.rt_ang,
				itsImpl->last.rt_x, itsImpl->last.rt_y, itsImpl->last.rt_z);
}

bool Position::check() const {
DOTRACE("Position::check");
  return ( itsImpl != 0 );
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
