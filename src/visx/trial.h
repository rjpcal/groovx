///////////////////////////////////////////////////////////////////////
//
// trial.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mar-99
// written: Wed May 23 18:48:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
#include "util/tracer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
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

  void writeMatlab(STD_IO::ostream& os) const;

  ////////////////////////////
  // accessors+manipulators //
  ////////////////////////////

  int getCorrectResponse() const;
  void setCorrectResponse(int response);

  int getResponseHandler() const;
  void setResponseHandler(int rhid);

  int getTimingHdlr() const;
  void setTimingHdlr(int thid);

  // returns some info about relationship between objects in trial
  virtual int trialType() const;
  void setType(int t);

  virtual const char* description() const;

  virtual int lastResponse() const;

  virtual void undoLastResponse();

  int numResponses() const;

  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void add(int objid, int posid);

  void addNode(IO::UID id);

  void trNextNode();

  int getCurrentNode() const;
  void setCurrentNode(int nodeNumber);

  void clearObjs();


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

  virtual void installSelf(GWT::Widget& widget) const;

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
