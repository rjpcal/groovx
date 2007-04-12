/** @file visx/eventresponsehdlr.cc collects and processes user
    responses during a psychophysics experiment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov  9 15:32:48 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_EVENTRESPONSEHDLR_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_EVENTRESPONSEHDLR_CC_UTC20050626084016_DEFINED

#include "visx/eventresponsehdlr.h"

#include "io/reader.h"
#include "io/writer.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "tcl/makecmd.h"
#include "tcl/eventloop.h"
#include "tcl/interp.h"

#include "tcl-gfx/toglet.h"
#include "tcl-io/tclprocwrapper.h"

#include "rutz/fileposition.h"
#include "rutz/fstring.h"
#include "rutz/shared_ptr.h"
#include "rutz/scopedptr.h"
#include "rutz/sfmt.h"

#include "visx/feedbackmap.h"
#include "visx/sound.h"
#include "visx/response.h"
#include "visx/trial.h"

#define GVX_DYNAMIC_TRACE_EXPR EventResponseHdlr::tracer.status()
#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const io::version_id ERH_SVID = 2;

  fstring uniqCmdName(const char* stem)
  {
    static int cmdCounter = 0;

    return rutz::sfmt("::__ERHPrivate::%s%d", stem, ++cmdCounter);
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
    nub::soft_ref<Toglet> itsWidget;
    Trial& itsTrial;
    fstring itsEventSequence;
    fstring itsBindingScript;
    unsigned int itsResponseCount;

    void attend()
    {
      if (itsWidget.is_valid())
        itsWidget->bind(itsEventSequence.c_str(), itsBindingScript.c_str());
    }

    void ignore()
    {
      if (itsWidget.is_valid())
        itsWidget->bind(itsEventSequence.c_str(), "");
    }

  public:
    ~ActiveState() { ignore(); }

    ActiveState(const EventResponseHdlr::Impl* erh,
                nub::soft_ref<Toglet> widget, Trial& trial,
                const fstring& seq, const fstring& script) :
      itsWidget(widget),
      itsTrial(trial),
      itsEventSequence(seq),
      itsBindingScript(script),
      itsResponseCount(0)
    {
      GVX_PRECONDITION((erh != 0) && (widget.is_valid()) && (&trial != 0));
      attend();
    }

    void rhAbortTrial()
    {
      ignore();

      nub::ref<Sound> p = Sound::getErrSound();
      p->play();
    }

    void rhEndTrial() { ignore(); }

    void rhHaltExpt() { ignore(); }

    void handleResponse(EventResponseHdlr::Impl* rep, const char* event_info)
    {
      Response theResponse;

      theResponse.setMsec(int(itsTrial.trElapsedMsec() + 0.5));

      nub::log( rutz::sfmt("event_info: %s", event_info) );

      theResponse.setVal(Response::INVALID_VALUE);

      if ( !rep->itsResponseProc->isNoop() )
        {
          try {
            theResponse.setVal(rep->itsResponseProc->call<int>(event_info));
          } catch (...) {}
        }

      nub::log( rutz::sfmt("response val: %d", theResponse.val()) );

      if (theResponse.shouldIgnore())
        return;

      if (++itsResponseCount >= rep->itsMaxResponses)
        ignore();

      if ( !theResponse.is_valid() )
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

  void becomeActive(nub::soft_ref<Toglet> widget, Trial& trial) const
  {
    nub::log( rutz::sfmt("binding to %s", itsCallbackName.c_str()) );

    const fstring script =
      rutz::sfmt("%s %lu [list %s]",
                 itsCallbackName.c_str(), itsOwner->id(),
                 itsBindingSubstitution.c_str());

    itsState.reset(new ActiveState(this, widget, trial,
                                   itsEventSequence, script));
  }

  void becomeInactive() const { itsState.reset(0); }

  bool isActive() const   { return itsState.get() != 0; }
  bool isInactive() const { return itsState.get() == 0; }

  static void handleResponseCallback(nub::ref<EventResponseHdlr> erh,
                                     const char* event_info)
  {
    EventResponseHdlr::Impl* rep = erh->rep;
    GVX_PRECONDITION( rep->isActive() );

    rep->itsState->handleResponse(rep, event_info);
  }

  //
  // data
  //

  EventResponseHdlr* itsOwner;

  mutable rutz::scoped_ptr<ActiveState> itsState;

  tcl::interpreter itsInterp;

  fstring itsCallbackName;

  FeedbackMap itsFeedbackMap;

  fstring itsEventSequence;
  fstring itsBindingSubstitution;

  bool itsAbortInvalidResponses;

  nub::ref<tcl::ProcWrapper> itsResponseProc;

  unsigned int itsMaxResponses;
};

///////////////////////////////////////////////////////////////////////
//
// Static member definitions
//
///////////////////////////////////////////////////////////////////////

rutz::tracer EventResponseHdlr::tracer;


///////////////////////////////////////////////////////////////////////
//
// EventResponseHdlr::Impl method definitions
//
///////////////////////////////////////////////////////////////////////

EventResponseHdlr::Impl::Impl(EventResponseHdlr* owner) :
  itsOwner(owner),
  itsState(0),
  itsInterp(tcl::event_loop::interp()),
  itsCallbackName(uniqCmdName("handler")),
  itsFeedbackMap(),
  itsEventSequence("<KeyPress>"),
  itsBindingSubstitution("%K"),
  itsAbortInvalidResponses(true),
  itsResponseProc(new tcl::ProcWrapper(itsInterp, uniqCmdName("responseProc"))),
  itsMaxResponses(1)
{
GVX_TRACE("EventResponseHdlr::Impl::Impl");

  tcl::make_command(itsInterp, &handleResponseCallback,
                    itsCallbackName.c_str(), "<private>", SRC_POS);
}

EventResponseHdlr::Impl::~Impl()
{
GVX_TRACE("EventResponseHdlr::Impl::~Impl");

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
GVX_TRACE("EventResponseHdlr::make");
  return new EventResponseHdlr;
}

EventResponseHdlr::EventResponseHdlr() :
  ResponseHandler(),
  rep(new Impl(this))
{}

EventResponseHdlr::~EventResponseHdlr() throw()
  { delete rep; }

io::version_id EventResponseHdlr::class_version_id() const
  { return ERH_SVID; }

void EventResponseHdlr::read_from(io::reader& reader)
{
GVX_TRACE("EventResponseHdlr::read_from");

  reader.ensure_version_id("EventResponseHdlr", 2,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  rep->becomeInactive();

  fstring fmap;
  reader.read_value("feedbackMap", fmap);
  rep->itsFeedbackMap.set(fmap);

  reader.read_value("useFeedback", rep->itsFeedbackMap.itsUseFeedback);
  reader.read_value("eventSequence", rep->itsEventSequence);
  reader.read_value("bindingSubstitution", rep->itsBindingSubstitution);

  reader.read_owned_object("responseProc", rep->itsResponseProc);
}

void EventResponseHdlr::write_to(io::writer& writer) const
{
GVX_TRACE("EventResponseHdlr::write_to");

  writer.ensure_output_version_id("EventResponseHdlr", ERH_SVID, 2,
                              "Try groovx0.8a7", SRC_POS);

  writer.write_value("feedbackMap", rep->itsFeedbackMap.rep());
  writer.write_value("useFeedback", rep->itsFeedbackMap.itsUseFeedback);
  writer.write_value("eventSequence", rep->itsEventSequence);
  writer.write_value("bindingSubstitution", rep->itsBindingSubstitution);

  writer.write_owned_object("responseProc", rep->itsResponseProc);
}

// FIXME XML still need this?
void EventResponseHdlr::setInputResponseMap(const fstring& s)
{
GVX_TRACE("EventResponseHdlr::setInputResponseMap");

  const fstring args("inp");

  const fstring body =
    rutz::sfmt("set map {%s}\n"
               "foreach pair $map {\n"
               "  foreach {rx val} $pair {\n"
               "    if [regexp $rx $inp] { return $val }\n"
               "  }\n"
               "}\n"
               "return -1\n",
               s.c_str());

  setResponseProc(args, body);
}

bool EventResponseHdlr::getUseFeedback() const
{
GVX_TRACE("EventResponseHdlr::getUseFeedback");
  return rep->itsFeedbackMap.itsUseFeedback;
}

void EventResponseHdlr::setUseFeedback(bool val)
{
GVX_TRACE("EventResponseHdlr::setUseFeedback");
  rep->itsFeedbackMap.itsUseFeedback = val;
}

const char* EventResponseHdlr::getFeedbackMap() const
{
GVX_TRACE("EventResponseHdlr::getFeedbackMap");
  return rep->itsFeedbackMap.rep().c_str();
}

void EventResponseHdlr::setFeedbackMap(const char* feedback_string)
{
GVX_TRACE("EventResponseHdlr::setFeedbackMap");
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

void EventResponseHdlr::rhBeginTrial(nub::soft_ref<Toglet> widget,
                                     Trial& trial) const
{
  GVX_PRECONDITION( rep->isInactive() );

  rep->itsInterp.clear_event_queue();

  rep->becomeActive(widget, trial);

  GVX_POSTCONDITION( rep->isActive() );
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

  GVX_POSTCONDITION( rep->isInactive() );
}

void EventResponseHdlr::rhHaltExpt() const
{
  if ( rep->isActive() )
    {
      rep->itsState->rhHaltExpt();
      rep->becomeInactive();
    }

  GVX_POSTCONDITION( rep->isInactive() );
}

void EventResponseHdlr::rhAllowResponses(nub::soft_ref<Toglet> widget,
                                         Trial& trial) const
{
  rep->becomeActive(widget, trial);

  GVX_POSTCONDITION( rep->isActive() );
}

void EventResponseHdlr::rhDenyResponses() const
{
  rep->becomeInactive();

  GVX_POSTCONDITION( rep->isInactive() );
}

static const char __attribute__((used)) vcid_groovx_visx_eventresponsehdlr_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_EVENTRESPONSEHDLR_CC_UTC20050626084016_DEFINED
