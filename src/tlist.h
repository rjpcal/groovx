///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Sat Sep 23 15:32:23 2000
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
  /// Construct and reserve space for \a size objects.
  Tlist(int size);

  /// Virtual destructor.
  virtual ~Tlist();

public:

  // Returns a reference to the singleton instance.
  static Tlist& theTlist();

  /// Overridden from PtrList<TrialBase> to provide compatibility with old Tlist.
  virtual void serialize(STD_IO::ostream &os, IO::IOFlag flag) const;

  /// Overridden from PtrList<TrialBase> to provide compatibility with old Tlist.
  virtual void deserialize(STD_IO::istream &is, IO::IOFlag flag);

  /// Overridden from PtrList<TrialBase> to provide compatibility with old Tlist.
  virtual int charCount() const;
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
