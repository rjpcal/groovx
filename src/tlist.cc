///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Wed Sep 27 18:11:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include "io/iolegacy.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations and helper functions
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char* ioTag = "Tlist";
}

///////////////////////////////////////////////////////////////////////
//
// Tlist member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Tlist::Tlist(int size) : 
  PtrList<TrialBase>(size)
{
DOTRACE("Tlist::Tlist");
}

Tlist::~Tlist() {}

Tlist Tlist::theInstance(100);

Tlist& Tlist::theTlist() {
DOTRACE("Tlist::theTlist");
  return theInstance;
}

//---------------------------------------------------------------------
//
// Tlist::legacySrlz
//
//---------------------------------------------------------------------

void Tlist::legacySrlz(IO::Writer* writer) const {
DOTRACE("Tlist::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 // Always legacySrlz the PtrList base
	 PtrList<TrialBase>::legacySrlz(writer);

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 writer->writeValue("dummy1", int(0));
	 writer->writeValue("dummy2", bool(false));
  }
}

//---------------------------------------------------------------------
//
// Tlist::legacyDesrlz
//
//---------------------------------------------------------------------

void Tlist::legacyDesrlz(IO::Reader* reader) {
DOTRACE("Tlist::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 // Always legacyDesrlz its PtrList<TrialBase> base
	 PtrList<TrialBase>::legacyDesrlz(reader);

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 int dummy1;
	 bool dummy2;
	 reader->readValue("dummy1", dummy1);
	 reader->readValue("dummy2", dummy2);
  }
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<TrialBase>
//
///////////////////////////////////////////////////////////////////////

#include "trialbase.h"
#include "ptrlist.cc"
template class PtrList<TrialBase>;

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
