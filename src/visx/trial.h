///////////////////////////////////////////////////////////////////////
//
// trial.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mar-99
// written: Sun Jul 22 07:33:00 2001
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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ATTRIBS_H_DEFINED)
#include "io/attribs.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRIALBASE_H_DEFINED)
#include "trialbase.h"
#endif

namespace Util
{
  template <class T> class Ref;
};

class ResponseHandler;
class TimingHdlr;

///////////////////////////////////////////////////////////////////////
//
// Trial class definition
//
///////////////////////////////////////////////////////////////////////

class Trial : public TrialBase, public FieldContainer {
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

  static const FieldMap& classFields();

  ////////////////////////////
  // accessors+manipulators //
  ////////////////////////////

  virtual Util::WeakRef<GWT::Widget> getWidget() const;

  int getCorrectResponse() const;
  void setCorrectResponse(int response);

  Util::Ref<ResponseHandler> getResponseHandler() const;
  void setResponseHandler(Util::Ref<ResponseHandler> rh);

  Util::Ref<TimingHdlr> getTimingHdlr() const;
  void setTimingHdlr(Util::Ref<TimingHdlr> th);

  // returns some info about relationship between objects in trial
  virtual int trialType() const;
  void setType(int t);

  ReadWriteAttrib<Trial, int> tType;

  virtual const char* description() const;

  virtual int lastResponse() const;

  virtual void undoLastResponse();

  unsigned int numResponses() const;

  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void add(Util::UID objid, Util::UID posid);

  void addNode(Util::UID id);

  void trNextNode();

  unsigned int getCurrentNode() const;
  void setCurrentNode(unsigned int nodeNumber);

  void clearObjs();


  /////////////
  // actions //
  /////////////

  virtual void trDoTrial(Util::WeakRef<GWT::Widget> widget,
                         Util::ErrorHandler& errhdlr, Block& block);

  virtual int trElapsedMsec();

  virtual void trAbortTrial();
  virtual void trEndTrial();
  virtual void trNextTrial();
  virtual void trHaltExpt();
  virtual void trResponseSeen();
  virtual void trRecordResponse(Response& response);
  virtual void trAllowResponses();
  virtual void trDenyResponses();

  virtual void installSelf(Util::WeakRef<GWT::Widget> widget) const;

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
