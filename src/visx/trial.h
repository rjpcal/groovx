///////////////////////////////////////////////////////////////////////
//
// trial.h
// Rob Peters
// created: Mar-99
// written: Mon Sep 27 10:32:10 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef ID_H_DEFINED
#include "id.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// TrialError class definition
//
///////////////////////////////////////////////////////////////////////

class TrialError : public ErrorWithMsg {
public:
  TrialError(const string& msg="") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// Trial class declaration
//
///////////////////////////////////////////////////////////////////////

class Trial : public virtual IO {
public:
  //////////////////
  // nested types //
  //////////////////
  class Response {
  public:
	 Response(int v = -1, int m = -1) : itsVal(v), itsMsec(m) {}
	 int& val() { return itsVal; }
	 int& msec() { return itsMsec; }
	 int val() const { return itsVal; }
	 int msec() const { return itsMsec; }
  private:
	 int itsVal;
	 int itsMsec;
  };

  struct IdPair {
	 IdPair(ObjId o, PosId p) : objid(o), posid(p) {}
	 ObjId objid;
	 PosId posid;
  };
  typedef vector<IdPair> ObjGrp;

  //////////////
  // creators //
  //////////////

  Trial();
  Trial(istream &is, IOFlag flag); 
  virtual ~Trial ();

  // write/read the object's state from/to an output/input stream
  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;
  
  // this function reads from an istream that is assumed to contain
  // objid's only; posid's are implied by the position in the input
  // NOTE: this function reads to the end of the input stream since it
  // has no way to know when the input would be done. Thus it should
  // be passed an istream containing only the line of interest. If
  // offset is non-zero, it will be added to each incoming objid
  // before the objid is inserted into the Trial. The function returns
  // the number of objid's read from the stream.
  int readFromObjidsOnly(istream &is, int offset = 0);

  ///////////////
  // accessors //
  ///////////////

  int getResponseHandler() const;
  int getTimingHdlr() const;

  const ObjGrp& objs() const { return itsIdPairs; }

  // returns some info about relationship between objects in trial
  int trialType() const { return itsType; } 

  const char* description() const;

  int lastResponse() const { return itsResponses.back().val(); }

  int numResponses() const { return itsResponses.size(); }

  double avgResponse() const;
  double avgRespTime() const;

  //////////////////
  // manipulators //
  //////////////////

  void add(ObjId objid, PosId posid) {
	 itsIdPairs.push_back(IdPair(objid, posid));
  }

  void clearObjs();

  void setType(int t) { itsType = t; }

  void setResponseHandler(int rhid);
  void setTimingHdlr(int thid);

  void recordResponse(int val, int msec) { 
	 itsResponses.push_back(Response(val, msec));
  }

  void undoLastResponse();

  void clearResponses();

  /////////////
  // actions //
  /////////////

  void trDraw() const;
  void trUndraw() const;

private:
  ObjGrp itsIdPairs;
  vector<Response> itsResponses;
  int itsType;
  int itsRhId;
  int itsThId;

  mutable bool isItRendered;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
