///////////////////////////////////////////////////////////////////////
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Fri Mar 12 11:01:52 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <cstdlib>				  // for exit

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#ifndef NULL
#define NULL 0L
#endif

///////////////////////////////////////////////////////////////////////
// GrObj member functions
///////////////////////////////////////////////////////////////////////

// GrObj default constructor
GrObj::GrObj(int categ) : 
  itsIsCurrent(0), itsDisplayList(-1)
{
    itsDisplayList = glGenLists(1);
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
GrObj::GrObj(istream &is) :
  itsIsCurrent(0), itsDisplayList(-1)
{
  deserialize(is);
  itsDisplayList = glGenLists(1);
}

// GrObj destructor
GrObj::~GrObj() {
  glDeleteLists(itsDisplayList, 1);
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
IOResult GrObj::serialize(ostream &os, IOFlag flag) const {
DOTRACE("GrObj::serialize");
  return IO_OK;
}

IOResult GrObj::deserialize(istream &is, IOFlag flag) {
DOTRACE("GrObj::deserialize");
  return IO_OK;
}

void GrObj::grAction() const {
DOTRACE("GrObj::grAction");
  if ( !grIsCurrent() ) grRecompile();
  glCallList( itsDisplayList );
}

// get rid of old display list, allocate a new display list, and
// check that the allocation actually succeeded
void GrObj::grNewList() const {
DOTRACE("GrObj::grNewList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1); 
  if (itsDisplayList == 0) {     
    cerr << "GrObj::grNewList: couldn't allocate display list\n";
	 exit(-1);
  }
}

#endif // !GROBJ_CC_DEFINED
