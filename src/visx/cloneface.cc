///////////////////////////////////////////////////////////////////////
//
// cloneface.cc
// Rob Peters
// created: Thu Apr 29 09:19:26 1999
// written: Wed Sep 27 14:36:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CLONEFACE_CC_DEFINED
#define CLONEFACE_CC_DEFINED

#include "cloneface.h"

#include "io/iolegacy.h"
#include "io/ioproxy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include <cstring>

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

CloneFace::~CloneFace () {
DOTRACE("CloneFace::~CloneFace");
}

// In legacySrlz/legacyDesrlz, the derived class (CloneFace) is handled
// before the base class (Face) since the first thing the virtual
// constructor sees must be the typename of the most fully derived
// class, in order to invoke the proper constructor.
void CloneFace::legacySrlz(IO::Writer* writer) const {
DOTRACE("CloneFace::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 lwriter->writeTypename(ioTag);

	 ostream& os = lwriter->output();

	 for (int i = 0; i < 24; ++i) {
		os << itsCtrlPnts[i] << IO::SEP;
	 }
	 os << itsEyeAspect << IO::SEP;
	 os << itsVertOffset << IO::SEP;

	 lwriter->throwIfError(ioTag);

	 // Always legacySrlz Face, regardless of lwriter->flags()
	 IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::BASES);
	 IO::ConstIoProxy<Face> baseclass(this);
	 lwriter->writeBaseClass("Face", &baseclass);
  }
}

void CloneFace::legacyDesrlz(IO::Reader* reader) {
DOTRACE("CloneFace::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->readTypename(ioTag);

	 istream& is = lreader->input();

	 for (int i = 0; i < 24; ++i) {
		is >> itsCtrlPnts[i];
	 }
	 is >> itsEyeAspect;
	 is >> itsVertOffset;

	 // Bug in aCC requires this rather than a simple throw; see face.cc
	 // for further explanation.
	 try {
		lreader->throwIfError(ioTag);
	 }
	 catch (IO::IoError&) {
		throw;
	 }

	 // Always legacyDesrlz Face, regardless of lreader->flags()
	 IO::LRFlagJanitor(*lreader, lreader->flags() | IO::BASES);
	 IO::IoProxy<Face> baseclass(this);
	 lreader->readBaseClass("Face", &baseclass);
  }
}

void CloneFace::readFrom(IO::Reader* reader) {
DOTRACE("CloneFace::readFrom");

  IO::ReadUtils::template readValueSeq<double>(reader, "ctrlPnts", itsCtrlPnts);
  reader->readValue("eyeAspect", itsEyeAspect);
  reader->readValue("vertOffset", itsVertOffset);

  IO::IoProxy<Face> baseclass(this);
  reader->readBaseClass("Face", &baseclass);
}

void CloneFace::writeTo(IO::Writer* writer) const {
DOTRACE("CloneFace::writeTo");

  IO::WriteUtils::writeValueSeq(writer, "ctrlPnts", itsCtrlPnts, itsCtrlPnts+24);
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
