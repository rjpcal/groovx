///////////////////////////////////////////////////////////////////////
// jitter.cc
// Rob Peters
// created: Wed Apr  7 13:46:41 1999
// written: Sun Apr 25 13:19:31 1999
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

Jitter::Jitter(istream &is, IOFlag flag) : 
  Position() 
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
  if (flag & TYPENAME) { os << typeid(Jitter).name() << sep; }

  os << itsXJitter << sep
	  << itsYJitter << sep
	  << itsRJitter << endl;

  // The base class (Position) is always serialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::serialize(os, IOFlag(flag | TYPENAME));
  if (os.fail()) throw OutputError(typeid(Jitter));
}

void Jitter::deserialize(istream &is, IOFlag flag) {
DOTRACE("Jitter::deserialize");
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(Jitter).name()) {
		throw InputError(typeid(Jitter));
	 }
  }

  is >> itsXJitter >> itsYJitter >> itsRJitter;
#ifdef LOCAL_DEBUG
  DUMP_VAL1(flag);
  DUMP_VAL1(itsXJitter);
  DUMP_VAL1(itsYJitter);
  DUMP_VAL2(itsRJitter);
#endif
  // The base class (Position) is always deserialized, regardless of flag.
  // The typename in the base class is always used, regardless of flag.
  Position::deserialize(is, IOFlag(flag | TYPENAME));
  if (is.fail()) throw InputError(typeid(Jitter));
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
