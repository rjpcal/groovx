///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov  9 15:32:48 1999
// written: Sat Jul 21 20:09:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "eventresponsehdlr.h"

#include "experiment.h"
#include "feedbackmap.h"
#include "grshapp.h"
#include "sound.h"
#include "response.h"
#include "responsemap.h"
#include "trialbase.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/widget.h"

#include "tcl/tclcmd.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#define DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const string_literal ioTag("EventResponseHdlr");

  const string_literal nullScript("{}");
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(EventResponseHdlr* owner, const char* input_response_map);
  ~Impl();

  class ERHState;
  friend class ERHState;
  class ERHActiveState;
  friend class ERHActiveState;


  class ERHState {
  protected:
    ERHState() {}

  public:
    virtual ~ERHState() {}

    // all virtual functions have default empty bodies, so this class
    // functions as the inactive state

    virtual void rhAbortTrial() {}
    virtual void rhEndTrial() {}
    virtual void rhHaltExpt() {}

    virtual void handleResponse(const char* /*keysym*/) {}

    virtual void onDestroy() {}

    static shared_ptr<ERHState> activeState(const EventResponseHdlr::Impl* erh,
                                            Util::WeakRef<GWT::Widget> widget,
                                            TrialBase& trial);
    static shared_ptr<ERHState> inactiveState();
  };


  class ERHActiveState : public ERHState {
  private:
    const EventResponseHdlr::Impl& itsErh;
    Util::WeakRef<GWT::Widget> itsWidget;
    TrialBase& itsTrial;

  public:
    virtual ~ERHActiveState()
    { itsErh.ignore(itsWidget); }

    ERHActiveState(const EventResponseHdlr::Impl* erh,
                   Util::WeakRef<GWT::Widget> widget, TrialBase& trial) :
      itsErh(*erh),
      itsWidget(widget),
      itsTrial(trial)
    {
      Assert((&itsErh != 0) && (&itsWidget != 0) && (&itsTrial != 0));
      itsErh.attend(itsWidget);
    }

    virtual void rhAbortTrial()
    {
      itsErh.ignore(itsWidget);

      Ref<Sound> p = Sound::getErrSound();
      p->play();
    }

    virtual void rhEndTrial()
    {
      itsErh.ignore(itsWidget);
      itsErh.changeState(ERHState::inactiveState());
    }

    virtual void rhHaltExpt()
    {
      itsErh.ignore(itsWidget);
      itsErh.changeState(ERHState::inactiveState());
    }

    virtual void handleResponse(const char* keysym);

    virtual void onDestroy()
    {
      itsErh.ignore(itsWidget);
    }
  }; // end class ERHActiveState


  void changeState(shared_ptr<ERHState> new_state) const
  { itsState = new_state; }


  // Delegand functions

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void rhBeginTrial(Util::WeakRef<GWT::Widget> widget, TrialBase& trial) const
    {
      itsSafeIntp.clearEventQueue();

      itsState = ERHState::activeState(this, widget, trial);
    }

  void rhAbortTrial() const { itsState->rhAbortTrial(); }

  void rhEndTrial() const { itsState->rhEndTrial(); }

  void rhHaltExpt() const { itsState->rhHaltExpt(); }

  void rhAllowResponses(Util::WeakRef<GWT::Widget> widget,
                        TrialBase& trial) const
    {
      itsState = ERHState::activeState(this, widget, trial);
    }

  void rhDenyResponses() const
    { itsState = ERHState::inactiveState(); }

  // Helper functions
private:

  dynamic_string getBindingScript() const
  {
    return dynamic_string("{ ")
      .append(itsCmdCallback->name()).append(" ")
      .append(itsBindingSubstitution).append(" }");
  }

  void attend(Util::WeakRef<GWT::Widget> widget) const
  {
    DOTRACE("EventResponseHdlr::Impl::attend");
	 if (widget.isValid())
		widget->bind(itsEventSequence.c_str(), getBindingScript().c_str());
  }

  void ignore(Util::WeakRef<GWT::Widget> widget) const
  {
    DOTRACE("EventResponseHdlr::Impl::ignore");
	 if (widget.isValid())
		widget->bind(itsEventSequence.c_str(), nullScript.c_str());
  }

  class PrivateHandleCmd;

  //
  // data
  //

  EventResponseHdlr* itsOwner;

  mutable shared_ptr<ERHState> itsState;

public:
  Tcl::SafeInterp itsSafeIntp;

  Tcl::TclCmd* itsCmdCallback;

  ResponseMap itsResponseMap;

  FeedbackMap itsFeedbackMap;

  fixed_string itsEventSequence;
  fixed_string itsBindingSubstitution;

  bool itsAbortInvalidResponses;
};

///////////////////////////////////////////////////////////////////////
//
// Static member definitions
//
///////////////////////////////////////////////////////////////////////

Util::Tracer EventResponseHdlr::tracer;


//--------------------------------------------------------------------
//
// EventResponseHdlr::privateHandleCmd --
//
// This Tcl command gives a hook back into the C++ code from Tcl from
// the event binding that is set up in attend().
//
//--------------------------------------------------------------------

class EventResponseHdlr::Impl::PrivateHandleCmd : public Tcl::TclCmd {
public:
  PrivateHandleCmd(EventResponseHdlr::Impl* owner) :
    Tcl::TclCmd(dynamic_cast<GrshApp&>(Application::theApp()).getInterp(),
                getUniqueCmdName(), "<private>", 2),
    itsOwner(*owner)
  {}

protected:
  virtual void invoke(Tcl::Context& ctx)
  {
    itsOwner.itsState->handleResponse(ctx.getCstringFromArg(1));
  }

private:
  const char* getUniqueCmdName()
    {
      static dynamic_string baseName;
      static int cmdCounter = 0;

      baseName = "__EventResponseHdlrPrivate::handle";
      baseName.append(++cmdCounter);
      return baseName.c_str();
    }

  EventResponseHdlr::Impl& itsOwner;
};

///////////////////////////////////////////////////////////////////////
//
// ERHState member definitions
//
///////////////////////////////////////////////////////////////////////

shared_ptr<EventResponseHdlr::Impl::ERHState>
EventResponseHdlr::Impl::ERHState::activeState(
  const EventResponseHdlr::Impl* erh,
  Util::WeakRef<GWT::Widget> widget,
  TrialBase& trial
)
{
DOTRACE("EventResponseHdlr::Impl::ERHState::activeState");
  return shared_ptr<ERHState>(new ERHActiveState(erh, widget, trial));
}

shared_ptr<EventResponseHdlr::Impl::ERHState>
EventResponseHdlr::Impl::ERHState::inactiveState()
{
DOTRACE("EventResponseHdlr::Impl::ERHState::inactiveState");
  return shared_ptr<ERHState>(new ERHState);
}

void EventResponseHdlr::Impl::ERHActiveState::handleResponse(
  const char* keysym
) {
DOTRACE("EventResponseHdlr::Impl::ERHActiveState::handleResponse");

  Response theResponse;

  theResponse.setMsec(itsTrial.trElapsedMsec());

  itsTrial.trResponseSeen();

  itsErh.ignore(itsWidget);

  theResponse.setVal(itsErh.itsResponseMap.valueFor(keysym));
  DebugEvalNL(theResponse.val());

  if ( !theResponse.isValid() )
    {
      if ( itsErh.itsAbortInvalidResponses )
        itsTrial.trAbortTrial();
    }
  else
    {
      itsTrial.trRecordResponse(theResponse);
      itsErh.itsFeedbackMap.giveFeedback(itsErh.itsSafeIntp, theResponse.val());
    }
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner,
                              const char* input_response_map) :
  itsOwner(owner),
  itsState(ERHState::inactiveState()),
  itsSafeIntp(dynamic_cast<GrshApp&>(Application::theApp()).getInterp()),
  itsCmdCallback(new PrivateHandleCmd(this)),
  itsResponseMap(input_response_map),
  itsFeedbackMap(),
  itsEventSequence("<KeyPress>"),
  itsBindingSubstitution("%K"),
  itsAbortInvalidResponses(true)
{
DOTRACE("EventResponseHdlr::Impl::Impl");
}

EventResponseHdlr::Impl::~Impl()
{
DOTRACE("EventResponseHdlr::Impl::~Impl");
  delete itsCmdCallback;
}

void EventResponseHdlr::Impl::readFrom(IO::Reader* reader) {
DOTRACE("EventResponseHdlr::Impl::readFrom");

  itsState = ERHState::inactiveState();

  {
    fixed_string rep;
    reader->readValue("inputResponseMap", rep);
    itsResponseMap.set(rep);
  }

  {
    fixed_string rep;
    reader->readValue("feedbackMap", rep);
    itsFeedbackMap.set(rep);
  }

  reader->readValue("useFeedback", itsFeedbackMap.itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);
}

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::writeTo");

  writer->writeValue("inputResponseMap", itsResponseMap.rep());
  writer->writeValue("feedbackMap", itsFeedbackMap.rep());
  writer->writeValue("useFeedback", itsFeedbackMap.itsUseFeedback);
  writer->writeValue("eventSequence", itsEventSequence);
  writer->writeValue("bindingSubstitution", itsBindingSubstitution);
}


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr* EventResponseHdlr::make() {
DOTRACE("EventResponseHdlr::make");
  return new EventResponseHdlr;
}

EventResponseHdlr::EventResponseHdlr() :
  ResponseHandler(),
  itsImpl(new Impl(this, ""))
{}

EventResponseHdlr::EventResponseHdlr(const char* input_response_map) :
  ResponseHandler(),
  itsImpl(new Impl(this, input_response_map))
{}

EventResponseHdlr::~EventResponseHdlr()
  { delete itsImpl; }

void EventResponseHdlr::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void EventResponseHdlr::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }

const fixed_string& EventResponseHdlr::getInputResponseMap() const
{
DOTRACE("EventResponseHdlr::getInputResponseMap");
  return itsImpl->itsResponseMap.rep();
}

void EventResponseHdlr::setInputResponseMap(const fixed_string& s) {
DOTRACE("EventResponseHdlr::setInputResponseMap");
  itsImpl->itsResponseMap.set(s);
}

bool EventResponseHdlr::getUseFeedback() const {
DOTRACE("EventResponseHdlr::getUseFeedback");
  return itsImpl->itsFeedbackMap.itsUseFeedback;
}

void EventResponseHdlr::setUseFeedback(bool val) {
DOTRACE("EventResponseHdlr::setUseFeedback");
  itsImpl->itsFeedbackMap.itsUseFeedback = val;
}

const char* EventResponseHdlr::getFeedbackMap() const {
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return itsImpl->itsFeedbackMap.rep().c_str();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string) {
DOTRACE("EventResponseHdlr::setFeedbackMap");
  itsImpl->itsFeedbackMap.set(feedback_string);
}

const fixed_string& EventResponseHdlr::getEventSequence() const
  { return itsImpl->itsEventSequence; }

void EventResponseHdlr::setEventSequence(const fixed_string& seq)
  { itsImpl->itsEventSequence = seq; }

const fixed_string& EventResponseHdlr::getBindingSubstitution() const
  { return itsImpl->itsBindingSubstitution; }

void EventResponseHdlr::setBindingSubstitution(const fixed_string& sub)
  { itsImpl->itsBindingSubstitution = sub; }

void EventResponseHdlr::abortInvalidResponses()
  { itsImpl->itsAbortInvalidResponses = true; }

void EventResponseHdlr::ignoreInvalidResponses()
  { itsImpl->itsAbortInvalidResponses = false; }

void EventResponseHdlr::rhBeginTrial(Util::WeakRef<GWT::Widget> widget,
                                     TrialBase& trial) const
  { itsImpl->rhBeginTrial(widget, trial); }

void EventResponseHdlr::rhAbortTrial() const
  { itsImpl->rhAbortTrial(); }

void EventResponseHdlr::rhEndTrial() const
  { itsImpl->rhEndTrial(); }

void EventResponseHdlr::rhHaltExpt() const
  { itsImpl->rhHaltExpt(); }

void EventResponseHdlr::rhAllowResponses(Util::WeakRef<GWT::Widget> widget,
                                         TrialBase& trial) const
  { itsImpl->rhAllowResponses(widget, trial); }

void EventResponseHdlr::rhDenyResponses() const
  { itsImpl->rhDenyResponses(); }

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
