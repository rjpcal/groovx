///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov  9 15:32:48 1999
// written: Thu Jul 19 16:41:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "eventresponsehdlr.h"

#include "experiment.h"
#include "grshapp.h"
#include "sound.h"
#include "response.h"
#include "responsemap.h"
#include "trialbase.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/widget.h"

#include "tcl/tclcmd.h"
#include "tcl/tclcode.h"
#include "tcl/tclobjptr.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclregexp.h"
#include "tcl/tclutil.h"

#include "util/error.h"
#include "util/minivec.h"
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
  class ERHInactiveState;
  friend class ERHInactiveState;
  class ERHActiveState;
  friend class ERHActiveState;


  class ERHState {
  protected:
    ERHState() {}

  public:
    virtual ~ERHState() {}

    virtual void rhAbortTrial() = 0;
    virtual void rhEndTrial() = 0;
    virtual void rhHaltExpt() = 0;

    virtual void handleResponse(const char* keysym) = 0;

    virtual void onDestroy() = 0;

    static shared_ptr<ERHState> activeState(const EventResponseHdlr::Impl* erh,
                                            GWT::Widget& widget,
                                            TrialBase& trial);
    static shared_ptr<ERHState> inactiveState();
  };


  class ERHInactiveState : public ERHState {
  public:
    virtual ~ERHInactiveState() {}

    virtual void rhAbortTrial() {}
    virtual void rhEndTrial() {}
    virtual void rhHaltExpt() {}

    virtual void handleResponse(const char*) {}

    virtual void onDestroy() {};
  };


  class ERHActiveState : public ERHState {
  private:
    const EventResponseHdlr::Impl& itsErh;
    GWT::Widget& itsWidget;
    TrialBase& itsTrial;

    bool check()
    { return (&itsErh != 0) && (&itsWidget != 0) && (&itsTrial != 0); }

  public:
    virtual ~ERHActiveState()
    { itsErh.ignore(itsWidget); }

    ERHActiveState(const EventResponseHdlr::Impl* erh,
                   GWT::Widget& widget, TrialBase& trial) :
      itsErh(*erh),
      itsWidget(widget),
      itsTrial(trial)
    {
      Invariant(check());
      itsErh.attend(itsWidget);
    }

    virtual void rhAbortTrial()
    {
      Invariant(check());
      itsErh.ignore(itsWidget);

      Ref<Sound> p = Sound::getErrSound();
      p->play();
    }

    virtual void rhEndTrial()
    {
      Invariant(check());
      itsErh.ignore(itsWidget);
      itsErh.changeState(ERHState::inactiveState());
    }

    virtual void rhHaltExpt()
    {
      Invariant(check());
      itsErh.ignore(itsWidget);
      itsErh.changeState(ERHState::inactiveState());
    }

    virtual void handleResponse(const char* keysym);

    virtual void onDestroy()
    {
      Invariant(check());
      itsErh.ignore(itsWidget);
    }
  }; // end class ERHActiveState


  void changeState(shared_ptr<ERHState> new_state) const
  { itsState = new_state; }


  // Delegand functions

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void setFeedbackMap(const char* feedback_string)
    {
      itsFeedbackMapRep = feedback_string;
      itsFeedbacksAreDirty = true;
      updateFeedbacksIfNeeded();
    }

  void rhBeginTrial(GWT::Widget& widget, TrialBase& trial) const
    {
      itsSafeIntp.clearEventQueue();

      itsState = ERHState::activeState(this, widget, trial);
    }

  void rhAbortTrial() const { itsState->rhAbortTrial(); }

  void rhEndTrial() const { itsState->rhEndTrial(); }

  void rhHaltExpt() const { itsState->rhHaltExpt(); }

  void rhAllowResponses(GWT::Widget& widget, TrialBase& trial) const
    { itsState = ERHState::activeState(this, widget, trial); }

  void rhDenyResponses() const
    { itsState = ERHState::inactiveState(); }

  // Helper functions
private:

  // When this procedure is invoked, the program listens to the events
  // that are pertinent to the response policy. This procedure should
  // be called when a trial is begun, and should be cancelled with
  // ignore() when a response has been made so that events are not
  // received during response processing and during the inter-trial
  // period.
  void attend(GWT::Widget& widget) const;

  // When this procedure is invoked, the program ignores
  // user-generated events (mouse or keyboard) that would otherwise
  // signal a response. This effect is useful when the experiment is
  // in between trials, when a current response is already being
  // processed, and any other time when it is necessary to avoid an
  // unintended key/button-press being interpreted as a response. The
  // effect is cancelled by calling attend().
  void ignore(GWT::Widget& widget) const;

  class PrivateHandleCmd;

  void feedback(int response) const;
  void updateFeedbacksIfNeeded() const;
  void updateRegexpsIfNeeded() const;

  dynamic_string getBindingScript() const;

  // data
private:
  EventResponseHdlr* itsOwner;

  mutable shared_ptr<ERHState> itsState;

  Tcl::SafeInterp itsSafeIntp;

  PrivateHandleCmd* itsCmdCallback;

public:
  ResponseMap itsResponseMap;

private:

  class Condition_Feedback {
  public:
    Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
      itsCondition(cond),
      itsResultCmd(res, Tcl::Code::THROW_EXCEPTION)
      {
        Precondition(cond != 0); Precondition(res != 0);
      }

    bool invokeIfTrue(const Tcl::SafeInterp& safeInterp) throw (ErrorWithMsg)
      {
        if (isTrue(safeInterp))
          { invoke(safeInterp); return true; }
        return false;
      }

  private:
    bool isTrue(const Tcl::SafeInterp& safeInterp) throw(ErrorWithMsg)
      {
        return safeInterp.evalBooleanExpr(itsCondition);
      }

    void invoke(const Tcl::SafeInterp& safeInterp) throw(ErrorWithMsg)
      { itsResultCmd.invoke(safeInterp.intp()); }

    Tcl::ObjPtr itsCondition;
    Tcl::Code itsResultCmd;
  };

public:
  fixed_string itsFeedbackMapRep;
private:
  mutable minivec<Condition_Feedback> itsFeedbacks;

  mutable bool itsFeedbacksAreDirty;

public:
  bool itsUseFeedback;

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
  const EventResponseHdlr::Impl* erh, GWT::Widget& widget, TrialBase& trial
) {
DOTRACE("EventResponseHdlr::Impl::ERHState::activeState");
  return shared_ptr<ERHState>(new ERHActiveState(erh, widget, trial));
}

shared_ptr<EventResponseHdlr::Impl::ERHState>
EventResponseHdlr::Impl::ERHState::inactiveState()
{
DOTRACE("EventResponseHdlr::Impl::ERHState::inactiveState");
  return shared_ptr<ERHState>(new ERHInactiveState);
}

void EventResponseHdlr::Impl::ERHActiveState::handleResponse(
  const char* keysym
) {
DOTRACE("EventResponseHdlr::Impl::ERHActiveState::handleResponse");

  Invariant(check());

  Response theResponse;

  theResponse.setMsec(itsTrial.trElapsedMsec());

  itsTrial.trResponseSeen();

  itsErh.ignore(itsWidget);

  theResponse.setVal(itsErh.itsResponseMap.valueFor(keysym));
  DebugEvalNL(theResponse.val());

  if ( !theResponse.isValid() ) {
    if ( itsErh.itsAbortInvalidResponses )
      itsTrial.trAbortTrial();
  }

  else {
    itsTrial.trRecordResponse(theResponse);
    itsErh.feedback(theResponse.val());
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
  itsFeedbackMapRep(),
  itsFeedbacks(),
  itsFeedbacksAreDirty(true),
  itsUseFeedback(true),
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

  reader->readValue("feedbackMap", itsFeedbackMapRep);
  reader->readValue("useFeedback", itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);

  itsFeedbacksAreDirty = true;
  updateFeedbacksIfNeeded();
}

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::writeTo");

  writer->writeValue("inputResponseMap", itsResponseMap.rep());
  writer->writeValue("feedbackMap", itsFeedbackMapRep);
  writer->writeValue("useFeedback", itsUseFeedback);
  writer->writeValue("eventSequence", itsEventSequence);
  writer->writeValue("bindingSubstitution", itsBindingSubstitution);
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl helper method definitions
//
///////////////////////////////////////////////////////////////////////

dynamic_string EventResponseHdlr::Impl::getBindingScript() const
{
  return dynamic_string("{ ")
    .append(itsCmdCallback->name()).append(" ")
    .append(itsBindingSubstitution).append(" }");
}

void EventResponseHdlr::Impl::attend(GWT::Widget& widget) const {
DOTRACE("EventResponseHdlr::Impl::attend");

  widget.bind(itsEventSequence.c_str(), getBindingScript().c_str());
}

void EventResponseHdlr::Impl::ignore(GWT::Widget& widget) const {
DOTRACE("EventResponseHdlr::Impl::ignore");

  widget.bind(itsEventSequence.c_str(), nullScript.c_str());
}


void EventResponseHdlr::Impl::feedback(int response) const {
DOTRACE("EventResponseHdlr::Impl::feedback");

  if (!itsUseFeedback) return;

  updateFeedbacksIfNeeded();

  itsSafeIntp.setGlobalVar("resp_val", response);

  bool feedbackGiven = false;
  for (size_t i = 0; i<itsFeedbacks.size() && !feedbackGiven; ++i)
    {
      feedbackGiven = itsFeedbacks[i].invokeIfTrue(itsSafeIntp);
    }

  itsSafeIntp.unsetGlobalVar("resp_val");
}


//---------------------------------------------------------------------
//
// EventResponseHdlr::updateFeedacksIfNeeded --
//
//---------------------------------------------------------------------

void EventResponseHdlr::Impl::updateFeedbacksIfNeeded() const {
DOTRACE("EventResponseHdlr::Impl::updateFeedbacksIfNeeded");

  if (!itsFeedbacksAreDirty) return;

  Tcl::List pairs_list(Tcl::ObjPtr(itsFeedbackMapRep.c_str()));

  itsFeedbacks.clear();

  for (unsigned int i = 0; i < pairs_list.length(); ++i)
    {
      Tcl::List current_pair(pairs_list[i]);

      if (current_pair.length() != 2)
        {
          throw ErrorWithMsg("\"pair\" did not have length 2 "
                             "in EventResponseHdlr::updateFeedbacksIfNeeded");
        }

      itsFeedbacks.push_back(Impl::Condition_Feedback(current_pair[0],
                                                      current_pair[1]));
    }

  itsFeedbacksAreDirty = false;

  DebugPrintNL("updateFeedbacksIfNeeded success!");
}


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr method definitions
//
// All of these methods just delegate to EventResponseHdlr::Impl methods
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
  return itsImpl->itsUseFeedback;
}

void EventResponseHdlr::setUseFeedback(bool val) {
DOTRACE("EventResponseHdlr::setUseFeedback");
  itsImpl->itsUseFeedback = val;
}

const char* EventResponseHdlr::getFeedbackMap() const {
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return itsImpl->itsFeedbackMapRep.c_str();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string) {
DOTRACE("EventResponseHdlr::setFeedbackMap");
  itsImpl->setFeedbackMap(feedback_string);
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

void EventResponseHdlr::rhBeginTrial(GWT::Widget& widget, TrialBase& trial) const
  { itsImpl->rhBeginTrial(widget, trial); }

void EventResponseHdlr::rhAbortTrial() const
  { itsImpl->rhAbortTrial(); }

void EventResponseHdlr::rhEndTrial() const
  { itsImpl->rhEndTrial(); }

void EventResponseHdlr::rhHaltExpt() const
  { itsImpl->rhHaltExpt(); }

void EventResponseHdlr::rhAllowResponses(GWT::Widget& widget,
                                         TrialBase& trial) const
  { itsImpl->rhAllowResponses(widget, trial); }

void EventResponseHdlr::rhDenyResponses() const
  { itsImpl->rhDenyResponses(); }

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
