/** @file visx/trial.cc run a series of timed events and collect user
    responses in a psychophysics experiment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Mar 12 17:43:21 1999
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

#include "trial.h"

#include "gfx/canvas.h"
#include "gfx/gxshapekit.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "rutz/fstring.h"
#include "rutz/iter.h"
#include "rutz/sfmt.h"

#include "tcl-gfx/toglet.h"

#include "visx/response.h"
#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"

#include <memory>
#include <sstream>
#include <vector>

#define GVX_TRACE_EXPR Trial::tracer.status()
#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

using nub::ref;
using nub::soft_ref;

rutz::tracer Trial::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace dummy_namespace_to_avoid_gcc411_bug_trial_cc
{
  const io::version_id TRIAL_SVID = 5;

  struct ActiveState
  {
    ActiveState(Trial* tr,
                Element* p, soft_ref<Toglet> w,
                ref<ResponseHandler> r, ref<TimingHdlr> t) :
      trial(tr),
      parent(p),
      widget(w),
      rh(r),
      th(t),
      status(Element::ChildStatus::OK)
    {
      GVX_PRECONDITION(parent != nullptr);
      nub::logging::add_obj_scope(*trial);
    }

    ~ActiveState()
    {
      nub::logging::remove_obj_scope(*trial);
    }

    Trial* trial;
    Element* parent;
    soft_ref<Toglet> widget;
    ref<ResponseHandler> rh;
    ref<TimingHdlr> th;
    Element::ChildStatus status;
  };
}

using namespace dummy_namespace_to_avoid_gcc411_bug_trial_cc;

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
    activeState(nullptr)
  {}

  Trial* owner;

  int correctResponse;

  typedef std::vector<ref<GxNode> > GxNodes;
  GxNodes gxNodes;

  unsigned int currentNode;

  std::vector<Response> responses;

  int trialType;
  soft_ref<ResponseHandler> rh;
  soft_ref<TimingHdlr> th;

  fstring info;

  std::unique_ptr<ActiveState> activeState;

  bool isActive() const { return activeState.get() != nullptr; }

  void becomeActive(Element* parent, soft_ref<Toglet> widget)
  {
    activeState.reset(new ActiveState(owner, parent, widget, rh, th));
  }

  void becomeInactive()
  {
    activeState.reset(nullptr);
  }

  void installCurrentNode() const
  {
    GVX_PRECONDITION( isActive() );
    if (activeState->widget.is_valid() && currentNode < gxNodes.size())
      {
        activeState->widget->setDrawable(gxNodes[currentNode]);
        nub::log(rutz::sfmt("current node is %s",
                            gxNodes[currentNode]->unique_name().c_str()));
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
GVX_TRACE("Trial::make");
  return new Trial;
}

Trial::Trial() :
  FieldContainer(0),
  rep( new Impl(this) )
{
GVX_TRACE("Trial::Trial()");

  setFieldMap(Trial::classFields());
}

Trial::~Trial() noexcept
{
GVX_TRACE("Trial::~Trial");
  delete rep;
}

io::version_id Trial::class_version_id() const
  { return TRIAL_SVID; }

void Trial::read_from(io::reader& reader)
{
GVX_TRACE("Trial::read_from");

  rep->becomeInactive();

  reader.ensure_version_id("Trial", 5,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  rep->gxNodes.clear();
  io::read_utils::read_object_seq<GxNode>
    (reader, "gxObjects", std::back_inserter(rep->gxNodes));

  rep->responses.clear();
  io::read_utils::read_value_obj_seq<Response>
    (reader, "responses", std::back_inserter(rep->responses));

  reader.read_value("correctResponse", rep->correctResponse);

  reader.read_value("type", rep->trialType);

  rep->rh = dyn_cast<ResponseHandler>(reader.read_weak_object("rh"));
  rep->th = dyn_cast<TimingHdlr>(reader.read_weak_object("th"));

  reader.read_value("info", rep->info);
}

void Trial::write_to(io::writer& writer) const
{
GVX_TRACE("Trial::write_to");

  writer.ensure_output_version_id("Trial", TRIAL_SVID, 5,
                              "Try groovx0.8a3", SRC_POS);

  io::write_utils::write_object_seq(writer, "gxObjects",
                                    rep->gxNodes.begin(),
                                    rep->gxNodes.end());

  io::write_utils::write_value_obj_seq(writer, "responses",
                                       rep->responses.begin(),
                                       rep->responses.end());

  writer.write_value("correctResponse", rep->correctResponse);

  writer.write_value("type", rep->trialType);

  writer.write_object("rh", rep->rh);
  writer.write_object("th", rep->th);

  writer.write_value("info", rep->info);
}

const soft_ref<Toglet>& Trial::getWidget() const
{
  GVX_PRECONDITION( rep->isActive() );
  return rep->activeState->widget;
}

int Trial::getCorrectResponse() const
  { return rep->correctResponse; }

void Trial::setCorrectResponse(int response)
  { rep->correctResponse = response; }


ref<ResponseHandler> Trial::getResponseHandler() const
  { return ref<ResponseHandler>(rep->rh); }

void Trial::setResponseHandler(ref<ResponseHandler> rh)
  { rep->rh = soft_ref<ResponseHandler>(rh); }


ref<TimingHdlr> Trial::getTimingHdlr() const
  { return ref<TimingHdlr>(rep->th); }

void Trial::setTimingHdlr(ref<TimingHdlr> th)
  { rep->th = soft_ref<TimingHdlr>(th); }


int Trial::trialType() const
  { return rep->trialType; }

void Trial::setType(int t)
  { rep->trialType = t; }


fstring Trial::vxInfo() const
{
GVX_TRACE("Trial::vxInfo");
  if (rep->info.is_empty())
    return stdInfo();

  return rep->info;
}

int Trial::lastResponse() const
{
GVX_TRACE("Trial::lastResponse");

  if (rep->responses.empty())
    throw rutz::error("the trial has no responses yet", SRC_POS);

  return rep->responses.back().val();
}

void Trial::vxUndo()
{
GVX_TRACE("Trial::vxUndo");
  if ( !rep->responses.empty() )
    rep->responses.pop_back();
}

rutz::fwd_iter<Response> Trial::responses() const
{
GVX_TRACE("Trial::responses");
  return rutz::fwd_iter<Response>
    (rep->responses.begin(), rep->responses.end());
}

size_t Trial::numResponses() const
  { return rep->responses.size(); }

void Trial::clearResponses()
  { rep->responses.clear(); }

double Trial::avgResponse() const
{
GVX_TRACE("Trial::avgResponse");
  int sum = 0;
  for (const Response& r: rep->responses)
    {
      sum += r.val();
    }
  return (rep->responses.size() > 0) ? double(sum)/rep->responses.size() : 0.0;
}

double Trial::avgRespTime() const
{
GVX_TRACE("Trial::avgRespTime");
  int sum = 0;
  for (const Response& r: rep->responses)
    {
      sum += r.msec();

      dbg_eval(3, sum);
      dbg_eval_nl(3, size_t(sum)/rep->responses.size());
    }
  return (rep->responses.size() > 0) ? double(sum)/rep->responses.size() : 0.0;
}


void Trial::addNode(ref<GxNode> item)
{
GVX_TRACE("Trial::addNode");
  rep->gxNodes.push_back(item);
}

void Trial::trNextNode()
{
GVX_TRACE("Trial::trNextNode");
  setCurrentNode(rep->currentNode+1);
}

rutz::fwd_iter<ref<GxNode> > Trial::nodes() const
{
GVX_TRACE("Trial::nodes");
  return rutz::fwd_iter<ref<GxNode> >
    (rep->gxNodes.begin(), rep->gxNodes.end());
}

unsigned int Trial::getCurrentNode() const
{
GVX_TRACE("Trial::getCurrentNode");
  return rep->currentNode;
}

void Trial::setCurrentNode(unsigned int nodeNumber)
{
GVX_TRACE("Trial::setCurrentNode");
  if (nodeNumber >= rep->gxNodes.size())
    {
      throw rutz::error(rutz::sfmt("invalid node number %u",
                                   nodeNumber), SRC_POS);
    }
  rep->currentNode = nodeNumber;
}

void Trial::clearObjs()
{
GVX_TRACE("Trial::clearObjs");
  rep->gxNodes.clear();
}

fstring Trial::stdInfo() const
{
GVX_TRACE("Trial::stdInfo");

  std::ostringstream objids;
  std::ostringstream cats;

  for (const ref<GxNode>& node: rep->gxNodes)
    {
      for (rutz::fwd_iter<const ref<GxNode> > tr(node->deepChildren());
           tr.is_valid();
           tr.next())
        {
          const GxShapeKit* g =
            dynamic_cast<const GxShapeKit*>((*tr).get());

          if (g)
            {
              objids << " " << g->unique_name();
              cats << " " << g->category();
            }
        }
    }

  return rutz::sfmt("trial type == %d, objs ==%s, categories == %s",
                    rep->trialType,
                    objids.str().c_str(),
                    cats.str().c_str());
}

void Trial::setInfo(fstring info)
{
GVX_TRACE("Trial::setInfo");
  rep->info = info;
}


void Trial::vxRun(Element& parent)
{
GVX_TRACE("Trial::vxRun");

  soft_ref<Toglet> widget = parent.getWidget();

  GVX_PRECONDITION(widget.is_valid());

  if ( rep->rh.is_invalid() || rep->th.is_invalid() )
    {
      throw rutz::error("the trial did not have a valid timing handler "
                        "and response handler", SRC_POS);
    }

  rep->becomeActive(&parent, widget);

  nub::log( vxInfo() );

  rep->currentNode = 0;

  rep->activeState->rh->rhBeginTrial(widget, *this);
  rep->activeState->th->thBeginTrial(*this);
}

double Trial::trElapsedMsec()
{
GVX_TRACE("Trial::trElapsedMsec");

  GVX_PRECONDITION( rep->isActive() );

  return rep->activeState->th->getElapsedMsec();
}

void Trial::trAbortTrial()
{
GVX_TRACE("Trial::trAbortTrial");

  GVX_PRECONDITION( rep->isActive() );

  nub::log("trAbortTrial");

  rep->activeState->status = ChildStatus::ABORTED;
  rep->activeState->rh->rhAbortTrial();
  rep->activeState->th->thAbortTrial();
}

void Trial::vxReturn(ChildStatus /*s*/)
{
  // FIXME
  GVX_ASSERT(false);
}

void Trial::trEndTrial()
{
GVX_TRACE("Trial::trEndTrial");

  GVX_PRECONDITION( rep->isActive() );

  nub::log("Trial::trEndTrial");

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
GVX_TRACE("Trial::vxHalt");

  if (!rep->isActive()) return;

  nub::log("Trial::vxHalt");

  if (rep->activeState->widget.is_valid())
    rep->activeState->widget->undraw();

  rep->activeState->rh->rhHaltExpt();
  rep->activeState->th->thHaltExpt();

  rep->becomeInactive();
}

void Trial::vxReset()
{
GVX_TRACE("Trial::vxReset");
  rep->responses.clear();
}

void Trial::trProcessResponse(Response& response)
{
GVX_TRACE("Trial::trProcessResponse");

  GVX_PRECONDITION( rep->isActive() );

  nub::log("trProcessResponse");

  rep->responses.push_back(response);

  dbg_eval(3, rep->correctResponse);
  dbg_eval_nl(3, response.val());

  rep->activeState->th->thResponseSeen();

  // If the response was incorrect, we'll ask our parent container to
  // kindly repeat this trial
  if (!response.matchesCorrectValue(rep->correctResponse))
    {
      rep->activeState->status = ChildStatus::REPEAT;
    }
}

void Trial::trDraw()
{
GVX_TRACE("Trial::trDraw");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    {
      rep->installCurrentNode();
      widget->setVisibility(true);
      widget->fullRender();
    }
}

void Trial::trRender()
{
GVX_TRACE("Trial::trRender");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    {
      rep->installCurrentNode();
      widget->setVisibility(true);
      widget->render();
    }
}

void Trial::trUndraw()
{
GVX_TRACE("Trial::trUndraw");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->undraw();
}

void Trial::trSwapBuffers()
{
GVX_TRACE("Trial::trSwapBuffers");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->swapBuffers();
}

void Trial::trRenderBack()
{
GVX_TRACE("Trial::trRenderBack");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->getCanvas()->drawOnBackBuffer();
}

void Trial::trRenderFront()
{
GVX_TRACE("Trial::trRenderFront");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->getCanvas()->drawOnFrontBuffer();
}

void Trial::trClearBuffer()
{
GVX_TRACE("Trial::trClearBuffer");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->clearscreen();
}

void Trial::trFinishDrawing()
{
GVX_TRACE("Trial::trFinishDrawing");
  soft_ref<Toglet> widget = getWidget();
  if (widget.is_valid())
    widget->getCanvas()->finishDrawing();
}


void Trial::trAllowResponses()
{
GVX_TRACE("Trial::trAllowResponses");

  GVX_PRECONDITION( rep->isActive() );

  nub::log("trAllowResponses");

  rep->activeState->rh->rhAllowResponses
    (rep->activeState->widget, *this);
}

void Trial::trDenyResponses()
{
GVX_TRACE("Trial::trDenyResponses");

  GVX_PRECONDITION( rep->isActive() );

  nub::log("trDenyResponses");

  rep->activeState->rh->rhDenyResponses();
}
