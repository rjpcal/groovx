///////////////////////////////////////////////////////////////////////
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Thu Jul  1 14:18:53 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>

#include "error.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

class GrObjError : public virtual Error {
public:
  GrObjError() : Error() {}
};

///////////////////////////////////////////////////////////////////////
// GrObj member functions
///////////////////////////////////////////////////////////////////////

// GrObj default constructor
GrObj::GrObj() : 
  itsIsCurrent(0),
  itsUsingCompile(true),
  itsDisplayList(-1)
{
DOTRACE("GrObj::GrObj");
  // The GrObj needs to observe itself in order to update its display
  // list according to state changes.
  attach(this);
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
GrObj::GrObj(istream& is, IOFlag flag) :
  itsIsCurrent(0),
  itsUsingCompile(true),
  itsDisplayList(-1)
{
DOTRACE("GrObj::GrObj(istream&)");
  deserialize(is, flag);
  itsDisplayList = glGenLists(1);

  attach(this);
}

// GrObj destructor
GrObj::~GrObj() {
DOTRACE("GrObj::~GrObj");
  glDeleteLists(itsDisplayList, 1);
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
void GrObj::serialize(ostream&, IOFlag) const {
DOTRACE("GrObj::serialize");
}

void GrObj::deserialize(istream&, IOFlag) {
DOTRACE("GrObj::deserialize");
}

void GrObj::receiveStateChangeMsg(const Observable* obj) {
DOTRACE("GrObj::receiveStateChangeMsg");
  if (obj == this) {
	 itsIsCurrent = false;
  }
}

void GrObj::receiveDestroyMsg(const Observable*) {
DOTRACE("GrObj::receiveDestroyMsg");
  // Do nothing since the only Observable that 'this' is watching is
  // itself
}

bool GrObj::getUsingCompile() const {
DOTRACE("GrObj::getUsingCompile");
  return itsUsingCompile;
}

void GrObj::setUsingCompile(bool val) {
DOTRACE("GrObj::setUsingCompile");
  itsUsingCompile = val;
  sendStateChangeMsg();
}

void GrObj::update() const {
DOTRACE("GrObj::update");
  if ( itsUsingCompile && !grIsCurrent() ) grRecompile(); 
}

void GrObj::draw() const {
DOTRACE("GrObj::draw");
  if (itsUsingCompile) { 
	 update();
	 glCallList( itsDisplayList );
  }
  else {
	 grRender();
  }
}

void GrObj::undraw() const {
DOTRACE("GrObj::undraw");
  if (itsUsingCompile) {
	 glCallList( itsDisplayList );
  }
  else {
	 grRender();
  }
}

void GrObj::grRecompile() const {
DOTRACE("GrObj::grRecompile");
  grNewList();

  glNewList(grDisplayList(), GL_COMPILE);
    grRender();
  glEndList();

  grPostUpdated();
}

bool GrObj::grIsCurrent() const {
DOTRACE("GrObj::grIsCurrent");
  return itsIsCurrent;
}

void GrObj::grPostUpdated() const {
DOTRACE("GrObj::grPostUpdated"); 
  itsIsCurrent = true; 
}

// get rid of old display list, allocate a new display list, and
// check that the allocation actually succeeded
void GrObj::grNewList() const {
DOTRACE("GrObj::grNewList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1); 
  if (itsDisplayList == 0) {     
	 cerr << "GrObj::grNewList: couldn't allocate display list\n";
	 throw GrObjError();
  }
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
