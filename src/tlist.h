///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Sat Dec  4 02:00:12 1999
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
private:
  int itsCurTrial;
  bool itsVisibility;

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

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // Reads a list of 'simple' trial descriptions which contain objid's
  // only; posid's are inferred from position int the list. If
  // num_trials is passed as < 0, the function will read to the end of
  // the istream. If offset is non-zero, it will be added to each
  // incoming objid before it is inserted into the Trial. Returns the
  // number of trials that were loaded.
  int readFromObjidsOnly(istream &is, int num_trials, int offset = 0);

  //////////////////
  // manipulators //
  //////////////////

  void clear();

  /////////////
  // actions //
  /////////////

  void drawTrial(int trial);
  void undrawTrial(int trial);
  void drawCurTrial() const;
  void undrawCurTrial() const;
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
