///////////////////////////////////////////////////////////////////////
//
// trial.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Mar 12 17:43:21 1999
// written: Wed Dec  4 15:45:29 2002
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

#include "visx/block.h"
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
    ActiveState(Block* block, SoftRef<Toglet> widget,
                Ref<ResponseHandler> rh, Ref<TimingHdlr> th) :
      itsBlock(block),
      itsWidget(widget),
      itsRh(rh),
      itsTh(th)
    {
      Precondition(block != 0);
    }

    Block* itsBlock;
    SoftRef<Toglet> itsWidget;
    Ref<ResponseHandler> itsRh;
    Ref<TimingHdlr> itsTh;
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
    itsCorrectResponse(Response::ALWAYS_CORRECT),
    itsGxNodes(),
    itsCurrentNode(0),
    itsResponses(),
    itsType(-1),
    itsRh(),
    itsTh(),
    itsActiveState(0)
  {}

  int itsCorrectResponse;

  typedef minivec<Ref<GxNode> > GxNodes;
  GxNodes itsGxNodes;

  unsigned int itsCurrentNode;

  minivec<Response> itsResponses;

  int itsType;
  SoftRef<ResponseHandler> itsRh;
  SoftRef<TimingHdlr> itsTh;

  scoped_ptr<ActiveState> itsActiveState;

  bool isActive() const { return itsActiveState.get() != 0; }
  bool isInactive() const { return itsActiveState.get() == 0; }

  void becomeActive(Block* block, SoftRef<Toglet> widget)
  {
    itsActiveState.reset(new ActiveState(block, widget, itsRh, itsTh));
    Util::Log::addScope("Trial");
  }

  void becomeInactive()
  {
    Util::Log::removeScope("Trial");
    itsActiveState.reset(0);
  }

public:

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  fstring status() const;

  int lastResponse() const;
  void undoLastResponse();

  double avgResponse() const;
  double avgRespTime() const;

  void trNextNode() { setCurrentNode(itsCurrentNode+1); }

  void setCurrentNode(unsigned int nodeNumber)
  {
    if (nodeNumber >= itsGxNodes.size())
      {
        throw Util::Error(fstring("invalid node number ", nodeNumber));
      }
    itsCurrentNode = nodeNumber;
  }

  void clearObjs();

  void trDoTrial(Trial* self, const SoftRef<Toglet>& widget,
                 Util::ErrorHandler& errhdlr, Block& block);
  double trElapsedMsec();
  void trAbortTrial();
  void trEndTrial();
  void trNextTrial();
  void trHaltExpt();
  void trResponseSeen();
  void trRecordResponse(Response& response);
  void trAllowResponses(Trial* self);
  void trDenyResponses();

  void installSelf(SoftRef<Toglet> widget) const;
};

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void Trial::Impl::readFrom(IO::Reader* reader)
{
DOTRACE("Trial::Impl::readFrom");

  becomeInactive();

  reader->ensureReadVersionId("Trial", 4, "Try grsh0.8a4");

  itsGxNodes.clear();
  IO::ReadUtils::readObjectSeq<GxNode>(
          reader, "gxObjects", std::back_inserter(itsGxNodes));

  itsResponses.clear();
  IO::ReadUtils::readValueObjSeq<Response>(
          reader, "responses", std::back_inserter(itsResponses));

  reader->readValue("correctResponse", itsCorrectResponse);

  reader->readValue("type", itsType);

  itsRh = dynamicCast<ResponseHandler>(reader->readMaybeObject("rh"));
  itsTh = dynamicCast<TimingHdlr>(reader->readMaybeObject("th"));
}

void Trial::Impl::writeTo(IO::Writer* writer) const
{
DOTRACE("Trial::Impl::writeTo");

  writer->ensureWriteVersionId("Trial", TRIAL_SERIAL_VERSION_ID, 3,
                               "Try grsh0.8a3");

  IO::WriteUtils::writeObjectSeq(writer, "gxObjects",
                                 itsGxNodes.begin(), itsGxNodes.end());

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
                                   itsResponses.begin(), itsResponses.end());

  writer->writeValue("correctResponse", itsCorrectResponse);

  writer->writeValue("type", itsType);

  writer->writeObject("rh", itsRh);
  writer->writeObject("th", itsTh);
}

///////////////
// accessors //
///////////////

fstring Trial::Impl::status() const
{
DOTRACE("Trial::Impl::status");

  fstring objids;
  fstring cats;

  for (GxNodes::const_iterator
         ii = itsGxNodes.begin(),
         end = itsGxNodes.end();
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

  buf.append("trial type == ", itsType);
  buf.append(", objs ==", objids, ", categories == ", cats);

  return buf;
}

int Trial::Impl::lastResponse() const
{
DOTRACE("Trial::Impl::lastResponse");

  if (itsResponses.empty())
    throw Util::Error("the trial has no responses yet");

  return itsResponses.back().val();
}

double Trial::Impl::avgResponse() const
{
DOTRACE("Trial::Impl::avgResponse");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = itsResponses.begin();
       ii != itsResponses.end();
       ++ii)
    {
      sum += ii->val();
    }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

double Trial::Impl::avgRespTime() const
{
DOTRACE("Trial::Impl::avgRespTime");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = itsResponses.begin();
       ii != itsResponses.end();
       ++ii)
    {
      sum += ii->msec();

      dbgEval(3, sum);
      dbgEvalNL(3, sum/itsResponses.size());
    }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

//////////////////
// manipulators //
//////////////////

void Trial::Impl::clearObjs()
{
DOTRACE("Trial::Impl::clearObjs");
  itsGxNodes.clear();
}

/////////////
// actions //
/////////////

void Trial::Impl::trDoTrial(Trial* self, const SoftRef<Toglet>& widget,
                            Util::ErrorHandler& errhdlr, Block& block)
{
DOTRACE("Trial::Impl::trDoTrial");
  Precondition(self != 0);
  Precondition(widget.isValid());
  Precondition(&errhdlr != 0);
  Precondition(&block != 0);

  if ( itsRh.isInvalid() || itsTh.isInvalid() )
    {
      errhdlr.handleMsg("the trial did not have a valid timing handler "
                        "and response handler");
      return;
    }

  becomeActive(&block, widget);

  Util::log("trDoTrial");

  itsCurrentNode = 0;

  itsActiveState->itsRh->rhBeginTrial(widget, *self);
  itsActiveState->itsTh->thBeginTrial(*self, errhdlr);
}

double Trial::Impl::trElapsedMsec()
{
DOTRACE("Trial::Impl::trElapsedMsec");

  Precondition( isActive() );

  return itsActiveState->itsTh->getElapsedMsec();
}

void Trial::Impl::trAbortTrial()
{
DOTRACE("Trial::Impl::trAbortTrial");

  Precondition( isActive() );

  Util::log("trAbortTrial");

  itsActiveState->itsRh->rhAbortTrial();
  itsActiveState->itsTh->thAbortTrial();
  itsActiveState->itsBlock->abortTrial();
}

void Trial::Impl::trEndTrial()
{
DOTRACE("Trial::Impl::trEndTrial");

  Precondition( isActive() );

  Util::log("trEndTrial");

  itsActiveState->itsRh->rhEndTrial();
  itsActiveState->itsBlock->endTrial();
}

void Trial::Impl::trNextTrial()
{
DOTRACE("Trial::Impl::trNextTrial");

  Precondition( isActive() );

  Util::log("trNextTrial");

  Block* block = itsActiveState->itsBlock;

  becomeInactive();

  block->nextTrial();
}

void Trial::Impl::trHaltExpt()
{
DOTRACE("Trial::Impl::trHaltExpt");

  if (isInactive()) return;

  Util::log("trHaltExpt");

  if (itsActiveState->itsWidget.isValid())
    itsActiveState->itsWidget->undraw();

  itsActiveState->itsRh->rhHaltExpt();
  itsActiveState->itsTh->thHaltExpt();

  becomeInactive();
}

void Trial::Impl::trResponseSeen()
{
DOTRACE("Trial::Impl::trResponseSeen");

  Precondition( isActive() );

  Util::log("trResponseSeen");

  itsActiveState->itsTh->thResponseSeen();
}

void Trial::Impl::trRecordResponse(Response& response)
{
DOTRACE("Trial::Impl::trRecordResponse");

  Precondition( isActive() );

  Util::log("trRecordResponse");
  response.setCorrectVal(itsCorrectResponse);

  itsResponses.push_back(response);

  itsActiveState->itsBlock->processResponse(response);
}

void Trial::Impl::trAllowResponses(Trial* self)
{
DOTRACE("Trial::Impl::trAllowResponses");

  Precondition( isActive() );

  Util::log("trAllowResponses");

  itsActiveState->itsRh->rhAllowResponses(itsActiveState->itsWidget, *self);
}

void Trial::Impl::trDenyResponses()
{
DOTRACE("Trial::Impl::trDenyResponses");

  Precondition( isActive() );

  Util::log("trDenyResponses");

  itsActiveState->itsRh->rhDenyResponses();
}

void Trial::Impl::installSelf(SoftRef<Toglet> widget) const
{
DOTRACE("Trial::Impl::installSelf");

  if (itsCurrentNode < itsGxNodes.size())
    widget->setDrawable(Ref<GxNode>(itsGxNodes[itsCurrentNode]));
}

void Trial::Impl::undoLastResponse()
{
DOTRACE("Trial::Impl::undoLastResponse");
  if ( !itsResponses.empty() )
    itsResponses.pop_back();
}


///////////////////////////////////////////////////////////////////////
//
// Trial member functions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const Field FIELD_ARRAY[] =
  {
    Field("tType", make_mypair(&Trial::trialType, &Trial::setType),
          -1, -10, 10, 1, Field::NEW_GROUP)
  };

  FieldMap TRIAL_FIELDS(FIELD_ARRAY);
}

//////////////
// creators //
//////////////

const FieldMap& Trial::classFields() { return TRIAL_FIELDS; }

Trial* Trial::make()
{
DOTRACE("Trial::make");
  return new Trial;
}

Trial::Trial() :
  FieldContainer(0),
  itsImpl( new Impl(this) )
{
DOTRACE("Trial::Trial()");

  setFieldMap(TRIAL_FIELDS);
}

Trial::~Trial()
{
DOTRACE("Trial::~Trial");
  delete itsImpl;
}

////////////////////////////////
// delegations to Trial::Impl //
////////////////////////////////

IO::VersionId Trial::serialVersionId() const
  { return TRIAL_SERIAL_VERSION_ID; }

void Trial::readFrom(IO::Reader* reader)
  { itsImpl->readFrom(reader); }

void Trial::writeTo(IO::Writer* writer) const
  { itsImpl->writeTo(writer); }

SoftRef<Toglet> Trial::getWidget() const
{
  Precondition( itsImpl->isActive() );
  return itsImpl->itsActiveState->itsWidget;
}

int Trial::getCorrectResponse() const
  { return itsImpl->itsCorrectResponse; }

void Trial::setCorrectResponse(int response)
  { itsImpl->itsCorrectResponse = response; }


Ref<ResponseHandler> Trial::getResponseHandler() const
  { return Ref<ResponseHandler>(itsImpl->itsRh); }

void Trial::setResponseHandler(Ref<ResponseHandler> rh)
  { itsImpl->itsRh = SoftRef<ResponseHandler>(rh); }


Ref<TimingHdlr> Trial::getTimingHdlr() const
  { return Ref<TimingHdlr>(itsImpl->itsTh); }

void Trial::setTimingHdlr(Ref<TimingHdlr> th)
  { itsImpl->itsTh = SoftRef<TimingHdlr>(th); }


int Trial::trialType() const
  { return itsImpl->itsType; }

void Trial::setType(int t)
  { itsImpl->itsType = t; }


fstring Trial::status() const
  { return itsImpl->status(); }

int Trial::lastResponse() const
  { return itsImpl->lastResponse(); }

void Trial::undoLastResponse()
  { itsImpl->undoLastResponse(); }

unsigned int Trial::numResponses() const
  { return itsImpl->itsResponses.size(); }

void Trial::clearResponses()
  { itsImpl->itsResponses.clear(); }

double Trial::avgResponse() const
  { return itsImpl->avgResponse(); }

double Trial::avgRespTime() const
  { return itsImpl->avgRespTime(); }


void Trial::addNode(Util::Ref<GxNode> item)
  { itsImpl->itsGxNodes.push_back(item); }

void Trial::trNextNode()
  { itsImpl->trNextNode(); }

Util::FwdIter<Util::Ref<GxNode> > Trial::nodes() const
{
  return Util::FwdIter<Util::Ref<GxNode> >
    (itsImpl->itsGxNodes.begin(),
     itsImpl->itsGxNodes.end());
}

unsigned int Trial::getCurrentNode() const
  { return itsImpl->itsCurrentNode; }

void Trial::setCurrentNode(unsigned int nodeNumber)
  { itsImpl->setCurrentNode(nodeNumber); }

void Trial::clearObjs()
  { itsImpl->clearObjs(); }


void Trial::trDoTrial(const SoftRef<Toglet>& widget,
                      Util::ErrorHandler& errhdlr, Block& block)
  { itsImpl->trDoTrial(this, widget, errhdlr, block); }

double Trial::trElapsedMsec()
  { return itsImpl->trElapsedMsec(); }

void Trial::trAbortTrial()
  { itsImpl->trAbortTrial(); }

void Trial::trEndTrial()
  { itsImpl->trEndTrial(); }

void Trial::trNextTrial()
  { itsImpl->trNextTrial(); }

void Trial::trHaltExpt()
  { itsImpl->trHaltExpt(); }

void Trial::trResponseSeen()
  { itsImpl->trResponseSeen(); }

void Trial::trRecordResponse(Response& response)
  { itsImpl->trRecordResponse(response); }

void Trial::trAllowResponses()
  { itsImpl->trAllowResponses(this); }

void Trial::trDenyResponses()
  { itsImpl->trDenyResponses(); }

void Trial::installSelf(SoftRef<Toglet> widget) const
  { itsImpl->installSelf(widget); }

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
