///////////////////////////////////////////////////////////////////////
//
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Fri Sep 17 09:58:52 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <GL/glu.h>

#include "error.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

class GrObjError : public virtual ErrorWithMsg {
public:
  GrObjError(const string& msg = "") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
#ifdef LOCAL_DEBUG
  inline void checkForGlError(const char* where) throw (GrObjError) {
	 GLenum status = glGetError();
	 if (status != GL_NO_ERROR) {
		string msg = reinterpret_cast<const char*>(gluErrorString(status));
		throw GrObjError(string ("GL error: ") + msg + " " + where);
	 }
  }
#else
#  define checkForGlError(x) {}
#endif
}

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl structure
//
///////////////////////////////////////////////////////////////////////

class GrObjImpl {
public:
  GrObjImpl() : isUsed(false) {};
  bool isUsed;
};

///////////////////////////////////////////////////////////////////////
//
// GrObj member definitions
//
///////////////////////////////////////////////////////////////////////

const GrObj::GrObjRenderMode GrObj::GROBJ_DIRECT_RENDER;
const GrObj::GrObjRenderMode GrObj::GROBJ_GL_COMPILE;
const GrObj::GrObjRenderMode GrObj::GROBJ_GL_BITMAP_CACHE;
const GrObj::GrObjRenderMode GrObj::GROBJ_X11_BITMAP_CACHE;

// GrObj default constructor
GrObj::GrObj() : 
  itsIsCurrent(0),
  itsUsingCompile(true),
  itsDisplayList(-1),
  itsImpl(new GrObjImpl)
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
  itsDisplayList(-1),
  itsImpl(new GrObjImpl)
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
  delete itsImpl;
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
void GrObj::serialize(ostream&, IOFlag) const {
DOTRACE("GrObj::serialize");
}

void GrObj::deserialize(istream&, IOFlag) {
DOTRACE("GrObj::deserialize");
}

bool GrObj::getBoundingBox(double& /*left*/, double& /*top*/,
									double& /*right*/, double& /*bottom*/) {
DOTRACE("GrObj::getBoundingBox");
  return false;
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
  checkForGlError("before GrObj::update");
  if ( itsUsingCompile && !grIsCurrent() ) grRecompile(); 
  checkForGlError("during GrObj::update");
}

void GrObj::draw() const {
DOTRACE("GrObj::draw");
  checkForGlError("before GrObj::draw");
  if (itsUsingCompile) { 
	 update();
	 glCallList( itsDisplayList );
  }
  else {
	 grRender();
  }
  checkForGlError("during GrObj::draw");
}

void GrObj::undraw() const {
DOTRACE("GrObj::undraw");
  checkForGlError("before GrObj::undraw");

  GLint foreground, background;
  glGetIntegerv(GL_CURRENT_INDEX, &foreground);
  glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
  glIndexi(background);
  glClearIndex(foreground);

  if (itsUsingCompile) {
	 // Since we don't do a recompile of the display list here, we must
	 // explicitly check that the display list is valid, since it might
	 // be invalid if the object was recently constructed, for example.
	 if (glIsList(itsDisplayList) == GL_TRUE) {
		glCallList( itsDisplayList ); DebugEvalNL(itsDisplayList);
	 }
  }
  else {
	 grRender();
  }
  glIndexi(foreground);
  glClearIndex(background);

  checkForGlError("during GrObj::undraw");
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
