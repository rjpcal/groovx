///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Sat Dec  4 03:54:51 1999
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
class ObjId;
class PosId;

///////////////////////////////////////////////////////////////////////
//
// Tlist class definition
//
///////////////////////////////////////////////////////////////////////

class Tlist : public PtrList<Trial>, public virtual IO {
  //////////////
  // creators //
  //////////////

protected:
  Tlist();

private:
  static Tlist theInstance;

public:

  // Returns a reference to the singleton instance of Tlist
  static Tlist& theTlist();

  virtual ~Tlist();

  // These I/O functions write/read the entire state of the Tlist
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);

  virtual int charCount() const;
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
