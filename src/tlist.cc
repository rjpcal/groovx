///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Wed Sep 27 11:23:52 2000
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
	 ostream& os = lwriter->output();
	 // Always legacySrlz the PtrList base
	 PtrList<TrialBase>::legacySrlz(writer);

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 os << int(0) << IO::SEP << bool(false) << IO::SEP;

	 if (os.fail()) throw IO::OutputError(ioTag);
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
	 istream& is = lreader->input();
	 // Always legacyDesrlz its PtrList<TrialBase> base
	 PtrList<TrialBase>::legacyDesrlz(reader);

	 // Here we are spoofing the obselete data members itsCurTrial and
	 // itsVisibility.
	 int dummy1, dummy2;
	 is >> dummy1 >> dummy2;

	 if (is.fail()) throw IO::InputError(ioTag);
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
