///////////////////////////////////////////////////////////////////////
//
// eventresponsehdlr.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Nov  9 15:32:48 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef EVENTRESPONSEHDLR_CC_DEFINED
#define EVENTRESPONSEHDLR_CC_DEFINED

#include "visx/eventresponsehdlr.h"

#include "gfx/toglet.h"

#include "io/reader.h"
#include "io/tclprocwrapper.h"
#include "io/writer.h"

#include "tcl/tclfunctor.h"
#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/fileposition.h"
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
DBG_REGISTER

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
      PRECONDITION((erh != 0) && (widget.isValid()) && (&trial != 0));
      attend();
    }

    void rhAbortTrial()
    {
      ignore();

      Util::Ref<Sound> p = Sound::getErrSound();
      p->play();
    }

    void rhEndTrial() { ignore(); }

    void rhHaltExpt() { ignore(); }

    void handleResponse(EventResponseHdlr::Impl* rep, const char* event_info)
    {
      Response theResponse;

      theResponse.setMsec(int(itsTrial.trElapsedMsec() + 0.5));

      Util::log( fstring("event_info: ", event_info) );

      theResponse.setVal(Response::INVALID_VALUE);

      if ( !rep->itsResponseProc->isNoop() )
        {
          try {
            theResponse.setVal(rep->itsResponseProc->call<int>(event_info));
          } catch (...) {}
        }

      Util::log( fstring("response val: ", theResponse.val()) );

      if (theResponse.shouldIgnore())
        return;

      if (++itsResponseCount >= rep->itsMaxResponses)
        ignore();

      if ( !theResponse.isValid() )
        {
          if ( rep->itsAbortInvalidResponses )
            itsTrial.trAbortTrial();
        }
      else
        {
          itsTrial.trProcessResponse(theResponse);
          rep->itsFeedbackMap.giveFeedback(rep->itsInterp, theResponse.val());
        }
    }
  };

  void becomeActive(Util::SoftRef<Toglet> widget, Trial& trial) const
  {
    Util::log( fstring("binding to ", itsCallbackName) );

    fstring script(itsCallbackName, ' ', (int)itsOwner->id());
    script.append(" [list ", itsBindingSubstitution, ']');

    itsState.reset(new ActiveState(this, widget, trial,
                                   itsEventSequence, script));
  }

  void becomeInactive() const { itsState.reset(0); }

  bool isActive() const   { return itsState.get() != 0; }
  bool isInactive() const { return itsState.get() == 0; }

  static void handleResponseCallback(Util::Ref<EventResponseHdlr> erh,
                                     const char* event_info)
  {
    EventResponseHdlr::Impl* rep = erh->rep;
    PRECONDITION( rep->isActive() );

    rep->itsState->handleResponse(rep, event_info);
  }

  //
  // data
  //

  EventResponseHdlr* itsOwner;

  mutable scoped_ptr<ActiveState> itsState;

  Tcl::Interp itsInterp;

  fstring itsCallbackName;
  shared_ptr<Tcl::Command> itsCmdCallback;

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
  itsCallbackName(uniqCmdName("handler")),
  itsCmdCallback(Tcl::makeCmd(itsInterp, &handleResponseCallback,
                              itsCallbackName.c_str(), "<private>",
                              SRC_POS)),
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
  rep(new Impl(this))
{}

EventResponseHdlr::~EventResponseHdlr() throw()
  { delete rep; }

IO::VersionId EventResponseHdlr::serialVersionId() const
  { return ERH_SERIAL_VERSION_ID; }

void EventResponseHdlr::readFrom(IO::Reader& reader)
{
DOTRACE("EventResponseHdlr::readFrom");

  reader.ensureReadVersionId("EventResponseHdlr", 2,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  rep->becomeInactive();

  fstring fmap;
  reader.readValue("feedbackMap", fmap);
  rep->itsFeedbackMap.set(fmap);

  reader.readValue("useFeedback", rep->itsFeedbackMap.itsUseFeedback);
  reader.readValue("eventSequence", rep->itsEventSequence);
  reader.readValue("bindingSubstitution", rep->itsBindingSubstitution);

  reader.readOwnedObject("responseProc", rep->itsResponseProc);
}

void EventResponseHdlr::writeTo(IO::Writer& writer) const
{
DOTRACE("EventResponseHdlr::writeTo");

  writer.ensureWriteVersionId("EventResponseHdlr", ERH_SERIAL_VERSION_ID, 2,
                              "Try groovx0.8a7", SRC_POS);

  writer.writeValue("feedbackMap", rep->itsFeedbackMap.rep());
  writer.writeValue("useFeedback", rep->itsFeedbackMap.itsUseFeedback);
  writer.writeValue("eventSequence", rep->itsEventSequence);
  writer.writeValue("bindingSubstitution", rep->itsBindingSubstitution);

  writer.writeOwnedObject("responseProc", rep->itsResponseProc);
}

// FIXME XML still need this?
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
  return rep->itsFeedbackMap.itsUseFeedback;
}

void EventResponseHdlr::setUseFeedback(bool val)
{
DOTRACE("EventResponseHdlr::setUseFeedback");
  rep->itsFeedbackMap.itsUseFeedback = val;
}

const char* EventResponseHdlr::getFeedbackMap() const
{
DOTRACE("EventResponseHdlr::getFeedbackMap");
  return rep->itsFeedbackMap.rep().c_str();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string)
{
DOTRACE("EventResponseHdlr::setFeedbackMap");
  rep->itsFeedbackMap.set(feedback_string);
}

const fstring& EventResponseHdlr::getEventSequence() const
  { return rep->itsEventSequence; }

void EventResponseHdlr::setEventSequence(const fstring& seq)
  { rep->itsEventSequence = seq; }

const fstring& EventResponseHdlr::getBindingSubstitution() const
  { return rep->itsBindingSubstitution; }

void EventResponseHdlr::setBindingSubstitution(const fstring& sub)
  { rep->itsBindingSubstitution = sub; }

fstring EventResponseHdlr::getResponseProc() const
{
  return rep->itsResponseProc->fullSpec();
}

void EventResponseHdlr::setResponseProc(const fstring& args,
                                        const fstring& body)
{
  rep->itsResponseProc->define(args, body);
}

void EventResponseHdlr::abortInvalidResponses()
  { rep->itsAbortInvalidResponses = true; }

void EventResponseHdlr::ignoreInvalidResponses()
  { rep->itsAbortInvalidResponses = false; }

void EventResponseHdlr::setMaxResponses(unsigned int count)
  { rep->itsMaxResponses = count; }

unsigned int EventResponseHdlr::getMaxResponses() const
  { return rep->itsMaxResponses; }

void EventResponseHdlr::rhBeginTrial(Util::SoftRef<Toglet> widget,
                                     Trial& trial) const
{
  PRECONDITION( rep->isInactive() );

  rep->itsInterp.clearEventQueue();

  rep->becomeActive(widget, trial);

  POSTCONDITION( rep->isActive() );
}

void EventResponseHdlr::rhAbortTrial() const
{
  if ( rep->isActive() )
    rep->itsState->rhAbortTrial();
}

void EventResponseHdlr::rhEndTrial() const
{
  if ( rep->isActive() )
    {
      rep->itsState->rhEndTrial();
      rep->becomeInactive();
    }

  POSTCONDITION( rep->isInactive() );
}

void EventResponseHdlr::rhHaltExpt() const
{
  if ( rep->isActive() )
    {
      rep->itsState->rhHaltExpt();
      rep->becomeInactive();
    }

  POSTCONDITION( rep->isInactive() );
}

void EventResponseHdlr::rhAllowResponses(Util::SoftRef<Toglet> widget,
                                         Trial& trial) const
{
  rep->becomeActive(widget, trial);

  POSTCONDITION( rep->isActive() );
}

void EventResponseHdlr::rhDenyResponses() const
{
  rep->becomeInactive();

  POSTCONDITION( rep->isInactive() );
}

static const char vcid_eventresponsehdlr_cc[] = "$Header$";
#endif // !EVENTRESPONSEHDLR_CC_DEFINED
