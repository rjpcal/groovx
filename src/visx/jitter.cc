///////////////////////////////////////////////////////////////////////
// jitter.cc
// Rob Peters
// created: Wed Apr  7 13:46:41 1999
// written: Wed Apr  7 14:43:19 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "jitter.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <GL/gl.h>

#include "randutils.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Jitter member functions
///////////////////////////////////////////////////////////////////////

Jitter::Jitter () :
  Position(), 
  itsXJitter(0.0), itsYJitter(0.0), itsRJitter(0.0)
{
DOTRACE("Jitter::Jitter");
  // empty
}

Jitter::Jitter(istream &is, IOFlag flag) {
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
IOResult Jitter::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Jitter::serialize");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Jitter).name() << sep; }

  os << itsXJitter << sep
	  << itsYJitter << sep
	  << itsRJitter << endl;

  // The base class is always serialized, regardless of flag.
  Position::serialize(os, flag);
  return checkStream(os);
}

IOResult Jitter::deserialize(istream &is, IOFlag flag) {
DOTRACE("Jitter::deserialize");
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != string(typeid(Jitter).name())) { return IO_ERROR; }
  }

  is >> itsXJitter >> itsYJitter >> itsRJitter;

  // The base class is always deserialized, regardless of flag.
  Position::deserialize(is, flag);
  return checkStream(is);
}

/////////////
// actions //
/////////////

void Jitter::translate() const {
DOTRACE("Jitter::translate");
  float x,y,z;
  getTranslate(x,y,z);
  glTranslatef(x+randDoubleRange(-itsXJitter, itsXJitter),
					y+randDoubleRange(-itsYJitter, itsYJitter),
					z);
}

void Jitter::rotate() const {
DOTRACE("Jitter::rotate");
  float a,x,y,z;
  getRotate(a,x,y,z);
  glRotatef(a+randDoubleRange(-itsRJitter, itsRJitter),
				x, y, z);
}

static const char vcid_jitter_cc[] = "$Header$";
#endif // !JITTER_CC_DEFINED
