///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov  9 15:32:48 1999
// written: Wed Aug  8 19:00:54 2001
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

#include "tcl/tclmemfunctor.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#define DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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

  class ERHActiveState;
  friend class ERHActiveState;

  class ERHActiveState {
  private:
    Util::WeakRef<GWT::Widget> itsWidget;
    TrialBase& itsTrial;
    fixed_string itsEventSequence;
    fixed_string itsBindingScript;

    void attend()
    {
      if (itsWidget.isValid())
        itsWidget->bind(itsEventSequence.c_str(), itsBindingScript.c_str());
    }

    void ignore()
    {
      if (itsWidget.isValid())
        itsWidget->bind(itsEventSequence.c_str(), "");
    }

  public:
    ~ERHActiveState() { ignore(); }

    ERHActiveState(const EventResponseHdlr::Impl* erh,
                   Util::WeakRef<GWT::Widget> widget, TrialBase& trial,
                   const fixed_string& seq, const fixed_string& script) :
      itsWidget(widget),
      itsTrial(trial),
      itsEventSequence(seq),
      itsBindingScript(script)
    {
      Precondition((erh != 0) && (widget.isValid()) && (&trial != 0));
      attend();
    }

    void rhAbortTrial()
    {
      ignore();

      Ref<Sound> p = Sound::getErrSound();
      p->play();
    }

    void rhEndTrial() { ignore(); }

    void rhHaltExpt() { ignore(); }

    void handleResponse(EventResponseHdlr::Impl* impl, const char* keysym)
    {
      DOTRACE("EventResponseHdlr::Impl::ERHActiveState::handleResponse");

      Response theResponse;

      theResponse.setMsec(itsTrial.trElapsedMsec());

      itsTrial.trResponseSeen();

      ignore();

      theResponse.setVal(impl->itsResponseMap.valueFor(keysym));
      DebugEvalNL(theResponse.val());

      if ( !theResponse.isValid() )
        {
          if ( impl->itsAbortInvalidResponses )
            itsTrial.trAbortTrial();
        }
      else
        {
          itsTrial.trRecordResponse(theResponse);
          impl->itsFeedbackMap.giveFeedback(impl->itsInterp, theResponse.val());
        }
    }
  };

  void becomeActive(Util::WeakRef<GWT::Widget> widget, TrialBase& trial) const
  {
    fixed_string script(itsCmdCallback->name());
    script.append(" ").append((int)itsOwner->id());
    script.append(" ").append(itsBindingSubstitution);

    itsState.reset(new ERHActiveState(this, widget, trial,
                                      itsEventSequence, script.c_str()));
  }

  void becomeInactive() const { itsState.reset(0); }

  bool isActive() const   { return itsState.get() != 0; }
  bool isInactive() const { return itsState.get() == 0; }

  // Delegand functions

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  static void handleResponseCallback(Util::Ref<EventResponseHdlr> erh,
                                     const char* keysym)
  {
    EventResponseHdlr::Impl* impl = erh->itsImpl;
    Precondition( impl->isActive() );
    impl->itsState->handleResponse(impl, keysym);
  }

  static const char* uniqueCmdName()
  {
    static fixed_string baseName;
    static int cmdCounter = 0;

    baseName = "__EventResponseHdlrPrivate::handle";
    baseName.append(++cmdCounter);
    return baseName.c_str();
  }

  //
  // data
  //

  EventResponseHdlr* itsOwner;

  mutable scoped_ptr<ERHActiveState> itsState;

  Tcl::Interp itsInterp;

  scoped_ptr<Tcl::TclCmd> itsCmdCallback;

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


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner,
                              const char* input_response_map) :
  itsOwner(owner),
  itsState(0),
  itsInterp(dynamic_cast<GrshApp&>(Application::theApp()).getInterp()),
  itsCmdCallback(Tcl::makeCmd(itsInterp.intp(), &handleResponseCallback,
                              uniqueCmdName(), "<private>")),
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

  // force the destructor to run now, rather than after ~Impl()
  // finishes, so that the reference to *this in ERHActiveState does
  // not become invalid
  itsState.reset(0);
}

void EventResponseHdlr::Impl::readFrom(IO::Reader* reader)
{
DOTRACE("EventResponseHdlr::Impl::readFrom");

  becomeInactive();

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

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const
{
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

void EventResponseHdlr::setInputResponseMap(const fixed_string& s)
{
DOTRACE("EventResponseHdlr::setInputResponseMap");
  itsImpl->itsResponseMap.set(s);
}

bool EventResponseHdlr::getUseFeedback() const
{
DOTRACE("EventResponseHdlr::getUseFeedback");
  return itsImpl->itsFeedbackMap.itsUseFeedback;
}

void EventResponseHdlr::setUseFeedback(bool val)
{
DOTRACE("EventResponseHdlr::setUseFeedback");
  itsImpl->itsFeedbackMap.itsUseFeedback = val;
}

const char* EventResponseHdlr::getFeedbackMap() const
{
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return itsImpl->itsFeedbackMap.rep().c_str();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string)
{
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
{
  Precondition( itsImpl->isInactive() );

  itsImpl->itsInterp.clearEventQueue();

  itsImpl->becomeActive(widget, trial);
}

void EventResponseHdlr::rhAbortTrial() const
{
  Precondition( itsImpl->isActive() );
  itsImpl->itsState->rhAbortTrial();
}

void EventResponseHdlr::rhEndTrial() const
{
  Precondition( itsImpl->isActive() );
  itsImpl->itsState->rhEndTrial();
  itsImpl->becomeInactive();
}

void EventResponseHdlr::rhHaltExpt() const
{
  if ( itsImpl->isInactive() ) return;

  itsImpl->itsState->rhHaltExpt();
  itsImpl->becomeInactive();
}

void EventResponseHdlr::rhAllowResponses(Util::WeakRef<GWT::Widget> widget,
                                         TrialBase& trial) const
{
  itsImpl->becomeActive(widget, trial);
}

void EventResponseHdlr::rhDenyResponses() const
{
  itsImpl->becomeInactive();
}

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
