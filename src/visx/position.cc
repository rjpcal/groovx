///////////////////////////////////////////////////////////////////////
//
// position.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 10 21:33:15 1999
// written: Tue Nov 28 19:07:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include "gx/vec3.h"

#include "gwt/canvas.h"

#include "io/reader.h"
#include "io/writer.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

struct PositionData {
  PositionData() :
	 tr(0.0, 0.0, 0.0),
	 sc(1.0, 1.0, 1.0),
	 rt(0.0, 0.0, 1.0),
	 rt_ang(0.0)
	 {}

  Vec3<double> tr;				  // x,y,z coord shift
  Vec3<double> sc;				  // x,y,z scaling
  Vec3<double> rt;				  // vector of rotation axis
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

  reader->readValue("transX", itsImpl->cur.tr.x());
  reader->readValue("transY", itsImpl->cur.tr.y());
  reader->readValue("transZ", itsImpl->cur.tr.z());

  reader->readValue("scaleX", itsImpl->cur.sc.x());
  reader->readValue("scaleY", itsImpl->cur.sc.y());
  reader->readValue("scaleZ", itsImpl->cur.sc.z());

  reader->readValue("rotateX", itsImpl->cur.rt.x());
  reader->readValue("rotateY", itsImpl->cur.rt.y());
  reader->readValue("rotateZ", itsImpl->cur.rt.z());
  reader->readValue("rotateAngle", itsImpl->cur.rt_ang);
}

void Position::writeTo(IO::Writer* writer) const {
DOTRACE("Position::writeTo");

  writer->writeValue("transX", itsImpl->cur.tr.x());
  writer->writeValue("transY", itsImpl->cur.tr.y());
  writer->writeValue("transZ", itsImpl->cur.tr.z());

  writer->writeValue("scaleX", itsImpl->cur.sc.x());
  writer->writeValue("scaleY", itsImpl->cur.sc.y());
  writer->writeValue("scaleZ", itsImpl->cur.sc.z());

  writer->writeValue("rotateX", itsImpl->cur.rt.x());
  writer->writeValue("rotateY", itsImpl->cur.rt.y());
  writer->writeValue("rotateZ", itsImpl->cur.rt.z());
  writer->writeValue("rotateAngle", itsImpl->cur.rt_ang);
}

///////////////
// accessors //
///////////////

void Position::getRotate(double &a, double &x, double &y, double &z) const {
DOTRACE("Position::getRotate");
  Invariant(check());
  a = itsImpl->cur.rt_ang;
  itsImpl->cur.rt.get(x, y, z);
}

void Position::getTranslate(double &x, double &y, double &z) const {
DOTRACE("Position::getTranslate");
  Invariant(check());
  itsImpl->cur.tr.get(x, y, z);
}

void Position::getScale(double &x, double &y, double &z) const {
DOTRACE("Position::getScale");
  Invariant(check());
  itsImpl->cur.sc.get(x, y, z);
}

const Vec3<double>& Position::translation() const
{ return itsImpl->cur.tr; }

const Vec3<double>& Position::scaling() const
{ return itsImpl->cur.sc; }

const Vec3<double>& Position::rotationAxis() const
{ return itsImpl->cur.rt; }

double Position::rotationAngle() const
{ return itsImpl->cur.rt_ang; }

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
  itsImpl->cur.rt.set(x, y, z);
}

void Position::setScale(double x, double y, double z) {
DOTRACE("Position::setScale");
  Invariant(check());
  itsImpl->cur.sc.set(x, y, z);
}

void Position::setTranslate(double x, double y, double z) {
DOTRACE("Position::setTranslate");
  Invariant(check());
  itsImpl->cur.tr.set(x, y, z);
}

/////////////
// actions //
/////////////

void Position::draw(GWT::Canvas& canvas) const {
DOTRACE("Position::draw");
  Invariant(check());
  canvas.translate(itsImpl->cur.tr);
  canvas.scale(itsImpl->cur.sc);
  canvas.rotate(itsImpl->cur.rt, itsImpl->cur.rt_ang);

  itsImpl->last = itsImpl->cur;
}

void Position::undraw(GWT::Canvas& canvas) const {
DOTRACE("Position::undraw");

  canvas.translate(itsImpl->last.tr);
  canvas.scale(itsImpl->last.sc);
  canvas.rotate(itsImpl->last.rt, itsImpl->last.rt_ang);
}

bool Position::check() const {
DOTRACE("Position::check");
  return ( itsImpl != 0 );
}

static const char vcid_position_cc[] = "$Header$";
#endif // !POSITION_CC_DEFINED
