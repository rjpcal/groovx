///////////////////////////////////////////////////////////////////////
// trial.h
// Rob Peters
// created: Mar-99
// written: Sun Apr 25 12:50:01 1999
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
public:
  //////////////
  // typedefs //
  //////////////
  typedef pair<int, int> IdPair;
  typedef vector<IdPair> ObjGrp;

  /////////////
  // structs //
  /////////////
  struct Response {
	 Response(int v = -1, int m = -1) : val(v), msec(m) {}
	 int val;
	 int msec;
  };

  //////////////
  // creators //
  //////////////

  Trial (const ObjList& olist, const PosList &plist) : 
    itsObjList(olist), itsPosList(plist), 
    itsIdPairs(), itsResponses(), itsType(-1) {}
  Trial(istream &is, IOFlag flag, const ObjList& olist, const PosList &plist); 
  virtual ~Trial () {}

  // write/read the object's state from/to an output/input stream
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  
  // this function reads from an istream that is assumed to contain
  // objid's only; posid's are implied by the position in the input
  // NOTE: this function reads to the end of the input stream since it
  // has no way to know when the input would be done. Thus it should
  // be passed an istrstream containing only the line of interest. If
  // offset is non-zero, it will be added to each incoming objid
  // before the objid is inserted into the Trial. The function returns
  // the number of objid's read from the stream.
  int readFromObjidsOnly(istream &is, int offset = 0);

  ///////////////
  // accessors //
  ///////////////

  const ObjGrp& objs() const { return itsIdPairs; }

  // returns some info about relationship between objects in trial
  int trialType() const { return itsType; } 

  const char* description() const;

  int lastResponse() const { return itsResponses.back().val; }

  int numResponses() const { return itsResponses.size(); }

  double avgResponse() const;
  double avgRespTime() const;

  //////////////////
  // manipulators //
  //////////////////

  void add(int objid, int posid) { itsIdPairs.push_back(IdPair(objid, posid)); }

  void setType(int t) { itsType = t; }

  void recordResponse(int val, int msec) { 
	 itsResponses.push_back(Response(val, msec));
  }

  /////////////
  // actions //
  /////////////

  void action() const;

private:
  const ObjList& itsObjList;
  const PosList& itsPosList;
  ObjGrp itsIdPairs;
  vector<Response> itsResponses;
  int itsType;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
