///////////////////////////////////////////////////////////////////////
// trial.h
// Rob Peters
// created: Mar-99
// written: Tue Mar 16 19:20:40 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#include <vector>

#ifndef IO_H_INCLUDED
#include "io.h"
#endif

class ObjList;
class PosList;
class istrstream;

///////////////////////////////////////////////////////////////////////
// Trial class declaration
///////////////////////////////////////////////////////////////////////

class Trial : public virtual IO {
private:
  typedef pair<int, int> IdPair;

public:
  Trial (const ObjList& olist, const PosList &plist) : 
    itsObjList(olist), itsPosList(plist), 
    itsIdPairs(), itsResponses(), itsType(-1) {}
  Trial(istream &is, IOFlag flag, const ObjList& olist, const PosList &plist); 
  ~Trial () {}

  // write/read the object's state from/to an output/input stream
  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);
  
  // this function reads from an istream that is assumed to contain
  // objid's only; posid's are implied by the position in the input
  // NOTE: this function reads to the end of the input stream since it
  // has no way to know when the input would be done. Thus it should
  // be passed an istrstream containing only the line of interest. If
  // offset is non-zero, it will be added to each incoming objid
  // before the objid is inserted into the Trial. The function returns
  // the number of objid's read from the stream.
  int readFromObjidsOnly(istrstream &ist, int offset = 0);

  typedef vector<IdPair> ObjGrp;

  const ObjGrp& objs() { return itsIdPairs; }

  void add(int objid, int posid) { itsIdPairs.push_back(IdPair(objid, posid)); }

  virtual int trialType() const { return itsType; } 
  // returns some info about relationship between objects in trial

  void action() const;

  const char* description() const;

  void recordResponse(int resp) { itsResponses.push_back(resp); }
  int lastResponse() const { return itsResponses.back(); }

private:
  const ObjList& itsObjList;
  const PosList& itsPosList;
  ObjGrp itsIdPairs;
  vector<int> itsResponses;
  int itsType;
};

///////////////////////////////////////////////////////////////////////
// declarations of simple derivations from Trial
///////////////////////////////////////////////////////////////////////

class SingleTrial : public Trial {
};

class PairTrial : public Trial {
};

class TripleTrial : public Trial {
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
