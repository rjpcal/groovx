///////////////////////////////////////////////////////////////////////
//
// tlist.cc
// Rob Peters
// created: Fri Mar 12 14:39:39 1999
// written: Sat Mar  4 16:25:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_CC_DEFINED
#define TLIST_CC_DEFINED

#include "tlist.h"

#include <iostream.h>
#include <cstring>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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
  PtrList<Trial>(size)
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

void Tlist::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Tlist::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // Always serialize the PtrList base
  if (true || flag & BASES) { 
	 PtrList<Trial>::serialize(os, flag | TYPENAME);
  }

  // Here we are spoofing the obselete data members itsCurTrial and
  // itsVisibility.
  os << int(0) << sep << bool(false) << sep;

  if (os.fail()) throw OutputError(ioTag);
}

//---------------------------------------------------------------------
//
// Tlist::deserialize
//
//---------------------------------------------------------------------

void Tlist::deserialize(istream &is, IOFlag flag) {
DOTRACE("Tlist::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  // Always deserialize its PtrList<Trial> base
  if (true || flag & BASES) {
	 PtrList<Trial>::deserialize(is, flag | TYPENAME);
  }

  // Here we are spoofing the obselete data members itsCurTrial and
  // itsVisibility.
  int dummy1, dummy2;
  is >> dummy1 >> dummy2;

  if (is.fail()) throw InputError(ioTag);
}

int Tlist::charCount() const {
  return (strlen(ioTag) + 1
			 + gCharCount<int>(0) + 1
			 + gCharCount<bool>(false) + 1
			 + PtrList<Trial>::charCount()
			 + 5 ); // fudge factor 5
}

///////////////////////////////////////////////////////////////////////
//
// Template instantiation of PtrList<Trial>
//
///////////////////////////////////////////////////////////////////////

#include "trial.h"
#include "ptrlist.cc"
template class PtrList<Trial>;

static const char vcid_tlist_cc[] = "$Header$";
#endif // !TLIST_CC_DEFINED
