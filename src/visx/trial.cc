///////////////////////////////////////////////////////////////////////
//
// trial.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Mar 12 17:43:21 1999
// written: Thu Dec  5 14:17:16 2002
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
    ActiveState(Element* parent, SoftRef<Toglet> widget,
                Ref<ResponseHandler> rh, Ref<TimingHdlr> th) :
      itsParent(parent),
      itsWidget(widget),
      itsRh(rh),
      itsTh(th)
    {
      Precondition(parent != 0);
    }

    Element* itsParent;
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

  void becomeActive(Element* parent, SoftRef<Toglet> widget)
  {
    itsActiveState.reset(new ActiveState(parent, widget, itsRh, itsTh));
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
  void vxUndo();

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

  void vxRun(Trial* self, Element& parent);
  double trElapsedMsec();
  void vxAbort();
  void vxEndTrial();
  void vxNext();
  void vxHalt();
  void trResponseSeen();
  void vxProcessResponse(Response& response);
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

void Trial::Impl::vxRun(Trial* self, Element& parent)
{
DOTRACE("Trial::Impl::vxRun");

  Precondition(self != 0);
  Precondition(&parent != 0);

  SoftRef<Toglet> widget = parent.getWidget();
  Util::ErrorHandler& errhdlr = parent.getErrorHandler();

  Precondition(widget.isValid());
  Precondition(&errhdlr != 0);

  if ( itsRh.isInvalid() || itsTh.isInvalid() )
    {
      errhdlr.handleMsg("the trial did not have a valid timing handler "
                        "and response handler");
      return;
    }

  becomeActive(&parent, widget);

  Util::log("Trial::vxRun");

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

void Trial::Impl::vxAbort()
{
DOTRACE("Trial::Impl::vxAbort");

  Precondition( isActive() );

  Util::log("vxAbort");

  itsActiveState->itsRh->rhAbortTrial();
  itsActiveState->itsTh->thAbortTrial();
  itsActiveState->itsParent->vxAbort();
}

void Trial::Impl::vxEndTrial()
{
DOTRACE("Trial::Impl::vxEndTrial");

  Precondition( isActive() );

  Util::log("Trial::vxEndTrial");

  itsActiveState->itsRh->rhEndTrial();
  itsActiveState->itsParent->vxEndTrial();
}

void Trial::Impl::vxNext()
{
DOTRACE("Trial::Impl::vxNext");

  Precondition( isActive() );

  Util::log("Trial::vxNext");

  Element* parent = itsActiveState->itsParent;

  becomeInactive();

  parent->vxNext();
}

void Trial::Impl::vxHalt()
{
DOTRACE("Trial::Impl::vxHalt");

  if (isInactive()) return;

  Util::log("Trial::vxHalt");

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

void Trial::Impl::vxProcessResponse(Response& response)
{
DOTRACE("Trial::Impl::vxProcessResponse");

  Precondition( isActive() );

  Util::log("Trial::vxProcessResponse");
  response.setCorrectVal(itsCorrectResponse);

  itsResponses.push_back(response);

  itsActiveState->itsParent->vxProcessResponse(response);
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

void Trial::Impl::vxUndo()
{
DOTRACE("Trial::Impl::vxUndo");
  if ( !itsResponses.empty() )
    itsResponses.pop_back();
}

void Trial::vxReset()
{
DOTRACE("Trial::vxReset");
  rep->itsResponses.clear();
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
  rep( new Impl(this) )
{
DOTRACE("Trial::Trial()");

  setFieldMap(TRIAL_FIELDS);
}

Trial::~Trial()
{
DOTRACE("Trial::~Trial");
  delete rep;
}

////////////////////////////////
// delegations to Trial::Impl //
////////////////////////////////

IO::VersionId Trial::serialVersionId() const
  { return TRIAL_SERIAL_VERSION_ID; }

void Trial::readFrom(IO::Reader* reader)
  { rep->readFrom(reader); }

void Trial::writeTo(IO::Writer* writer) const
  { rep->writeTo(writer); }

Util::ErrorHandler& Trial::getErrorHandler() const
{
  Precondition( rep->isActive() );
  return rep->itsActiveState->itsParent->getErrorHandler();
}

const SoftRef<Toglet>& Trial::getWidget() const
{
  Precondition( rep->isActive() );
  return rep->itsActiveState->itsWidget;
}

int Trial::getCorrectResponse() const
  { return rep->itsCorrectResponse; }

void Trial::setCorrectResponse(int response)
  { rep->itsCorrectResponse = response; }


Ref<ResponseHandler> Trial::getResponseHandler() const
  { return Ref<ResponseHandler>(rep->itsRh); }

void Trial::setResponseHandler(Ref<ResponseHandler> rh)
  { rep->itsRh = SoftRef<ResponseHandler>(rh); }


Ref<TimingHdlr> Trial::getTimingHdlr() const
  { return Ref<TimingHdlr>(rep->itsTh); }

void Trial::setTimingHdlr(Ref<TimingHdlr> th)
  { rep->itsTh = SoftRef<TimingHdlr>(th); }


int Trial::trialType() const
  { return rep->itsType; }

void Trial::setType(int t)
  { rep->itsType = t; }


fstring Trial::status() const
  { return rep->status(); }

int Trial::lastResponse() const
  { return rep->lastResponse(); }

void Trial::vxUndo()
  { rep->vxUndo(); }

unsigned int Trial::numResponses() const
  { return rep->itsResponses.size(); }

void Trial::clearResponses()
  { rep->itsResponses.clear(); }

double Trial::avgResponse() const
  { return rep->avgResponse(); }

double Trial::avgRespTime() const
  { return rep->avgRespTime(); }


void Trial::addNode(Util::Ref<GxNode> item)
  { rep->itsGxNodes.push_back(item); }

void Trial::trNextNode()
  { rep->trNextNode(); }

Util::FwdIter<Util::Ref<GxNode> > Trial::nodes() const
{
  return Util::FwdIter<Util::Ref<GxNode> >
    (rep->itsGxNodes.begin(),
     rep->itsGxNodes.end());
}

unsigned int Trial::getCurrentNode() const
  { return rep->itsCurrentNode; }

void Trial::setCurrentNode(unsigned int nodeNumber)
  { rep->setCurrentNode(nodeNumber); }

void Trial::clearObjs()
  { rep->clearObjs(); }


void Trial::vxRun(Element& parent)
  { rep->vxRun(this, parent); }

double Trial::trElapsedMsec()
  { return rep->trElapsedMsec(); }

void Trial::vxAbort()
  { rep->vxAbort(); }

void Trial::vxEndTrial()
  { rep->vxEndTrial(); }

void Trial::vxNext()
  { rep->vxNext(); }

void Trial::vxHalt() const
  { rep->vxHalt(); }

void Trial::trResponseSeen()
  { rep->trResponseSeen(); }

void Trial::vxProcessResponse(Response& response)
  { rep->vxProcessResponse(response); }

void Trial::trAllowResponses()
  { rep->trAllowResponses(this); }

void Trial::trDenyResponses()
  { rep->trDenyResponses(); }

void Trial::installSelf(SoftRef<Toglet> widget) const
  { rep->installSelf(widget); }

int Trial::numCompleted() const
  { return rep->itsResponses.size(); }

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
