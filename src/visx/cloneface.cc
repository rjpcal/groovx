///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
// Rob Peters
// created: Thu Apr 29 09:19:26 1999
// written: Wed Mar 15 10:17:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "cloneface.h"

#include "readutils.h"
#include "writeutils.h"

#include <cstring>
#include <iostream.h>			  // for serialize

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data 
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "CloneFace";
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
  const double* const face_pnts = Face::getCtrlPnts();
  for (int i = 0; i < 24; ++i) {
	 itsCtrlPnts[i] = face_pnts[i];
  }
}

CloneFace::CloneFace (istream& is, IOFlag flag) :
  Face(), itsEyeAspect(0.0), itsVertOffset(0.0) 
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

  if (os.fail()) throw OutputError(ioTag);

  // Always serialize Face, regardless of (flag & BASES)
  // Always use the typename in the base class, regardless of flag
  Face::serialize(os, (flag | TYPENAME));
}

void CloneFace::deserialize(istream &is, IOFlag flag) {
DOTRACE("CloneFace::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }
  
  for (int i = 0; i < 24; ++i) {
	 is >> itsCtrlPnts[i];
  }
  is >> itsEyeAspect;
  is >> itsVertOffset;

  // Bug in aCC requires this rather than a simple throw; see face.cc
  // for further explanation.
  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) {
 	 throw;
  }

  // Always deserialize Face, regardless of (flag & BASES)
  // Always use the typename in the base class, regardless of flag
  Face::deserialize(is, (flag | TYPENAME));
}

int CloneFace::charCount() const {
DOTRACE("CloneFace::charCount");
  return ( strlen(ioTag) + 1  
			  + 24*6 + 1
			  + gCharCount(itsEyeAspect) + 1
			  + gCharCount(itsVertOffset) + 1
			  + Face::charCount()
			  + 5 ); 
}

void CloneFace::readFrom(Reader* reader) {
DOTRACE("CloneFace::readFrom");

  ReadUtils::readValueSeq(reader, "ctrlPnts", itsCtrlPnts, (double*)0);
  reader->readValue("eyeAspect", itsEyeAspect);
  reader->readValue("vertOffset", itsVertOffset);
}

void CloneFace::writeTo(Writer* writer) const {
DOTRACE("CloneFace::writeTo");

  WriteUtils::writeValueSeq(writer, "ctrlPnts", itsCtrlPnts, itsCtrlPnts+24);
  writer->writeValue("eyeAspect", itsEyeAspect);
  writer->writeValue("vertOffset", itsVertOffset);
}

const double* CloneFace::getCtrlPnts() const {
DOTRACE("CloneFace::getCtrlPnts");
  return itsCtrlPnts;
}

double CloneFace::getEyeAspect() const {
DOTRACE("CloneFace::getEyeAspect");
  return itsEyeAspect;
}

double CloneFace::getVertOffset() const {
DOTRACE("CloneFace::getVertOffset");
  return itsVertOffset;
}

static const char vcid_cloneface_cc[] = "$Header$";
#endif // !CLONEFACE_CC_DEFINED
