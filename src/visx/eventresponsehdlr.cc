///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov  9 15:32:48 1999
// written: Thu Jul 12 18:24:24 2001
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
#include "trialbase.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/widget.h"

#include "tcl/convert.h"
#include "tcl/tclcmd.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclobjptr.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclutil.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include <tcl.h>

#define DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string_literal ioTag("EventResponseHdlr");

  const string_literal nullScript("{}");

  Tcl::ObjPtr theNullObject("");
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
    };
  };

  void changeState(shared_ptr<ERHState> new_state) const
  { itsState = new_state; }

  // Delegand functions

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  const fixed_string& getInputResponseMap() const
    { return itsInputResponseMap; }

  void setInputResponseMap(const fixed_string& s)
    {
      itsInputResponseMap = s;
      itsRegexpsAreDirty = true;
      updateRegexpsIfNeeded();
    }

  bool getUseFeedback() const
    { return itsUseFeedback; }

  void setUseFeedback(bool val)
    { itsUseFeedback = val; }

  const char* getFeedbackMap() const
    { return itsFeedbackMap.c_str(); }

  void setFeedbackMap(const char* feedback_string)
    {
      itsFeedbackMap = feedback_string;
      itsFeedbacksAreDirty = true;
      updateFeedbacksIfNeeded();
    }

  const fixed_string& getEventSequence() const
    { return itsEventSequence; }

  void setEventSequence(const fixed_string& seq)
    { itsEventSequence = seq; }

  const fixed_string& getBindingSubstitution() const
    { return itsBindingSubstitution; }

  void setBindingSubstitution(const fixed_string& sub)
    { itsBindingSubstitution = sub.c_str(); }

  void abortInvalidResponses()
    { itsAbortInvalidResponses = true; }

  void ignoreInvalidResponses()
    { itsAbortInvalidResponses = false; }

  void rhBeginTrial(GWT::Widget& widget, TrialBase& trial) const
    {
      clearEventQueue();

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

  int getRespFromKeysym(const char* keysym) const;
  void feedback(int response) const;
  void updateFeedbacksIfNeeded() const;
  void updateRegexpsIfNeeded() const;

  dynamic_string getBindingScript() const;

  void clearEventQueue() const
    {
      while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT) != 0)
        { /* Empty loop body */ }
    }

  // We take the last character of the string as the response, in
  // order to handle the numeric keypad, where the keysysms are
  // 'KP_0', 'KP_3', etc., wheras we want just the 0 or the 3.
  const char* extractStringFromKeysym(const char* text) const
    {
      while ( *(++text) != '\0' )
        { ; }
      return (text-1);
    }

  // data
private:
  EventResponseHdlr* itsOwner;

  mutable shared_ptr<ERHState> itsState;

  Tcl::SafeInterp itsSafeIntp;

  PrivateHandleCmd* itsCmdCallback;

  class RegExp_ResponseVal {
  public:
    RegExp_ResponseVal(Tcl::ObjPtr obj = theNullObject, int rv = -1) :
      itsPatternObj(obj),
      itsRespVal(rv)
      {}

    RegExp_ResponseVal(const RegExp_ResponseVal& other) :
      itsPatternObj(other.itsPatternObj),
      itsRespVal(other.itsRespVal)
      {}

    RegExp_ResponseVal& operator=(const RegExp_ResponseVal& other)
      {
        itsPatternObj = other.itsPatternObj;
        itsRespVal = other.itsRespVal;
        return *this;
      }

    bool matchesString(const char* string_to_match) throw(ErrorWithMsg)
      {
        static const int REGEX_ERROR        = -1;
        static const int REGEX_NO_MATCH     =  0;
        static const int REGEX_FOUND_MATCH  =  1;

        DebugEval(string_to_match); DebugEvalNL(Tcl_GetString(itsPatternObj));

        Tcl_RegExp regexp = getCheckedRegexp(itsPatternObj);

        // OK to pass Tcl_Interp*==0
        int regex_result =
          Tcl_RegExpExec(0, regexp, string_to_match, string_to_match);

        switch (regex_result) {
        case REGEX_ERROR:
          throw ErrorWithMsg("error executing regular expression "
                             "for EventResponseHdlr");

        case REGEX_NO_MATCH:
          return false;

        case REGEX_FOUND_MATCH:
          return true;

        default: // "can't happen"
          Assert(false);
        }

        return false;
      }

    int responseValue() { return itsRespVal; }

  private:
    static Tcl_RegExp getCheckedRegexp(Tcl::ObjPtr patternObj)
      throw(ErrorWithMsg)
      {
        const int flags = 0;
        // OK to pass Tcl_Interp*==0
        Tcl_RegExp regexp = Tcl_GetRegExpFromObj(0, patternObj, flags);
        if (!regexp) {
          throw ErrorWithMsg("error getting a Tcl_RegExp from ")
                  .appendMsg("'", Tcl_GetString(patternObj), "'");
        }
        return regexp;
      }

    Tcl::ObjPtr itsPatternObj;
    int itsRespVal;
  };

  fixed_string itsInputResponseMap;
  mutable dynamic_block<RegExp_ResponseVal> itsRegexps;

  mutable bool itsRegexpsAreDirty;

  class Condition_Feedback {
  public:
    // This no-argument constructor puts the object in an invalid
    // state, which we mark with itsIsValid=false. In order for the
    // object to be used, it must be assigned to with the default
    // assignment constructor.
    Condition_Feedback() :
      itsIsValid(false),
      itsCondition(theNullObject),
      itsResultCmd(theNullObject)
      {}

    Condition_Feedback(Tcl_Obj* cond, Tcl_Obj* res) :
      itsIsValid(false),
      itsCondition(cond),
      itsResultCmd(res, Tcl::TclEvalCmd::THROW_EXCEPTION, TCL_EVAL_GLOBAL)
      {
        if (cond != 0 && res != 0)
          itsIsValid = true;
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
        Precondition(itsIsValid);
        return safeInterp.evalBooleanExpr(itsCondition);
      }

    void invoke(const Tcl::SafeInterp& safeInterp) throw(ErrorWithMsg)
      { itsResultCmd.invoke(safeInterp.intp()); }

    bool itsIsValid;
    Tcl::ObjPtr itsCondition;
    Tcl::TclEvalCmd itsResultCmd;
  };

  fixed_string itsFeedbackMap;
  mutable dynamic_block<Condition_Feedback> itsFeedbacks;

  mutable bool itsFeedbacksAreDirty;

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

  theResponse.setVal(itsErh.getRespFromKeysym(keysym));
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
  itsInputResponseMap(input_response_map),
  itsRegexps(),
  itsRegexpsAreDirty(true),
  itsFeedbackMap(),
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

  reader->readValue("inputResponseMap", itsInputResponseMap);
  reader->readValue("feedbackMap", itsFeedbackMap);
  reader->readValue("useFeedback", itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);

  itsRegexpsAreDirty = true;
  itsFeedbacksAreDirty = true;
  updateRegexpsIfNeeded();
  updateFeedbacksIfNeeded();
}

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("EventResponseHdlr::Impl::writeTo");

  writer->writeValue("inputResponseMap", itsInputResponseMap);
  writer->writeValue("feedbackMap", itsFeedbackMap);
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


//--------------------------------------------------------------------
//
// EventResponseHdlr::getRespFromKeysysm --
//
// This procedure returns the response value associated with the first
// regexp in itsRegexps that matches the given character string, or
// returns INVALID_RESPONSE if none of the regexps give a match.
//
//--------------------------------------------------------------------

int EventResponseHdlr::Impl::getRespFromKeysym(const char* keysym) const {
DOTRACE("EventResponseHdlr::Impl::getRespFromKeysym");

  const char* response_string = extractStringFromKeysym(keysym);

  DebugEvalNL(response_string);

  updateRegexpsIfNeeded();

  DebugEvalNL(itsRegexps.size());

  for (size_t i = 0; i < itsRegexps.size(); ++i) {

    DebugEvalNL(i);

    if (itsRegexps[i].matchesString(response_string)) {
      return itsRegexps[i].responseValue();
    }
  }

  return ResponseHandler::INVALID_RESPONSE;
}

void EventResponseHdlr::Impl::feedback(int response) const {
DOTRACE("EventResponseHdlr::Impl::feedback");

  if (!itsUseFeedback) return;

  Precondition(itsSafeIntp.hasInterp());

  DebugEvalNL(response);

  updateFeedbacksIfNeeded();

  itsSafeIntp.setGlobalVar("resp_val", Tcl_NewIntObj(response));

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

  Tcl::List pairs_list(Tcl::ObjPtr(itsFeedbackMap.c_str()));

  unsigned int uint_num_pairs = pairs_list.length();

  itsFeedbacks.resize(uint_num_pairs);

  for (unsigned int i = 0; i < uint_num_pairs; ++i)
    {
      Tcl::List current_pair(pairs_list[i]);

      if (current_pair.length() != 2)
        {
          throw ErrorWithMsg("\"pair\" did not have length 2 "
                             "in EventResponseHdlr::updateFeedbacksIfNeeded");
        }

      itsFeedbacks.at(i) = Impl::Condition_Feedback(current_pair[0],
                                                    current_pair[1]);
    }

  itsFeedbacksAreDirty = false;

  DebugPrintNL("updateFeedbacksIfNeeded success!");
}

//--------------------------------------------------------------------
//
// EventResponseHdlr::updateRegexpsIfNeeded --
//
// Recompiles the internal table of regexps to correspond with the
// list of regexps and response values stored in the string
// itsInputResponseMap.
//
//--------------------------------------------------------------------

void EventResponseHdlr::Impl::updateRegexpsIfNeeded() const {
DOTRACE("EventResponseHdlr::Impl::updateRegexpsIfNeeded");

  if (!itsRegexpsAreDirty) return;

  Tcl::List pairs_list(Tcl::ObjPtr(itsInputResponseMap.c_str()));

  unsigned int uint_num_pairs = pairs_list.length();

  itsRegexps.resize(uint_num_pairs);

  for (unsigned int i = 0; i < uint_num_pairs; ++i)
    {
      Tcl::List current_pair(pairs_list[i]);

      if (current_pair.length() != 2)
        {
          throw ErrorWithMsg("\"pair\" did not have length 2 "
                             "in EventResponseHdlr::updateRegexpsIfNeeded");
        }

      Tcl::ObjPtr patternObj = current_pair[0];
      int response_val = current_pair.get<int>(1);

      itsRegexps.at(i) = Impl::RegExp_ResponseVal(patternObj, response_val);
    }

  itsRegexpsAreDirty = false;

  DebugPrintNL("updateRegexpsIfNeeded success!");
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

const fixed_string& EventResponseHdlr::getInputResponseMap() const {
DOTRACE("EventResponseHdlr::getInputResponseMap");
  return itsImpl->getInputResponseMap();
}

void EventResponseHdlr::setInputResponseMap(const fixed_string& s) {
DOTRACE("EventResponseHdlr::setInputResponseMap");
  itsImpl->setInputResponseMap(s);
}

bool EventResponseHdlr::getUseFeedback() const {
DOTRACE("EventResponseHdlr::getUseFeedback");
  return itsImpl->getUseFeedback();
}

void EventResponseHdlr::setUseFeedback(bool val) {
DOTRACE("EventResponseHdlr::setUseFeedback");
  itsImpl->setUseFeedback(val);
}

const char* EventResponseHdlr::getFeedbackMap() const {
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return itsImpl->getFeedbackMap();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string) {
DOTRACE("EventResponseHdlr::setFeedbackMap");
  itsImpl->setFeedbackMap(feedback_string);
}

const fixed_string& EventResponseHdlr::getEventSequence() const
  { return itsImpl->getEventSequence(); }

void EventResponseHdlr::setEventSequence(const fixed_string& seq)
  { itsImpl->setEventSequence(seq); }

const fixed_string& EventResponseHdlr::getBindingSubstitution() const
  { return itsImpl->getBindingSubstitution(); }

void EventResponseHdlr::setBindingSubstitution(const fixed_string& sub)
  { itsImpl->setBindingSubstitution(sub); }

void EventResponseHdlr::abortInvalidResponses()
  { itsImpl->abortInvalidResponses(); }

void EventResponseHdlr::ignoreInvalidResponses()
  { itsImpl->ignoreInvalidResponses(); }

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
