///////////////////////////////////////////////////////////////////////
//
// jitter.cc
// Rob Peters
// created: Wed Apr  7 13:46:41 1999
// written: Fri Sep 29 16:10:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "jitter.h"

#include "io/iolegacy.h"
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

///////////////////////////////////////////////////////////////////////
//
// Jitter member functions
//
///////////////////////////////////////////////////////////////////////

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


// In legacySrlz/legacyDesrlz, the derived class (Jitter) is handled
// before the base class (Position), since the first thing that the
// PosMgr virtual constructor sees must be the name of the most fully
// derived class, in order to invoke the proper constructor.
void Jitter::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("Jitter::legacySrlz");

  lwriter->writeValue("jitterX", itsXJitter);
  lwriter->writeValue("jitterY", itsYJitter);
  lwriter->setFieldSeparator('\n');
  lwriter->writeValue("jitterR", itsRJitter);

  // The base class (Position) is always legacySrlzd, regardless of flag.
  IO::LWFlagJanitor(*lwriter, lwriter->flags() | IO::BASES);
  IO::ConstIoProxy<Position> baseclass(this);
  lwriter->writeBaseClass("Position", &baseclass);
}

void Jitter::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("Jitter::legacyDesrlz");

  lreader->readValue("jitterX", itsXJitter);
  lreader->readValue("jitterY", itsYJitter);
  lreader->readValue("jitterR", itsRJitter);

  // The base class (Position) is always legacyDesrlzd, regardless of flag.
  IO::LRFlagJanitor(*lreader, lreader->flags() | IO::BASES);
  IO::IoProxy<Position> baseclass(this);
  lreader->readBaseClass("Position", &baseclass);
}

void Jitter::readFrom(IO::Reader* reader) {
DOTRACE("Jitter::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  Position::readFrom(reader);

  reader->readValue("jitterX", itsXJitter);
  reader->readValue("jitterY", itsYJitter);
  reader->readValue("jitterR", itsRJitter);
}

void Jitter::writeTo(IO::Writer* writer) const {
DOTRACE("Jitter::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  Position::writeTo(writer);

  writer->writeValue("jitterX", itsXJitter);
  writer->writeValue("jitterY", itsYJitter);
  writer->writeValue("jitterR", itsRJitter);
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

void Jitter::go() const {
DOTRACE("Jitter::go");
  rejitter();
  rego();
}

void Jitter::rego() const {
DOTRACE("Jitter::rego");
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
