///////////////////////////////////////////////////////////////////////
//
// trial.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  1 08:00:00 1999
// written: Thu Dec 19 18:27:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_H_DEFINED
#define TRIAL_H_DEFINED

#include "io/fields.h"

#include "util/tracer.h"

#include "visx/element.h"

namespace Util
{
  template <class T> class FwdIter;
  template <class T> class Ref;
};

class GxNode;
class Response;
class ResponseHandler;
class TimingHdlr;

/// Trial collaborates with ResponseHandler+TimingHdlr.
class Trial : public Element, public FieldContainer
{
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

  int getCorrectResponse() const;
  void setCorrectResponse(int response);

  Util::Ref<ResponseHandler> getResponseHandler() const;
  void setResponseHandler(Util::Ref<ResponseHandler> rh);

  Util::Ref<TimingHdlr> getTimingHdlr() const;
  void setTimingHdlr(Util::Ref<TimingHdlr> th);

  void setType(int t);

  unsigned int numResponses() const;

  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void addNode(Util::Ref<GxNode> item);

  void trNextNode();

  /// Returns an iterator to all the nodes contained in the Trial.
  Util::FwdIter<Util::Ref<GxNode> > nodes() const;

  unsigned int getCurrentNode() const;
  void setCurrentNode(unsigned int nodeNumber);

  void clearObjs();


  //
  // Element interface
  //

  virtual const Util::SoftRef<Toglet>& getWidget() const;

  /// returns some user-defined info about relationship between objects in trial
  virtual int trialType() const;

  virtual int lastResponse() const;

  /// Overridden from Element.
  virtual fstring status() const;

  virtual void vxRun(Element& parent);

  virtual void vxHalt() const;

  virtual void vxReturn(ChildStatus s);

  virtual void vxUndo();

  virtual void vxReset();

  /////////////
  // actions //
  /////////////

  double trElapsedMsec();

  void trResponseSeen();
  void trProcessResponse(Response& response);
  void trAllowResponses();
  void trDenyResponses();
  void trAbort();
  void trEndTrial();

  void installSelf(Util::SoftRef<Toglet> widget) const;

private:
  Trial(const Trial&);
  Trial& operator=(const Trial&);

  class Impl;
  Impl* const rep;
};

static const char vcid_trial_h[] = "$Id$";
#endif // !TRIAL_H_DEFINED
