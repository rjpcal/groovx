///////////////////////////////////////////////////////////////////////
//
// trial.h
// Rob Peters
// created: Mar-99
// written: Mon Oct 23 16:36:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRIALBASE_H_DEFINED)
#include "trialbase.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ITEMWITHID_H_DEFINED)
#include "itemwithid.h"
#endif

namespace GWT {
  class Canvas;
  class Widget;
}

namespace Util { class ErrorHandler; }

class GrObj;
class Position;
class Block;
class Response;

///////////////////////////////////////////////////////////////////////
//
// Trial class definition
//
///////////////////////////////////////////////////////////////////////

class Trial : public TrialBase {
public:

  //////////////
  // creators //
  //////////////

protected:
  Trial();
public:
  static Trial* make();

  virtual ~Trial ();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  // this function reads from an STD_IO::istream that is assumed to contain
  // objid's only; posid's are implied by the position in the input
  // NOTE: this function reads to the end of the input stream since it
  // has no way to know when the input would be done. Thus it should
  // be passed an STD_IO::istream containing only the line of interest. If
  // offset is non-zero, it will be added to each incoming objid
  // before the objid is inserted into the Trial. The function returns
  // the number of objid's read from the stream.
  int readFromObjidsOnly(STD_IO::istream &is, int offset = 0);

  ///////////////
  // accessors //
  ///////////////

  int getCorrectResponse() const;

  int getResponseHandler() const;
  int getTimingHdlr() const;

  typedef const NullableItemWithId<GrObj>* GrObjItr;
  GrObjItr beginGrObjs() const;
  GrObjItr endGrObjs() const;

  // returns some info about relationship between objects in trial
  virtual int trialType() const;

  virtual const char* description() const;

  virtual int lastResponse() const;

  int numResponses() const;

  double avgResponse() const;
  double avgRespTime() const;

  //////////////////
  // manipulators //
  //////////////////

  void setCorrectResponse(int response);

  void add(int objid, int posid);

  void clearObjs();

  void setType(int t);

  void setResponseHandler(int rhid);
  void setTimingHdlr(int thid);

  virtual void undoLastResponse();

  void clearResponses();

  /////////////
  // actions //
  /////////////

  virtual void trDoTrial(GWT::Widget& widget,
								 Util::ErrorHandler& errhdlr, Block& block);

  virtual int trElapsedMsec();

  virtual void trAbortTrial();
  virtual void trEndTrial();
  virtual void trNextTrial();
  virtual void trHaltExpt();
  virtual void trResponseSeen();
  virtual void trRecordResponse(Response& response);
  virtual void trDrawTrial() const;
  virtual void trUndrawTrial() const;

  virtual void trDraw(GWT::Canvas& canvas, bool flush) const;
  virtual void trUndraw(GWT::Canvas& canvas, bool flush) const;

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
