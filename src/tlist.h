///////////////////////////////////////////////////////////////////////
// tlist.h
// Rob Peters
// created: Fri Mar 12 13:23:02 1999
// written: Tue Mar 16 19:23:58 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TLIST_H_DEFINED
#define TLIST_H_DEFINED

#include <vector>

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class Trial;
class ObjList;
class PosList;

class Tlist : public virtual IO {
private:
  const ObjList& itsObjList;
  const PosList& itsPosList;
  vector<Trial *> itsTrials;
  int itsCurTrial;
  bool itsVisibility;
public:
  //////////////
  // creators //
  //////////////

  Tlist (ObjList &olist, PosList &plist) : 
    itsObjList(olist), itsPosList(plist), 
    itsTrials(1, NULL), itsCurTrial(0), itsVisibility(false) {}
  ~Tlist();

  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);
  // These I/O functions write/read the _entire_ state of the Tlist,
  // including itsObjList and itsPosList, which are held only by reference.

  IOResult readFromObjidsOnly(istream &is, int num_trials, int offset = 0);
  // Reads a list of 'simple' trial descriptions which contain objid's
  // only; posid's are inferred from position int the list. If
  // num_trials is passed as < 0, the function will read to the end of
  // the istream. If offset is non-zero, it will be added to each
  // incoming objid before it is inserted into the Trial.

  ///////////////
  // accessors //
  ///////////////

  bool isValidTrial(int trial) const;
  Trial* getTrial(int trial) const;

  void getValidTrials(vector<int>& vec) const;
  // Puts a list of all valid (i.e. within-range and non-null) trial
  // ids into the vector<int> that is passed in by reference

  //////////////////
  // manipulators //
  //////////////////

  void setVisible(bool vis);
  void setCurTrial(int trial);
  void addToTrial(int trial, int objid, int posid);
  void clearTrial(int trial);
  void clearAllTrials();

  /////////////
  // actions //
  /////////////

  void drawTrial(int trial);
  void drawCurTrial() const;
};

static const char vcid_tlist_h[] = "$Header$";
#endif // !TLIST_H_DEFINED
