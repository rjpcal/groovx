///////////////////////////////////////////////////////////////////////
//
// trial.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Mar 12 17:43:21 1999
// written: Sun Dec  8 15:23:37 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "visx/trial.h"

#include "gfx/gxshapekit.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "tcl/toglet.h"

#include "util/errorhandler.h"
#include "util/iter.h"
#include "util/log.h"
#include "util/minivec.h"
#include "util/ref.h"
#include "util/strings.h"

#include "visx/response.h"
#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"

#define DYNAMIC_TRACE_EXPR Trial::tracer.status()
#include "util/trace.h"
#include "util/debug.h"

Util::Tracer Trial::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId TRIAL_SERIAL_VERSION_ID = 4;

  struct ActiveState
  {
    ActiveState(Element* p, SoftRef<Toglet> w,
                Ref<ResponseHandler> r, Ref<TimingHdlr> t) :
      parent(p),
      widget(w),
      rh(r),
      th(t),
      status(Element::CHILD_OK)
    {
      Precondition(parent != 0);
      Util::Log::addScope("Trial");
    }

    ~ActiveState()
    {
      Util::Log::removeScope("Trial");
    }

    Element* parent;
    SoftRef<Toglet> widget;
    Ref<ResponseHandler> rh;
    Ref<TimingHdlr> th;
    Element::ChildStatus status;
  };
}

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Trial::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Trial*) :
    correctResponse(Response::ALWAYS_CORRECT),
    gxNodes(),
    currentNode(0),
    responses(),
    trialType(-1),
    rh(),
    th(),
    activeState(0)
  {}

  int correctResponse;

  typedef minivec<Ref<GxNode> > GxNodes;
  GxNodes gxNodes;

  unsigned int currentNode;

  minivec<Response> responses;

  int trialType;
  SoftRef<ResponseHandler> rh;
  SoftRef<TimingHdlr> th;

  scoped_ptr<ActiveState> activeState;

  bool isActive() const { return activeState.get() != 0; }

  void becomeActive(Element* parent, SoftRef<Toglet> widget)
  {
    activeState.reset(new ActiveState(parent, widget, rh, th));
  }

  void becomeInactive()
  {
    activeState.reset(0);
  }
};

///////////////////////////////////////////////////////////////////////
//
// Trial member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

const FieldMap& Trial::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("tType", make_mypair(&Trial::trialType, &Trial::setType),
          -1, -10, 10, 1, Field::NEW_GROUP)
  };

  static FieldMap TRIAL_FIELDS(FIELD_ARRAY);

  return TRIAL_FIELDS;
}

Trial* Trial::make()
{
DOTRACE("Trial::make");
  return new Trial;
}

Trial::Trial() :
  FieldContainer(0),
  rep( new Impl(this) )
{
DOTRACE("Trial::Trial()");

  setFieldMap(Trial::classFields());
}

Trial::~Trial()
{
DOTRACE("Trial::~Trial");
  delete rep;
}

IO::VersionId Trial::serialVersionId() const
  { return TRIAL_SERIAL_VERSION_ID; }

void Trial::readFrom(IO::Reader* reader)
{
DOTRACE("Trial::readFrom");

  rep->becomeInactive();

  reader->ensureReadVersionId("Trial", 4, "Try grsh0.8a4");

  rep->gxNodes.clear();
  IO::ReadUtils::readObjectSeq<GxNode>
    (reader, "gxObjects", std::back_inserter(rep->gxNodes));

  rep->responses.clear();
  IO::ReadUtils::readValueObjSeq<Response>
    (reader, "responses", std::back_inserter(rep->responses));

  reader->readValue("correctResponse", rep->correctResponse);

  reader->readValue("type", rep->trialType);

  rep->rh = dynamicCast<ResponseHandler>(reader->readMaybeObject("rh"));
  rep->th = dynamicCast<TimingHdlr>(reader->readMaybeObject("th"));
}

void Trial::writeTo(IO::Writer* writer) const
{
DOTRACE("Trial::writeTo");

  writer->ensureWriteVersionId("Trial", TRIAL_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a3");

  IO::WriteUtils::writeObjectSeq(writer, "gxObjects",
                                 rep->gxNodes.begin(), rep->gxNodes.end());

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
                                   rep->responses.begin(), rep->responses.end());

  writer->writeValue("correctResponse", rep->correctResponse);

  writer->writeValue("type", rep->trialType);

  writer->writeObject("rh", rep->rh);
  writer->writeObject("th", rep->th);
}

Util::ErrorHandler& Trial::getErrorHandler() const
{
  Precondition( rep->isActive() );
  return rep->activeState->parent->getErrorHandler();
}

const SoftRef<Toglet>& Trial::getWidget() const
{
  Precondition( rep->isActive() );
  return rep->activeState->widget;
}

int Trial::getCorrectResponse() const
  { return rep->correctResponse; }

void Trial::setCorrectResponse(int response)
  { rep->correctResponse = response; }


Ref<ResponseHandler> Trial::getResponseHandler() const
  { return Ref<ResponseHandler>(rep->rh); }

void Trial::setResponseHandler(Ref<ResponseHandler> rh)
  { rep->rh = SoftRef<ResponseHandler>(rh); }


Ref<TimingHdlr> Trial::getTimingHdlr() const
  { return Ref<TimingHdlr>(rep->th); }

void Trial::setTimingHdlr(Ref<TimingHdlr> th)
  { rep->th = SoftRef<TimingHdlr>(th); }


int Trial::trialType() const
  { return rep->trialType; }

void Trial::setType(int t)
  { rep->trialType = t; }


fstring Trial::status() const
{
DOTRACE("Trial::status");

  fstring objids;
  fstring cats;

  for (Impl::GxNodes::const_iterator
         ii = rep->gxNodes.begin(),
         end = rep->gxNodes.end();
       ii != end;
       ++ii)
    {
      for (Util::FwdIter<const Util::Ref<GxNode> > tr((*ii)->deepChildren());
           tr.isValid();
           tr.next())
        {
          const GxShapeKit* g = dynamic_cast<const GxShapeKit*>((*tr).get());
          if (g)
            {
              objids.append(" ", g->id());
              cats.append(" ", g->category());
            }
        }
    }

  fstring buf;

  buf.append("trial type == ", rep->trialType);
  buf.append(", objs ==", objids, ", categories == ", cats);

  return buf;
}

int Trial::lastResponse() const
{
DOTRACE("Trial::lastResponse");

  if (rep->responses.empty())
    throw Util::Error("the trial has no responses yet");

  return rep->responses.back().val();
}

void Trial::vxUndo()
{
DOTRACE("Trial::vxUndo");
  if ( !rep->responses.empty() )
    rep->responses.pop_back();
}

unsigned int Trial::numResponses() const
  { return rep->responses.size(); }

void Trial::clearResponses()
  { rep->responses.clear(); }

double Trial::avgResponse() const
{
DOTRACE("Trial::avgResponse");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = rep->responses.begin();
       ii != rep->responses.end();
       ++ii)
    {
      sum += ii->val();
    }
  return (rep->responses.size() > 0) ? double(sum)/rep->responses.size() : 0.0;
}

double Trial::avgRespTime() const
{
DOTRACE("Trial::avgRespTime");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = rep->responses.begin();
       ii != rep->responses.end();
       ++ii)
    {
      sum += ii->msec();

      dbgEval(3, sum);
      dbgEvalNL(3, sum/rep->responses.size());
    }
  return (rep->responses.size() > 0) ? double(sum)/rep->responses.size() : 0.0;
}


void Trial::addNode(Util::Ref<GxNode> item)
{
DOTRACE("Trial::addNode");
  rep->gxNodes.push_back(item);
}

void Trial::trNextNode()
{
DOTRACE("Trial::trNextNode");
  setCurrentNode(rep->currentNode+1);
}

Util::FwdIter<Util::Ref<GxNode> > Trial::nodes() const
{
DOTRACE("Trial::nodes");
  return Util::FwdIter<Util::Ref<GxNode> >
    (rep->gxNodes.begin(), rep->gxNodes.end());
}

unsigned int Trial::getCurrentNode() const
{
DOTRACE("Trial::getCurrentNode");
  return rep->currentNode;
}

void Trial::setCurrentNode(unsigned int nodeNumber)
{
DOTRACE("Trial::setCurrentNode");
  if (nodeNumber >= rep->gxNodes.size())
    {
      throw Util::Error(fstring("invalid node number ", nodeNumber));
    }
  rep->currentNode = nodeNumber;
}

void Trial::clearObjs()
{
DOTRACE("Trial::clearObjs");
  rep->gxNodes.clear();
}


void Trial::vxRun(Element& parent)
{
DOTRACE("Trial::vxRun");

  Precondition(&parent != 0);

  SoftRef<Toglet> widget = parent.getWidget();
  Util::ErrorHandler& errhdlr = parent.getErrorHandler();

  Precondition(widget.isValid());
  Precondition(&errhdlr != 0);

  if ( rep->rh.isInvalid() || rep->th.isInvalid() )
    {
      errhdlr.handleMsg("the trial did not have a valid timing handler "
                        "and response handler");
      return;
    }

  rep->becomeActive(&parent, widget);

  Util::log("Trial::vxRun");

  rep->currentNode = 0;

  rep->activeState->rh->rhBeginTrial(widget, *this);
  rep->activeState->th->thBeginTrial(*this, errhdlr);
}

double Trial::trElapsedMsec()
{
DOTRACE("Trial::trElapsedMsec");

  Precondition( rep->isActive() );

  return rep->activeState->th->getElapsedMsec();
}

void Trial::trAbort()
{
DOTRACE("Trial::trAbort");

  Precondition( rep->isActive() );

  Util::log("trAbort");

  rep->activeState->status = CHILD_ABORTED;
  rep->activeState->rh->rhAbortTrial();
  rep->activeState->th->thAbortTrial();
}

void Trial::vxReturn(ChildStatus /*s*/)
{
  // FIXME
  Assert(false);
}

void Trial::trEndTrial()
{
DOTRACE("Trial::trEndTrial");

  Precondition( rep->isActive() );

  Util::log("Trial::trEndTrial");

  rep->activeState->rh->rhEndTrial();
  rep->activeState->parent->vxEndTrialHook();

  Element* parent = rep->activeState->parent;
  Element::ChildStatus status = rep->activeState->status;

  rep->becomeInactive();

  parent->vxReturn(status);
}

void Trial::vxHalt() const
{
DOTRACE("Trial::vxHalt");

  if (!rep->isActive()) return;

  Util::log("Trial::vxHalt");

  if (rep->activeState->widget.isValid())
    rep->activeState->widget->undraw();

  rep->activeState->rh->rhHaltExpt();
  rep->activeState->th->thHaltExpt();

  rep->becomeInactive();
}

void Trial::vxReset()
{
DOTRACE("Trial::vxReset");
  rep->responses.clear();
}

void Trial::trResponseSeen()
{
DOTRACE("Trial::trResponseSeen");

  Precondition( rep->isActive() );

  Util::log("trResponseSeen");

  rep->activeState->th->thResponseSeen();
}

void Trial::trProcessResponse(Response& response)
{
DOTRACE("Trial::trProcessResponse");

  Precondition( rep->isActive() );

  Util::log("trProcessResponse");
  response.setCorrectVal(rep->correctResponse);

  rep->responses.push_back(response);

  dbgEval(3, response.correctVal());
  dbgEvalNL(3, response.val());

  // If the response was incorrect, we'll ask our parent container to
  // kindly repeat this trial
  if (!response.isCorrect())
    {
      rep->activeState->status = CHILD_REPEAT;
    }
}

void Trial::trAllowResponses()
{
DOTRACE("Trial::trAllowResponses");

  Precondition( rep->isActive() );

  Util::log("trAllowResponses");

  rep->activeState->rh->rhAllowResponses
    (rep->activeState->widget, *this);
}

void Trial::trDenyResponses()
{
DOTRACE("Trial::trDenyResponses");

  Precondition( rep->isActive() );

  Util::log("trDenyResponses");

  rep->activeState->rh->rhDenyResponses();
}

void Trial::installSelf(SoftRef<Toglet> widget) const
{
DOTRACE("Trial::installSelf");

  if (rep->currentNode < rep->gxNodes.size())
    widget->setDrawable(Ref<GxNode>(rep->gxNodes[rep->currentNode]));
}

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
