///////////////////////////////////////////////////////////////////////
//
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Wed Jun  9 15:08:09 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_H_DEFINED
#define TLIST_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
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

class Tlist : public virtual IO {
private:
  vector<Trial *> itsTrials;
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

  int readFromObjidsOnly(istream &is, int num_trials, int offset = 0);
  // Reads a list of 'simple' trial descriptions which contain objid's
  // only; posid's are inferred from position int the list. If
  // num_trials is passed as < 0, the function will read to the end of
  // the istream. If offset is non-zero, it will be added to each
  // incoming objid before it is inserted into the Trial. Returns the
  // number of trials that were loaded.

  ///////////////
  // accessors //
  ///////////////

  int trialCount() const;
  // Return the current number of valid trials

  bool isValidTrial(int trial) const;

  Trial& getTrial(int trial);
  // Precondition: trial > 0
  // This function will grow the Tlist if trial is beyond the current capacity,
  // and it will create a new Trial if the Trial* at trial is currently NULL.

  Trial& getTrial(int trial) const;
  // Precondition: trial is a valid trialid 
  //
  // This accessor for const Tlist's will not create a new Trial or
  // expand the Tlist

  void getValidTrials(vector<int>& vec) const;
  // Puts a list of all valid (i.e. within-range and non-null) trial
  // ids into the vector<int> that is passed in by reference

  //////////////////
  // manipulators //
  //////////////////

  void setVisible(bool vis);
  void setCurTrial(int trial);
  void addToTrial(int trial, ObjId objid, PosId posid);
  void clearTrial(int trial);
  void clearAllTrials();

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
