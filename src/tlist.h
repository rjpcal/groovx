///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Wed Feb 16 08:02:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_H_DEFINED
#define TLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

class Trial;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tlist is a singleton wrapper for \c PtrList<Trial>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tlist : public PtrList<Trial> {
private:
  static Tlist theInstance;

protected:
  /// Construct and reserve space for \a size objects.
  Tlist(int size);

public:

  // Returns a reference to the singleton instance.
  static Tlist& theTlist();

  /// Overridden from PtrList<Trial> to provide compatibility with old Tlist.
  virtual void serialize(ostream &os, IOFlag flag) const;

  /// Overridden from PtrList<Trial> to provide compatibility with old Tlist.
  virtual void deserialize(istream &is, IOFlag flag);

  /// Overridden from PtrList<Trial> to provide compatibility with old Tlist.
  virtual int charCount() const;
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
