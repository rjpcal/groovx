///////////////////////////////////////////////////////////////////////
//
// jitter.cc
// Rob Peters
// created: Wed Apr  7 13:46:41 1999
// written: Wed Mar 15 10:17:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "jitter.h"

#include "randutils.h"
#include "reader.h"
#include "writer.h"

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

Jitter::Jitter(istream &is, IOFlag flag) : 
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
void Jitter::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Jitter::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  os << itsXJitter << sep
	  << itsYJitter << sep
	  << itsRJitter << endl;

  if (os.fail()) throw OutputError(ioTag);

  // The base class (Position) is always serialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::serialize(os, (flag | TYPENAME));
}

void Jitter::deserialize(istream &is, IOFlag flag) {
DOTRACE("Jitter::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> itsXJitter >> itsYJitter >> itsRJitter;

  DebugEval(flag);
  DebugEval(itsXJitter);
  DebugEval(itsYJitter);
  DebugEvalNL(itsRJitter);

  if (is.fail()) throw InputError(ioTag);

  // The base class (Position) is always deserialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::deserialize(is, (flag | TYPENAME));
}

int Jitter::charCount() const {
  return (strlen(ioTag) + 1
			 + gCharCount<double>(itsXJitter) + 1
			 + gCharCount<double>(itsYJitter) + 1
			 + gCharCount<double>(itsRJitter) + 1
			 + Position::charCount()
			 + 1);// fudge factor
}

void Jitter::readFrom(Reader* reader) {
DOTRACE("Jitter::readFrom");

  Position::readFrom(reader);

  reader->readValue("jitterX", itsXJitter);
  reader->readValue("jitterY", itsYJitter);
  reader->readValue("jitterR", itsRJitter);
}

void Jitter::writeTo(Writer* writer) const {
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
  glTranslatef(x+randDoubleRange(-itsXJitter, itsXJitter),
					y+randDoubleRange(-itsYJitter, itsYJitter),
					z);
}

void Jitter::rotate() const {
DOTRACE("Jitter::rotate");
  double a,x,y,z;
  getRotate(a,x,y,z);
  glRotatef(a+randDoubleRange(-itsRJitter, itsRJitter),
				x, y, z);
}

void Jitter::rejitter() const {
DOTRACE("Jitter::rejitter");
  itsXShift = randDoubleRange(-itsXJitter, itsXJitter);
  itsYShift = randDoubleRange(-itsYJitter, itsYJitter);
  itsRShift = randDoubleRange(-itsRJitter, itsRJitter);
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
