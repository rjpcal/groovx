///////////////////////////////////////////////////////////////////////
//
// jitter.cc
// Rob Peters
// created: Wed Apr  7 13:46:41 1999
// written: Sat Sep 23 15:32:25 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "jitter.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/randutils.h"

#include <cstring>
#include <iostream.h>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "Jitter";
}

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

Jitter::Jitter(STD_IO::istream &is, IO::IOFlag flag) : 
  Position() ,
  itsXJitter(0.0), itsYJitter(0.0), itsRJitter(0.0),
  itsXShift(0.0), itsYShift(0.0), itsRShift(0.0)
{
DOTRACE("Jitter::Jitter");
  deserialize(is, flag);
}

Jitter::~Jitter () {
DOTRACE("Jitter::~Jitter");
  // empty
}


// In serialize/deserialize, the derived class (Jitter) is handled
// before the base class (Position), since the first thing that the
// PosMgr virtual constructor sees must be the name of the most fully
// derived class, in order to invoke the proper constructor.
void Jitter::serialize(STD_IO::ostream &os, IO::IOFlag flag) const {
DOTRACE("Jitter::serialize");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  os << itsXJitter << sep
	  << itsYJitter << sep
	  << itsRJitter << endl;

  if (os.fail()) throw IO::OutputError(ioTag);

  // The base class (Position) is always serialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::serialize(os, (flag | IO::TYPENAME));
}

void Jitter::deserialize(STD_IO::istream &is, IO::IOFlag flag) {
DOTRACE("Jitter::deserialize");
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  is >> itsXJitter >> itsYJitter >> itsRJitter;

  DebugEval(flag);
  DebugEval(itsXJitter);
  DebugEval(itsYJitter);
  DebugEvalNL(itsRJitter);

  if (is.fail()) throw IO::InputError(ioTag);

  // The base class (Position) is always deserialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::deserialize(is, (flag | IO::TYPENAME));
}

int Jitter::charCount() const {
  return (strlen(ioTag) + 1
			 + IO::gCharCount<double>(itsXJitter) + 1
			 + IO::gCharCount<double>(itsYJitter) + 1
			 + IO::gCharCount<double>(itsRJitter) + 1
			 + Position::charCount()
			 + 1);// fudge factor
}

void Jitter::readFrom(IO::Reader* reader) {
DOTRACE("Jitter::readFrom");

  Position::readFrom(reader);

  reader->readValue("jitterX", itsXJitter);
  reader->readValue("jitterY", itsYJitter);
  reader->readValue("jitterR", itsRJitter);
}

void Jitter::writeTo(IO::Writer* writer) const {
DOTRACE("Jitter::writeTo");

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
