///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Thu May 11 19:55:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include <iostream.h>
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

Tlist Tlist::theInstance(100);

Tlist& Tlist::theTlist() {
DOTRACE("Tlist::theTlist");
  return theInstance;
}

//---------------------------------------------------------------------
//
// Tlist::serialize
//
//---------------------------------------------------------------------

void Tlist::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("Tlist::serialize");
  // Always serialize the PtrList base
  if (true || flag & IO::BASES) { 
	 PtrList<TrialBase>::serialize(os, flag);
  }

  // Here we are spoofing the obselete data members itsCurTrial and
  // itsVisibility.
  os << int(0) << IO::SEP << bool(false) << IO::SEP;

  if (os.fail()) throw IO::OutputError(ioTag);
}

//---------------------------------------------------------------------
//
// Tlist::deserialize
//
//---------------------------------------------------------------------

void Tlist::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("Tlist::deserialize");
  // Always deserialize its PtrList<TrialBase> base
  if (true || flag & IO::BASES) {
	 PtrList<TrialBase>::deserialize(is, flag);
  }

  // Here we are spoofing the obselete data members itsCurTrial and
  // itsVisibility.
  int dummy1, dummy2;
  is >> dummy1 >> dummy2;

  if (is.fail()) throw IO::InputError(ioTag);
}

int Tlist::charCount() const {
  return (strlen(ioTag) + 1
			 + IO::gCharCount<int>(0) + 1
			 + IO::gCharCount<bool>(false) + 1
			 + PtrList<TrialBase>::charCount()
			 + 5 ); // fudge factor 5
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
