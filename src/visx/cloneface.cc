///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
// Rob Peters
// created: Thu Apr 29 09:19:26 1999
// written: Thu Jun  3 10:57:16 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "cloneface.h"

#include <iostream.h>			  // for serialize
#include <string>

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data 
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "CloneFace";
}

///////////////////////////////////////////////////////////////////////
//
// CloneFace member functions
//
///////////////////////////////////////////////////////////////////////

CloneFace::CloneFace () : 
  Face(), itsEyeAspect(0.0), itsVertOffset(0.0) 
{
DOTRACE("CloneFace::CloneFace()");
  // Copy Face's control points into CloneFace's itsCtrlPnts member,
  // so that the default behavior of CloneFace is to mimic Face.
  const float* const face_pnts = Face::getCtrlPnts();
  for (int i = 0; i < 24; ++i) {
	 itsCtrlPnts[i] = face_pnts[i];
  }
}

CloneFace::CloneFace (istream& is, IOFlag flag) :
  Face()
{
DOTRACE("CloneFace::CloneFace(istream&, IOFlag)");
  deserialize(is, flag);
}


CloneFace::~CloneFace () {
DOTRACE("CloneFace::~CloneFace");
}

// In serialize/deserialize, the derived class (CloneFace) is handled
// before the base class (Face) since the first thing the virtual
// constructor sees must be the typename of the most fully derived
// class, in order to invoke the proper constructor.
void CloneFace::serialize(ostream &os, IOFlag flag) const {
DOTRACE("CloneFace::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  for (int i = 0; i < 24; ++i) {
	 os << itsCtrlPnts[i] << sep;
  }
  os << itsEyeAspect << sep;
  os << itsVertOffset << sep;

  // Always serialize Face, regardless of (flag & BASES)
  // Always use the typename in the base class, regardless of flag
  Face::serialize(os, (flag | TYPENAME));

  if (os.fail()) throw OutputError(ioTag);
}

void CloneFace::deserialize(istream &is, IOFlag flag) {
DOTRACE("CloneFace::deserialize");
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != ioTag) { 
		throw InputError(ioTag);
	 }
  }
  
  for (int i = 0; i < 24; ++i) {
	 is >> itsCtrlPnts[i];
  }
  is >> itsEyeAspect;
  is >> itsVertOffset;

  // Always deserialize Face, regardless of (flag & BASES)
  // Always use the typename in the base class, regardless of flag
  Face::deserialize(is, (flag | TYPENAME));

  // Bug in aCC requires this rather than a simple throw; see face.cc
  // for further explanation.
  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) {
 	 throw;
  }
}

const float* CloneFace::getCtrlPnts() const {
DOTRACE("CloneFace::getCtrlPnts");
  return itsCtrlPnts;
}

float CloneFace::getEyeAspect() const {
DOTRACE("CloneFace::getEyeAspect");
  return itsEyeAspect;
}

float CloneFace::getVertOffset() const {
DOTRACE("CloneFace::getVertOffset");
  return itsVertOffset;
}

static const char vcid_cloneface_cc[] = "$Header$";
#endif // !CLONEFACE_CC_DEFINED
