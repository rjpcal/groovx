///////////////////////////////////////////////////////////////////////
//
// trial.h
// Rob Peters
// created: Mar-99
// written: Thu Mar  9 16:09:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef VALUE_H_DEFINED
#include "value.h"
#endif

class Canvas;

///////////////////////////////////////////////////////////////////////
//
// TrialError class definition
//
///////////////////////////////////////////////////////////////////////

class TrialError : public ErrorWithMsg {
public:
  TrialError(const char* msg) : ErrorWithMsg(msg) {}
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
  class Response : public Value {
  public:
	 Response(int v = -1, int m = -1) : itsVal(v), itsMsec(m) {}

	 virtual Value* clone() const;
	 virtual Type getNativeType() const;
	 virtual const char* getNativeTypeName() const;

	 virtual void printTo(ostream& os) const;
	 virtual void scanFrom(istream& is);

	 int& val() { return itsVal; }
	 int& msec() { return itsMsec; }
	 int val() const { return itsVal; }
	 int msec() const { return itsMsec; }
  private:
	 int itsVal;
	 int itsMsec;
  };

  struct IdPair : public Value {
	 IdPair(int o = 0, int p = 0) : objid(o), posid(p) {}

	 virtual Value* clone() const;
	 virtual Type getNativeType() const;
	 virtual const char* getNativeTypeName() const;

	 virtual void printTo(ostream& os) const;
	 virtual void scanFrom(istream& is);

	 int objid;
	 int posid;
  };

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
  
  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

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

  typedef const IdPair* IdPairItr;
  IdPairItr beginIdPairs() const;
  IdPairItr endIdPairs() const;

  // returns some info about relationship between objects in trial
  int trialType() const;

  const char* description() const;

  int lastResponse() const;

  int numResponses() const;

  double avgResponse() const;
  double avgRespTime() const;

  //////////////////
  // manipulators //
  //////////////////

  void add(int objid, int posid);

  void clearObjs();

  void setType(int t);

  void setResponseHandler(int rhid);
  void setTimingHdlr(int thid);

  void recordResponse(int val, int msec);

  void undoLastResponse();

  void clearResponses();

  /////////////
  // actions //
  /////////////

  void trDraw(Canvas& canvas, bool flush) const;
  void trUndraw(Canvas& canvas, bool flush) const;

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
