///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Sun Oct  8 16:00:00 2000
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

void Tlist::writeTo(IO::Writer* writer) const {
DOTRACE("Tlist::writeTo");
  // Always legacySrlz the PtrList base
  PtrList<TrialBase>::writeTo(writer);

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 lwriter->resetFieldSeparator();

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

void Tlist::readFrom(IO::Reader* reader) {
DOTRACE("Tlist::readFrom");
  // Always legacyDesrlz its PtrList<TrialBase> base
  PtrList<TrialBase>::readFrom(reader);

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {

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
template class MasterPtr<TrialBase>;
template class PtrList<TrialBase>;

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
