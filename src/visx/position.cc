///////////////////////////////////////////////////////////////////////
//
// position.cc
// Rob Peters
// created: Wed Mar 10 21:33:15 1999
// written: Sat Mar  4 16:30:53 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSITION_CC_DEFINED
#define POSITION_CC_DEFINED

#include "position.h"

#include "reader.h"
#include "writer.h"

#include <cstring>
#include <iostream.h>
#include <GL/gl.h>

#define NO_TRACE
#include "trace.h"
#define LOCAL_INVARIANT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "Position";
}

///////////////////////////////////////////////////////////////////////
//
// PositionImpl structure 
//
///////////////////////////////////////////////////////////////////////

struct PositionImpl {
  PositionImpl(double tx = 0.0, double ty = 0.0, double tz = 0.0,
               double sx = 1.0, double sy = 1.0, double sz = 1.0,
               double rx = 0.0, double ry = 0.0, double rz = 1.0, double ra = 0.0,
					double r = 1.0, double g = 1.0, double b = 1.0) :
  tr_x(tx), tr_y(ty), tr_z(tz),
  sc_x(sx), sc_y(sy), sc_z(sz),
  rt_x(rx), rt_y(ry), rt_z(rz), rt_ang(ra),
  red(r), green(g), blue(b) {}

  double tr_x, tr_y, tr_z;		  // x,y,z coord shift
  double sc_x, sc_y, sc_z;		  // x,y,z scaling
  double rt_x, rt_y, rt_z;		  // vector of rotation axis
  double rt_ang;					  // angle in degrees of rotation around axis
  double red, green, blue;		  // RGB color values ( 0.0 <= val <= 1.0 )
};

///////////////////////////////////////////////////////////////////////
//
// Position member functions
//
///////////////////////////////////////////////////////////////////////

Position::Position() : 
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position()");

  DebugEvalNL((void *) itsImpl);

  // empty
  Invariant(check());
}

Position::Position (istream& is, IOFlag flag) : 
  itsImpl(new PositionImpl)
{
DOTRACE("Position::Position(istream&, IOFlag)");
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
  if (flag & TYPENAME) { os << ioTag << sep; }

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
  if (os.fail()) throw OutputError(ioTag);
}

void Position::deserialize(istream &is, IOFlag flag) {
DOTRACE("Position::deserialize");
  Invariant(check());

  DebugEvalNL(flag);

  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

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

  DebugEval(itsImpl->tr_x);
  DebugEvalNL(itsImpl->rt_ang);

  if (is.fail()) throw InputError(ioTag);
}

int Position::charCount() const {
  return (strlen(ioTag) + 1
			 + gCharCount<double>(itsImpl->tr_x) + 1
			 + gCharCount<double>(itsImpl->tr_y) + 1
			 + gCharCount<double>(itsImpl->tr_z) + 1
			 + gCharCount<double>(itsImpl->sc_x) + 1
			 + gCharCount<double>(itsImpl->sc_y) + 1
			 + gCharCount<double>(itsImpl->sc_z) + 1
			 + gCharCount<double>(itsImpl->rt_x) + 1
			 + gCharCount<double>(itsImpl->rt_y) + 1
			 + gCharCount<double>(itsImpl->rt_z) + 1
			 + gCharCount<double>(itsImpl->rt_ang) + 1
			 + 1);// fudge factor
}

void Position::readFrom(Reader* reader) {
DOTRACE("Position::readFrom");

  reader->readValue("transX", itsImpl->tr_x);
  reader->readValue("transY", itsImpl->tr_y);
  reader->readValue("transZ", itsImpl->tr_z);

  reader->readValue("scaleX", itsImpl->sc_x);
  reader->readValue("scaleY", itsImpl->sc_y);
  reader->readValue("scaleZ", itsImpl->sc_z);

  reader->readValue("rotateX", itsImpl->rt_x);
  reader->readValue("rotateY", itsImpl->rt_y);
  reader->readValue("rotateZ", itsImpl->rt_z);
  reader->readValue("rotateAngle", itsImpl->rt_ang);
}

void Position::writeTo(Writer* writer) const {
DOTRACE("Position::writeTo");

  writer->writeValue("transX", itsImpl->tr_x);
  writer->writeValue("transY", itsImpl->tr_y);
  writer->writeValue("transZ", itsImpl->tr_z);

  writer->writeValue("scaleX", itsImpl->sc_x);
  writer->writeValue("scaleY", itsImpl->sc_y);
  writer->writeValue("scaleZ", itsImpl->sc_z);

  writer->writeValue("rotateX", itsImpl->rt_x);
  writer->writeValue("rotateY", itsImpl->rt_y);
  writer->writeValue("rotateZ", itsImpl->rt_z);
  writer->writeValue("rotateAngle", itsImpl->rt_ang);
}

///////////////
// accessors //
///////////////

void Position::getRotate(double &a, double &x, double &y, double &z) const {
DOTRACE("Position::getRotate");
  Invariant(check());
  a = itsImpl->rt_ang;
  x = itsImpl->rt_x;
  y = itsImpl->rt_y;
  z = itsImpl->rt_z;
}

void Position::getTranslate(double &x, double &y, double &z) const {
DOTRACE("Position::getTranslate");
  Invariant(check());
  x = itsImpl->tr_x;
  y = itsImpl->tr_y;
  z = itsImpl->tr_z;
}

void Position::getScale(double &x, double &y, double &z) const {
DOTRACE("Position::getScale");
  Invariant(check());
  x = itsImpl->sc_x;
  y = itsImpl->sc_y;
  z = itsImpl->sc_z;
}

//////////////////
// manipulators //
//////////////////

void Position::setAngle(double a) {
DOTRACE("Position::setAngle");
  Invariant(check());
  itsImpl->rt_ang = a;
}

void Position::setRotate(double a, double x, double y, double z) {
DOTRACE("Position::setRotate");
  Invariant(check());
  itsImpl->rt_ang = a;
  itsImpl->rt_x = x;
  itsImpl->rt_y = y;
  itsImpl->rt_z = z;
}

void Position::setScale(double x, double y, double z) {
DOTRACE("Position::setScale");
  Invariant(check());
  itsImpl->sc_x = x;
  itsImpl->sc_y = y;
  itsImpl->sc_z = z;
}

void Position::setTranslate(double x, double y, double z) {
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
  DebugEvalNL((void *) itsImpl);

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

void Position::rego() const {
DOTRACE("Position::rego");
  go();
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
