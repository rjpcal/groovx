///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov  9 15:32:48 1999
// written: Tue Dec 10 16:58:10 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "visx/eventresponsehdlr.h"

#include "io/reader.h"
#include "io/writer.h"

#include "tcl/tclfunctor.h"
#include "tcl/tclmain.h"
#include "tcl/tclprocwrapper.h"
#include "tcl/tclsafeinterp.h"
#include "tcl/toglet.h"

#include "util/log.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include "visx/feedbackmap.h"
#include "visx/sound.h"
#include "visx/response.h"
#include "visx/trial.h"

#define DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId ERH_SERIAL_VERSION_ID = 2;

  fstring uniqCmdName(const char* stem)
  {
    static int cmdCounter = 0;

    return fstring("::__ERHPrivate::", stem, ++cmdCounter);
  }
}

///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class EventResponseHdlr::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(EventResponseHdlr* owner);
  ~Impl();

  class ActiveState;
  friend class ActiveState;

  class ActiveState
  {
  private:
    Util::SoftRef<Toglet> itsWidget;
    Trial& itsTrial;
    fstring itsEventSequence;
    fstring itsBindingScript;
    unsigned int itsResponseCount;

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
    ~ActiveState() { ignore(); }

    ActiveState(const EventResponseHdlr::Impl* erh,
                Util::SoftRef<Toglet> widget, Trial& trial,
                const fstring& seq, const fstring& script) :
      itsWidget(widget),
      itsTrial(trial),
      itsEventSequence(seq),
      itsBindingScript(script),
      itsResponseCount(0)
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

    void handleResponse(EventResponseHdlr::Impl* impl, const char* event_info)
    {
      Response theResponse;

      theResponse.setMsec(int(itsTrial.trElapsedMsec()));

      itsTrial.trResponseSeen();

      if (++itsResponseCount >= impl->itsMaxResponses)
        ignore();

      Util::log( fstring("event_info: ", event_info) );

      theResponse.setVal(Response::INVALID_VALUE);

      if ( !impl->itsResponseProc->isNoop() )
        {
          try {
            theResponse.setVal(impl->itsResponseProc->call<int>(event_info));
          } catch (...) {}
        }

      Util::log( fstring("response val: ", theResponse.val()) );

      if ( !theResponse.isValid() )
        {
          if ( impl->itsAbortInvalidResponses )
            itsTrial.trAbort();
        }
      else
        {
          itsTrial.trProcessResponse(theResponse);
          impl->itsFeedbackMap.giveFeedback(impl->itsInterp, theResponse.val());
        }
    }
  };

  void becomeActive(Util::SoftRef<Toglet> widget, Trial& trial) const
  {
    Util::log( fstring("binding to ", itsCmdCallback->name()) );

    fstring script(itsCmdCallback->name(), ' ', (int)itsOwner->id());
    script.append(" [list ", itsBindingSubstitution, ']');

    itsState.reset(new ActiveState(this, widget, trial,
                                   itsEventSequence, script));
  }

  void becomeInactive() const { itsState.reset(0); }

  bool isActive() const   { return itsState.get() != 0; }
  bool isInactive() const { return itsState.get() == 0; }

  // Delegand functions

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  static void handleResponseCallback(Util::Ref<EventResponseHdlr> erh,
                                     const char* event_info)
  {
    EventResponseHdlr::Impl* impl = erh->itsImpl;
    Precondition( impl->isActive() );

    impl->itsState->handleResponse(impl, event_info);
  }

  //
  // data
  //

  EventResponseHdlr* itsOwner;

  mutable scoped_ptr<ActiveState> itsState;

  Tcl::Interp itsInterp;

  scoped_ptr<Tcl::Command> itsCmdCallback;

  FeedbackMap itsFeedbackMap;

  fstring itsEventSequence;
  fstring itsBindingSubstitution;

  bool itsAbortInvalidResponses;

  Util::Ref<Tcl::ProcWrapper> itsResponseProc;

  unsigned int itsMaxResponses;
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

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner) :
  itsOwner(owner),
  itsState(0),
  itsInterp(Tcl::Main::interp()),
  itsCmdCallback(Tcl::makeCmd(itsInterp, &handleResponseCallback,
                              uniqCmdName("handler").c_str(), "<private>")),
  itsFeedbackMap(),
  itsEventSequence("<KeyPress>"),
  itsBindingSubstitution("%K"),
  itsAbortInvalidResponses(true),
  itsResponseProc(new Tcl::ProcWrapper(itsInterp, uniqCmdName("responseProc"))),
  itsMaxResponses(1)
{
DOTRACE("EventResponseHdlr::Impl::Impl");
}

EventResponseHdlr::Impl::~Impl()
{
DOTRACE("EventResponseHdlr::Impl::~Impl");

  // force the destructor to run now, rather than after ~Impl()
  // finishes, so that the reference to *this in ActiveState does
  // not become invalid
  itsState.reset(0);
}

void EventResponseHdlr::Impl::readFrom(IO::Reader* reader)
{
DOTRACE("EventResponseHdlr::Impl::readFrom");

  const int svid = reader->ensureReadVersionId("EventResponseHdlr", 0,
                                               "Try grsh0.8a7");

  becomeInactive();

  if (svid < 2)
    {
      fstring rmap;
      reader->readValue("inputResponseMap", rmap);
      itsOwner->setInputResponseMap(rmap);
    }

  {
    fstring rep;
    reader->readValue("feedbackMap", rep);
    itsFeedbackMap.set(rep);
  }

  reader->readValue("useFeedback", itsFeedbackMap.itsUseFeedback);
  reader->readValue("eventSequence", itsEventSequence);
  reader->readValue("bindingSubstitution", itsBindingSubstitution);

  if (svid >= 2)
    {
      reader->readOwnedObject("responseProc", itsResponseProc);
    }
}

void EventResponseHdlr::Impl::writeTo(IO::Writer* writer) const
{
DOTRACE("EventResponseHdlr::Impl::writeTo");

  writer->ensureWriteVersionId("EventResponseHdlr", ERH_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a7");

  writer->writeValue("feedbackMap", itsFeedbackMap.rep());
  writer->writeValue("useFeedback", itsFeedbackMap.itsUseFeedback);
  writer->writeValue("eventSequence", itsEventSequence);
  writer->writeValue("bindingSubstitution", itsBindingSubstitution);

  writer->writeOwnedObject("responseProc", itsResponseProc);
}


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr* EventResponseHdlr::make()
{
DOTRACE("EventResponseHdlr::make");
  return new EventResponseHdlr;
}

EventResponseHdlr::EventResponseHdlr() :
  ResponseHandler(),
  itsImpl(new Impl(this))
{}

EventResponseHdlr::~EventResponseHdlr()
  { delete itsImpl; }

IO::VersionId EventResponseHdlr::serialVersionId() const
  { return ERH_SERIAL_VERSION_ID; }

void EventResponseHdlr::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void EventResponseHdlr::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }

void EventResponseHdlr::setInputResponseMap(const fstring& s)
{
DOTRACE("EventResponseHdlr::setInputResponseMap");

  fstring args("inp");

  fstring body("set map {", s, "}\n"
               "foreach pair $map {\n"
               "  foreach {rx val} $pair {\n"
               "    if [regexp $rx $inp] { return $val }\n"
               "  }\n"
               "}\n"
               "return -1\n");

  setResponseProc(args, body);
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

const fstring& EventResponseHdlr::getEventSequence() const
  { return itsImpl->itsEventSequence; }

void EventResponseHdlr::setEventSequence(const fstring& seq)
  { itsImpl->itsEventSequence = seq; }

const fstring& EventResponseHdlr::getBindingSubstitution() const
  { return itsImpl->itsBindingSubstitution; }

void EventResponseHdlr::setBindingSubstitution(const fstring& sub)
  { itsImpl->itsBindingSubstitution = sub; }

fstring EventResponseHdlr::getResponseProc() const
{
  return itsImpl->itsResponseProc->fullSpec();
}

void EventResponseHdlr::setResponseProc(const fstring& args,
                                        const fstring& body)
{
  itsImpl->itsResponseProc->define(args, body);
}

void EventResponseHdlr::abortInvalidResponses()
  { itsImpl->itsAbortInvalidResponses = true; }

void EventResponseHdlr::ignoreInvalidResponses()
  { itsImpl->itsAbortInvalidResponses = false; }

void EventResponseHdlr::setMaxResponses(unsigned int count)
  { itsImpl->itsMaxResponses = count; }

unsigned int EventResponseHdlr::getMaxResponses() const
  { return itsImpl->itsMaxResponses; }

void EventResponseHdlr::rhBeginTrial(Util::SoftRef<Toglet> widget,
                                     Trial& trial) const
{
  Precondition( itsImpl->isInactive() );

  itsImpl->itsInterp.clearEventQueue();

  itsImpl->becomeActive(widget, trial);

  Postcondition( itsImpl->isActive() );
}

void EventResponseHdlr::rhAbortTrial() const
{
  if ( itsImpl->isActive() )
    itsImpl->itsState->rhAbortTrial();
}

void EventResponseHdlr::rhEndTrial() const
{
  if ( itsImpl->isActive() )
    {
      itsImpl->itsState->rhEndTrial();
      itsImpl->becomeInactive();
    }

  Postcondition( itsImpl->isInactive() );
}

void EventResponseHdlr::rhHaltExpt() const
{
  if ( itsImpl->isActive() )
    {
      itsImpl->itsState->rhHaltExpt();
      itsImpl->becomeInactive();
    }

  Postcondition( itsImpl->isInactive() );
}

void EventResponseHdlr::rhAllowResponses(Util::SoftRef<Toglet> widget,
                                         Trial& trial) const
{
  itsImpl->becomeActive(widget, trial);

  Postcondition( itsImpl->isActive() );
}

void EventResponseHdlr::rhDenyResponses() const
{
  itsImpl->becomeInactive();

  Postcondition( itsImpl->isInactive() );
}

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
