///////////////////////////////////////////////////////////////////////
//
// trial.h
// Rob Peters
// created: Mar-99
// written: Thu Nov  2 15:44:18 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRIALBASE_H_DEFINED)
#include "trialbase.h"
#endif

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

template <class T> class IdItem;

namespace GWT {
  class Canvas;
  class Widget;
}

namespace Util { class ErrorHandler; }

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

  /** This tracer dynamically controls the tracing of Trial member
		functions. */
  static Util::Tracer tracer;

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

  void writeMatlab(STD_IO::ostream& os) const;

  ///////////////
  // accessors //
  ///////////////

  int getCorrectResponse() const;

  int getResponseHandler() const;
  int getTimingHdlr() const;

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
