///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Wed Oct 25 16:53:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_H_DEFINED
#define TLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
#include "ptrlist.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class TrialBase;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tlist is a singleton wrapper for \c PtrList<Trial>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tlist : public PtrList<TrialBase> {
private:
  static Tlist theInstance;

protected:
  /// Default constructor makes an empty list.
  Tlist();

  /// Virtual destructor.
  virtual ~Tlist();

public:
  // Returns a reference to the singleton instance.
  static Tlist& theTlist();
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
