///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Apr 29 09:19:26 1999
// written: Fri Nov 10 17:27:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "cloneface.h"

#include "io/ioproxy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// CloneFace member functions
//
///////////////////////////////////////////////////////////////////////

CloneFace* CloneFace::make() {
DOTRACE("CloneFace::make");
  return new CloneFace;
}

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

CloneFace::~CloneFace () {
DOTRACE("CloneFace::~CloneFace");
}

void CloneFace::readFrom(IO::Reader* reader) {
DOTRACE("CloneFace::readFrom");

  IO::ReadUtils::readValueSeq<double>(reader, "ctrlPnts",
															  itsCtrlPnts, 24);
  reader->readValue("eyeAspect", itsEyeAspect);
  reader->readValue("vertOffset", itsVertOffset);

  IO::IoProxy<Face> baseclass(this);
  reader->readBaseClass("Face", &baseclass);
}

void CloneFace::writeTo(IO::Writer* writer) const {
DOTRACE("CloneFace::writeTo");

  IO::WriteUtils::writeValueSeq(writer, "ctrlPnts",
										  itsCtrlPnts, itsCtrlPnts+24, true);
  writer->writeValue("eyeAspect", itsEyeAspect);
  writer->writeValue("vertOffset", itsVertOffset);

  IO::ConstIoProxy<Face> baseclass(this);
  writer->writeBaseClass("Face", &baseclass);
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
