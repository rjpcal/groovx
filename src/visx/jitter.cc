///////////////////////////////////////////////////////////////////////
//
// jitter.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr  7 13:46:41 1999
// written: Tue Nov 14 15:08:09 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "jitter.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/randutils.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const IO::VersionId JITTER_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Jitter member functions
//
///////////////////////////////////////////////////////////////////////

Jitter* Jitter::make() {
DOTRACE("Jitter::make");
  return new Jitter;
}

Jitter::Jitter () :
  Position(), 
  itsXJitter(0.0), itsYJitter(0.0), itsRJitter(0.0),
  itsXShift(0.0), itsYShift(0.0), itsRShift(0.0)
{
DOTRACE("Jitter::Jitter");
  // empty
}

Jitter::~Jitter () {
DOTRACE("Jitter::~Jitter");
  // empty
}

IO::VersionId Jitter::serialVersionId() const {
DOTRACE("Jitter::serialVersionId");
  return JITTER_SERIAL_VERSION_ID;
}

void Jitter::readFrom(IO::Reader* reader) {
DOTRACE("Jitter::readFrom");

  reader->ensureReadVersionId("Jitter", 2, "Try grsh0.8a4");

  reader->readValue("jitterX", itsXJitter);
  reader->readValue("jitterY", itsYJitter);
  reader->readValue("jitterR", itsRJitter);

  IO::IoProxy<Position> baseclass(this);
  reader->readBaseClass("Position", &baseclass);
}

void Jitter::writeTo(IO::Writer* writer) const {
DOTRACE("Jitter::writeTo");

  writer->ensureWriteVersionId("Jitter", JITTER_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  writer->writeValue("jitterX", itsXJitter);
  writer->writeValue("jitterY", itsYJitter);
  writer->writeValue("jitterR", itsRJitter);

  IO::ConstIoProxy<Position> baseclass(this);
  writer->writeBaseClass("Position", &baseclass);
}

/////////////
// actions //
/////////////

void Jitter::translate() const {
DOTRACE("Jitter::translate");
  double x,y,z;
  getTranslate(x,y,z);
  glTranslatef(x+Util::randDoubleRange(-itsXJitter, itsXJitter),
					y+Util::randDoubleRange(-itsYJitter, itsYJitter),
					z);
}

void Jitter::rotate() const {
DOTRACE("Jitter::rotate");
  double a,x,y,z;
  getRotate(a,x,y,z);
  glRotatef(a+Util::randDoubleRange(-itsRJitter, itsRJitter),
				x, y, z);
}

void Jitter::rejitter() const {
DOTRACE("Jitter::rejitter");
  itsXShift = Util::randDoubleRange(-itsXJitter, itsXJitter);
  itsYShift = Util::randDoubleRange(-itsYJitter, itsYJitter);
  itsRShift = Util::randDoubleRange(-itsRJitter, itsRJitter);
}

void Jitter::draw(GWT::Canvas& canvas) const {
DOTRACE("Jitter::draw");
  rejitter();
  undraw(canvas);
}

void Jitter::undraw(GWT::Canvas&) const {
DOTRACE("Jitter::undraw");
  // Translate
  double tx, ty, tz;
  getTranslate(tx, ty, tz);
  glTranslatef(tx+itsXShift, ty+itsYShift, tz);

  // Scale
  Position::scale();

  // Rotate
  double ra, rx, ry, rz;
  getRotate(ra, rx, ry, rz);
  glRotatef(ra+itsRShift, rx, ry, rz);
}

static const char vcid_jitter_cc[] = "$Header$";
#endif // !JITTER_CC_DEFINED
