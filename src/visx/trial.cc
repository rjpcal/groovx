///////////////////////////////////////////////////////////////////////
//
// trial.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Mar 12 17:43:21 1999
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

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include "gfx/canvas.h"
#include "gfx/gxshapekit.h"
#include "gfx/toglet.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "util/fstring.h"
#include "util/iter.h"
#include "util/scopedptr.h"

#include "visx/response.h"
#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"

#include <vector>

#define DYNAMIC_TRACE_EXPR Trial::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::fstring;

using Nub::Ref;
using Nub::SoftRef;

rutz::tracer Trial::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId TRIAL_SVID = 5;

  struct ActiveState
  {
    ActiveState(Trial* tr,
                Element* p, SoftRef<Toglet> w,
                Ref<ResponseHandler> r, Ref<TimingHdlr> t) :
      trial(tr),
      parent(p),
      widget(w),
      rh(r),
      th(t),
      status(Element::CHILD_OK)
    {
      PRECONDITION(parent != 0);
      Nub::Log::addObjScope(*trial);
    }

    ~ActiveState()
    {
      Nub::Log::removeObjScope(*trial);
    }

    Trial* trial;
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
  Impl(Trial* t) :
    owner(t),
    correctResponse(Response::ALWAYS_CORRECT),
    gxNodes(),
    currentNode(0),
    responses(),
    trialType(-1),
    rh(),
    th(),
    info(),
    activeState(0)
  {}

  Trial* owner;

  int correctResponse;

  typedef std::vector<Ref<GxNode> > GxNodes;
  GxNodes gxNodes;

  unsigned int currentNode;

  std::vector<Response> responses;

  int trialType;
  SoftRef<ResponseHandler> rh;
  SoftRef<TimingHdlr> th;

  fstring info;

  rutz::scoped_ptr<ActiveState> activeState;

  bool isActive() const { return activeState.get() != 0; }

  void becomeActive(Element* parent, SoftRef<Toglet> widget)
  {
    activeState.reset(new ActiveState(owner, parent, widget, rh, th));
  }

  void becomeInactive()
  {
    activeState.reset(0);
  }

  void installCurrentNode() const
  {
    PRECONDITION( isActive() );
    if (activeState->widget.isValid() && currentNode < gxNodes.size())
      {
        activeState->widget->setDrawable(gxNodes[currentNode]);
        Nub::log(fstring("current node is ",
                         gxNodes[currentNode]->uniqueName()));
      }
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
    Field("tType", &Trial::trialType, &Trial::setType,
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

Trial::~Trial() throw()
{
DOTRACE("Trial::~Trial");
  delete rep;
}

IO::VersionId Trial::serialVersionId() const
  { return TRIAL_SVID; }

void Trial::readFrom(IO::Reader& reader)
{
DOTRACE("Trial::readFrom");

  rep->becomeInactive();

  reader.ensureReadVersionId("Trial", 5,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  rep->gxNodes.clear();
  IO::ReadUtils::readObjectSeq<GxNode>
    (reader, "gxObjects", std::back_inserter(rep->gxNodes));

  rep->responses.clear();
  IO::ReadUtils::readValueObjSeq<Response>
    (reader, "responses", std::back_inserter(rep->responses));

  reader.readValue("correctResponse", rep->correctResponse);

  reader.readValue("type", rep->trialType);

  rep->rh = dynamicCast<ResponseHandler>(reader.readMaybeObject("rh"));
  rep->th = dynamicCast<TimingHdlr>(reader.readMaybeObject("th"));

  reader.readValue("info", rep->info);
}

void Trial::writeTo(IO::Writer& writer) const
{
DOTRACE("Trial::writeTo");

  writer.ensureWriteVersionId("Trial", TRIAL_SVID, 5,
                              "Try groovx0.8a3", SRC_POS);

  IO::WriteUtils::writeObjectSeq(writer, "gxObjects",
                                 rep->gxNodes.begin(),
                                 rep->gxNodes.end());

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
                                   rep->responses.begin(),
                                   rep->responses.end());

  writer.writeValue("correctResponse", rep->correctResponse);

  writer.writeValue("type", rep->trialType);

  writer.writeObject("rh", rep->rh);
  writer.writeObject("th", rep->th);

  writer.writeValue("info", rep->info);
}

const SoftRef<Toglet>& Trial::getWidget() const
{
  PRECONDITION( rep->isActive() );
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


fstring Trial::vxInfo() const
{
DOTRACE("Trial::vxInfo");
  if (rep->info.is_empty())
    return stdInfo();

  return rep->info;
}

int Trial::lastResponse() const
{
DOTRACE("Trial::lastResponse");

  if (rep->responses.empty())
    throw rutz::error("the trial has no responses yet", SRC_POS);

  return rep->responses.back().val();
}

void Trial::vxUndo()
{
DOTRACE("Trial::vxUndo");
  if ( !rep->responses.empty() )
    rep->responses.pop_back();
}

rutz::fwd_iter<Response> Trial::responses() const
{
DOTRACE("Trial::responses");
  return rutz::fwd_iter<Response>
    (rep->responses.begin(), rep->responses.end());
}

unsigned int Trial::numResponses() const
  { return rep->responses.size(); }

void Trial::clearResponses()
  { rep->responses.clear(); }

double Trial::avgResponse() const
{
DOTRACE("Trial::avgResponse");
  int sum = 0;
  for (std::vector<Response>::const_iterator ii = rep->responses.begin();
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
  for (std::vector<Response>::const_iterator ii = rep->responses.begin();
       ii != rep->responses.end();
       ++ii)
    {
      sum += ii->msec();

      dbg_eval(3, sum);
      dbg_eval_nl(3, sum/rep->responses.size());
    }
  return (rep->responses.size() > 0) ? double(sum)/rep->responses.size() : 0.0;
}


void Trial::addNode(Ref<GxNode> item)
{
DOTRACE("Trial::addNode");
  rep->gxNodes.push_back(item);
}

void Trial::trNextNode()
{
DOTRACE("Trial::trNextNode");
  setCurrentNode(rep->currentNode+1);
}

rutz::fwd_iter<Ref<GxNode> > Trial::nodes() const
{
DOTRACE("Trial::nodes");
  return rutz::fwd_iter<Ref<GxNode> >
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
      throw rutz::error(fstring("invalid node number ", nodeNumber),
                        SRC_POS);
    }
  rep->currentNode = nodeNumber;
}

void Trial::clearObjs()
{
DOTRACE("Trial::clearObjs");
  rep->gxNodes.clear();
}

fstring Trial::stdInfo() const
{
DOTRACE("Trial::stdInfo");

  fstring objids;
  fstring cats;

  for (Impl::GxNodes::const_iterator
         ii = rep->gxNodes.begin(),
         end = rep->gxNodes.end();
       ii != end;
       ++ii)
    {
      for (rutz::fwd_iter<const Ref<GxNode> > tr((*ii)->deepChildren());
           tr.is_valid();
           tr.next())
        {
          const GxShapeKit* g =
            dynamic_cast<const GxShapeKit*>((*tr).get());

          if (g)
            {
              objids.append(" ", g->uniqueName());
              cats.append(" ", g->category());
            }
        }
    }

  fstring buf;

  buf.append("trial type == ", rep->trialType);
  buf.append(", objs ==", objids, ", categories == ", cats);

  return buf;
}

void Trial::setInfo(fstring info)
{
DOTRACE("Trial::setInfo");
  rep->info = info;
}


void Trial::vxRun(Element& parent)
{
DOTRACE("Trial::vxRun");

  PRECONDITION(&parent != 0);

  SoftRef<Toglet> widget = parent.getWidget();

  PRECONDITION(widget.isValid());

  if ( rep->rh.isInvalid() || rep->th.isInvalid() )
    {
      throw rutz::error("the trial did not have a valid timing handler "
                        "and response handler", SRC_POS);
    }

  rep->becomeActive(&parent, widget);

  Nub::log( vxInfo() );

  rep->currentNode = 0;

  rep->activeState->rh->rhBeginTrial(widget, *this);
  rep->activeState->th->thBeginTrial(*this);
}

double Trial::trElapsedMsec()
{
DOTRACE("Trial::trElapsedMsec");

  PRECONDITION( rep->isActive() );

  return rep->activeState->th->getElapsedMsec();
}

void Trial::trAbortTrial()
{
DOTRACE("Trial::trAbortTrial");

  PRECONDITION( rep->isActive() );

  Nub::log("trAbortTrial");

  rep->activeState->status = CHILD_ABORTED;
  rep->activeState->rh->rhAbortTrial();
  rep->activeState->th->thAbortTrial();
}

void Trial::vxReturn(ChildStatus /*s*/)
{
  // FIXME
  ASSERT(false);
}

void Trial::trEndTrial()
{
DOTRACE("Trial::trEndTrial");

  PRECONDITION( rep->isActive() );

  Nub::log("Trial::trEndTrial");

  rep->activeState->rh->rhEndTrial();
  rep->activeState->th->thEndTrial();
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

  Nub::log("Trial::vxHalt");

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

void Trial::trProcessResponse(Response& response)
{
DOTRACE("Trial::trProcessResponse");

  PRECONDITION( rep->isActive() );

  Nub::log("trProcessResponse");

  rep->responses.push_back(response);

  dbg_eval(3, rep->correctResponse);
  dbg_eval_nl(3, response.val());

  rep->activeState->th->thResponseSeen();

  // If the response was incorrect, we'll ask our parent container to
  // kindly repeat this trial
  if (!response.matchesCorrectValue(rep->correctResponse))
    {
      rep->activeState->status = CHILD_REPEAT;
    }
}

void Trial::trDraw()
{
DOTRACE("Trial::trDraw");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    {
      rep->installCurrentNode();
      widget->setVisibility(true);
      widget->fullRender();
    }
}

void Trial::trRender()
{
DOTRACE("Trial::trRender");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    {
      rep->installCurrentNode();
      widget->setVisibility(true);
      widget->render();
    }
}

void Trial::trUndraw()
{
DOTRACE("Trial::trUndraw");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->undraw();
}

void Trial::trSwapBuffers()
{
DOTRACE("Trial::trSwapBuffers");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->swapBuffers();
}

void Trial::trRenderBack()
{
DOTRACE("Trial::trRenderBack");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->getCanvas()->drawOnBackBuffer();
}

void Trial::trRenderFront()
{
DOTRACE("Trial::trRenderFront");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->getCanvas()->drawOnFrontBuffer();
}

void Trial::trClearBuffer()
{
DOTRACE("Trial::trClearBuffer");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->clearscreen();
}

void Trial::trFinishDrawing()
{
DOTRACE("Trial::trFinishDrawing");
  SoftRef<Toglet> widget = getWidget();
  if (widget.isValid())
    widget->getCanvas()->finishDrawing();
}


void Trial::trAllowResponses()
{
DOTRACE("Trial::trAllowResponses");

  PRECONDITION( rep->isActive() );

  Nub::log("trAllowResponses");

  rep->activeState->rh->rhAllowResponses
    (rep->activeState->widget, *this);
}

void Trial::trDenyResponses()
{
DOTRACE("Trial::trDenyResponses");

  PRECONDITION( rep->isActive() );

  Nub::log("trDenyResponses");

  rep->activeState->rh->rhDenyResponses();
}

static const char vcid_trial_cc[] = "$Id$ $URL$";
#endif // !TRIAL_CC_DEFINED
