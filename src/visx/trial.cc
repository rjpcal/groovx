///////////////////////////////////////////////////////////////////////
//
// trial.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Mar 12 17:43:21 1999
// written: Wed Jun  6 09:39:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include "block.h"
#include "response.h"
#include "responsehandler.h"
#include "timinghdlr.h"

#include "grobj.h"

#include "gx/gxtraversal.h"
#include "gx/gxseparator.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "gwt/widget.h"

#include "util/errorhandler.h"
#include "util/iditem.h"
#include "util/minivec.h"

#include <strstream.h>

#define DYNAMIC_TRACE_EXPR Trial::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#define TIME_TRACE

Util::Tracer Trial::tracer;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const IO::VersionId TRIAL_SERIAL_VERSION_ID = 4;

  const char* ioTag = "Trial";
}

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Trial::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  enum TrialState { ACTIVE, INACTIVE };

  Impl(Trial*) :
	 itsCorrectResponse(Response::ALWAYS_CORRECT),
	 itsGxNodes(),
	 itsCurrentNode(0),
	 itsResponses(),
	 itsType(-1),
	 itsRh(),
	 itsTh(),
	 itsState(INACTIVE),
	 itsWidget(0),
	 itsBlock(0)
	 {}

private:
  int itsCorrectResponse;

  typedef minivec<IdItem<GxSeparator> > GxNodes;
  GxNodes itsGxNodes;

  unsigned int itsCurrentNode;

  minivec<Response> itsResponses;
  int itsType;
  MaybeIdItem<ResponseHandler> itsRh;
  MaybeIdItem<TimingHdlr> itsTh;

  TrialState itsState;

  GWT::Widget* itsWidget;
  Block* itsBlock;

  bool assertIdsOrHalt()
	 {
		if ( itsRh.isValid() && itsTh.isValid() )
		  return true;

		DebugPrintNL("Trial::assertIdsOrHalt failed");
		DebugEval(itsRh.id()); DebugEvalNL(itsTh.id());

		trHaltExpt();

		return false;
	 }

  GWT::Widget& getWidget() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition(itsWidget != 0);
		return *itsWidget;
	 }

  ResponseHandler& responseHandler()
	 {
		Precondition(itsState == ACTIVE);
		Precondition( itsRh.isValid() );
		return *(itsRh.get());
	 }

  const ResponseHandler& responseHandler() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition( itsRh.isValid() );
		return *(itsRh.get());
	 }

  TimingHdlr& timingHdlr()
	 {
		Precondition(itsState == ACTIVE);
		Precondition( itsTh.isValid() );
		return *(itsTh.get());
	 }

  const TimingHdlr& timingHdlr() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition( itsTh.isValid() );
		return *(itsTh.get());
	 }

  Block& getBlock() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition(itsBlock != 0);
		return *itsBlock;
	 }

  inline void timeTrace(const char* loc) {
#ifdef TIME_TRACE
	 cerr << "in " << loc
			<< "@ elapsed time == " << timingHdlr().getElapsedMsec() << endl;
#endif
  }

public:

  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;

  void writeMatlab(STD_IO::ostream& os) const;

  int getCorrectResponse() const { return itsCorrectResponse; }
  void setCorrectResponse(int response) { itsCorrectResponse = response; }

#ifndef ACC_COMPILER
  IdItem<ResponseHandler> getResponseHandler() const { return itsRh; }
  void setResponseHandler(IdItem<ResponseHandler> rh) { itsRh = rh; }

  IdItem<TimingHdlr> getTimingHdlr() const { return itsTh; }
  void setTimingHdlr(IdItem<TimingHdlr> th) { itsTh = th; }
#else
  IdItem<ResponseHandler> getResponseHandler() const
    { return IdItem<ResponseHandler>(itsRh); }
  void setResponseHandler(IdItem<ResponseHandler> rh)
    { itsRh = MaybeIdItem<ResponseHandler>(rh); }

  IdItem<TimingHdlr> getTimingHdlr() const
    { return IdItem<TimingHdlr>(itsTh); }
  void setTimingHdlr(IdItem<TimingHdlr> th)
    { itsTh = MaybeIdItem<TimingHdlr>(th); }
#endif

  int trialType() const;
  void setType(int t);

  const char* description() const;

  int lastResponse() const;
  void undoLastResponse();
  int numResponses() const;
  void clearResponses();

  double avgResponse() const;
  double avgRespTime() const;

  void add(Util::UID objid, Util::UID posid);

  void addNode(Util::UID id)
  {
	 IdItem<GxNode> obj(id);
	 try {
		IdItem<GxSeparator> sep = dynamicCast<GxSeparator>(obj);
		itsGxNodes.push_back(sep);		
	 }
	 catch (...) {
		IdItem<GxSeparator> sep(GxSeparator::make());
		sep->addChild(id);
		itsGxNodes.push_back(sep);
	 }
  }

  void trNextNode() { setCurrentNode(itsCurrentNode+1); }

  unsigned int getCurrentNode() const { return itsCurrentNode; }
  void setCurrentNode(unsigned int nodeNumber)
  {
	 if (nodeNumber >= itsGxNodes.size())
		{
		  ErrorWithMsg err("invalid node number ");
		  err.appendNumber(nodeNumber);
		  throw err;
		}
	 itsCurrentNode = nodeNumber;
  }

  void clearObjs();

  void trDoTrial(Trial* self, GWT::Widget& widget,
					  Util::ErrorHandler& errhdlr, Block& block);
  int trElapsedMsec();
  void trAbortTrial();
  void trEndTrial();
  void trNextTrial();
  void trHaltExpt();
  void trResponseSeen();
  void trRecordResponse(Response& response);
  void trAllowResponses(Trial* self);
  void trDenyResponses();

  void installSelf(GWT::Widget& widget) const;
};

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void Trial::Impl::readFrom(IO::Reader* reader) {
DOTRACE("Trial::Impl::readFrom");

  itsState = INACTIVE;

  reader->ensureReadVersionId("Trial", 4, "Try grsh0.8a4");

  itsGxNodes.clear();
  IO::ReadUtils::readObjectSeq<GxSeparator>(
          reader, "gxObjects", std::back_inserter(itsGxNodes));

  itsResponses.clear();
  IO::ReadUtils::readValueObjSeq<Response>(
          reader, "responses", std::back_inserter(itsResponses));

  reader->readValue("correctResponse", itsCorrectResponse);

  reader->readValue("type", itsType);

  itsRh = dynamicCast<ResponseHandler>(reader->readMaybeObject("rh"));
  itsTh = dynamicCast<TimingHdlr>(reader->readMaybeObject("th"));
}

void Trial::Impl::writeTo(IO::Writer* writer) const {
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

void Trial::Impl::writeMatlab(STD_IO::ostream& os) const {
DOTRACE("Trial::Impl::writeMatlab");

  for (GxNodes::const_iterator
			ii = itsGxNodes.begin(),
			end = itsGxNodes.end();
		 ii != end;
		 ++ii)
	 {
		for (GxTraversal tr(ii->get()); tr.hasMore(); tr.advance())
		  {
			 const GrObj* g = dynamic_cast<const GrObj*>(tr.current());
			 if (g)
				os << g->id() << ' ';
		  }
	 }
}

///////////////
// accessors //
///////////////

int Trial::Impl::trialType() const {
DOTRACE("Trial::Impl::trialType");
  return itsType;
} 

const char* Trial::Impl::description() const {
DOTRACE("Trial::Impl::description");

  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];

  ostrstream ost(buf, BUF_SIZE);
  
  ost << "trial type == " << trialType()
      << ", objs ==";


  {for (GxNodes::const_iterator
			 ii = itsGxNodes.begin(),
			 end = itsGxNodes.end();
		  ii != end;
		  ++ii)
	 {
		for (GxTraversal tr(ii->get()); tr.hasMore(); tr.advance())
		  {
			 const GrObj* g = dynamic_cast<const GrObj*>(tr.current());
			 if (g)
				ost << ' ' << g->id();
		  }
	 }
  }

  ost << ", categories == ";

  {for (GxNodes::const_iterator
			 ii = itsGxNodes.begin(),
			 end = itsGxNodes.end();
		  ii != end;
		  ++ii)
	 {
		for (GxTraversal tr(ii->get()); tr.hasMore(); tr.advance())
		  {
			 const GrObj* g = dynamic_cast<const GrObj*>(tr.current());
			 if (g)
				ost << ' ' << g->category();
		  }
	 }
  }

  ost << '\0';

  return buf;
}

int Trial::Impl::lastResponse() const {
DOTRACE("Trial::Impl::lastResponse");
  return itsResponses.back().val();
}

int Trial::Impl::numResponses() const {
DOTRACE("Trial::Impl::numResponses");
  return itsResponses.size();
}

double Trial::Impl::avgResponse() const {
DOTRACE("Trial::Impl::avgResponse");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = itsResponses.begin();
		 ii != itsResponses.end();
		 ++ii) {
	 sum += ii->val();
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

double Trial::Impl::avgRespTime() const {
DOTRACE("Trial::Impl::avgRespTime");
  int sum = 0;
  for (minivec<Response>::const_iterator ii = itsResponses.begin();
		 ii != itsResponses.end();
		 ++ii) {
	 sum += ii->msec();

	 DebugEval(sum);
	 DebugEvalNL(sum/itsResponses.size());
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

//////////////////
// manipulators //
//////////////////

void Trial::Impl::add(Util::UID objid, Util::UID posid) {
DOTRACE("Trial::Impl::add");

  IdItem<GxSeparator> sepitem(GxSeparator::make());

  sepitem->addChild(posid);
  sepitem->addChild(objid);

  if (itsGxNodes.size() == 0) {
	 itsGxNodes.push_back(sepitem);
  }
  else {
	 itsGxNodes[0]->addChild(sepitem.id());
  }
}

void Trial::Impl::clearObjs() {
DOTRACE("Trial::Impl::clearObjs");
  itsGxNodes.clear();
}

void Trial::Impl::setType(int t) {
DOTRACE("Trial::Impl::setType");
  itsType = t;
}

void Trial::Impl::clearResponses() {
DOTRACE("Trial::Impl::clearResponses");
  itsResponses.clear();
}

/////////////
// actions //
/////////////

void Trial::Impl::trDoTrial(Trial* self, GWT::Widget& widget,
									 Util::ErrorHandler& errhdlr, Block& block) {
DOTRACE("Trial::Impl::trDoTrial");
  Precondition(self != 0); 
  Precondition(&widget != 0);
  Precondition(&errhdlr != 0);
  Precondition(&block != 0);

  itsWidget = &widget;
  itsBlock = &block;

  Assert(itsBlock != 0);
  Assert(itsWidget != 0);

  if ( !assertIdsOrHalt() ) return;

  itsState = ACTIVE;

  itsCurrentNode = 0;

  responseHandler().rhBeginTrial(widget, *self);

  timingHdlr().thBeginTrial(widget, errhdlr, *self);
  timeTrace("trDoTrial"); 
}

int Trial::Impl::trElapsedMsec() {
DOTRACE("Trial::Impl::trElapsedMsec");
  if ( itsState == INACTIVE ) return -1;
  if ( !assertIdsOrHalt() ) return -1;

  return timingHdlr().getElapsedMsec();
}

void Trial::Impl::trAbortTrial() {
DOTRACE("Trial::Impl::trAbortTrial");
  if (INACTIVE == itsState) return;

  if ( !assertIdsOrHalt() ) return;

  timeTrace("trAbortTrial");

  responseHandler().rhAbortTrial();
  timingHdlr().thAbortTrial();

  getBlock().abortTrial();
}

void Trial::Impl::trEndTrial() {
DOTRACE("Trial::Impl::trEndTrial");
  if (INACTIVE == itsState) return;

  if ( !assertIdsOrHalt() ) return;

  timeTrace("trEndTrial");

  responseHandler().rhEndTrial();

  getBlock().endTrial();
}

void Trial::Impl::trNextTrial() {
DOTRACE("Trial::Impl::trNextTrial");
  if (INACTIVE == itsState) return;

  if ( !assertIdsOrHalt() ) return;

  timeTrace("trNextTrial");

  // We have to getBlock() before changing to inactive state, since
  // the invariant says we cannot getBlock() when in an inactive state.
  Block& block = getBlock();

  itsState = INACTIVE;

  block.nextTrial();
}

void Trial::Impl::trHaltExpt() {
DOTRACE("Trial::Impl::trHaltExpt");
  if (INACTIVE == itsState) return;

  if ( itsTh.isValid() ) { 
	 timeTrace("trHaltExpt");
  }

  getWidget().undraw();

  if ( itsRh.isValid() ) {
	 responseHandler().rhHaltExpt();
  }

  // This must come last in order to ensure that we cancel any timer
  // callbacks that were scheduled in trAbortTrial().
  if ( itsTh.isValid() ) { 
	 timingHdlr().thHaltExpt();
  }

  itsState = INACTIVE;
}

void Trial::Impl::trResponseSeen() {
DOTRACE("Trial::Impl::trResponseSeen");
  if (INACTIVE == itsState) return;

  if ( !assertIdsOrHalt() ) return;

  timeTrace("trResponseSeen");

  timingHdlr().thResponseSeen();
}

void Trial::Impl::trRecordResponse(Response& response) {
DOTRACE("Trial::Impl::trRecordResponse");
  if (INACTIVE == itsState) return;

  timeTrace("trRecordResponse");
  response.setCorrectVal(itsCorrectResponse);

  itsResponses.push_back(response);

  getBlock().processResponse(response);
}

void Trial::Impl::trAllowResponses(Trial* self) {
DOTRACE("Trial::Impl::trAllowResponses");
  if (INACTIVE == itsState) return;

  timeTrace("trAllowResponses");

  responseHandler().rhAllowResponses(getWidget(), *self);
}

void Trial::Impl::trDenyResponses() {
DOTRACE("Trial::Impl::trDenyResponses");
  if (INACTIVE == itsState) return;

  timeTrace("trDenyResponses");

  responseHandler().rhDenyResponses();
}

void Trial::Impl::installSelf(GWT::Widget& widget) const {
DOTRACE("Trial::Impl::installSelf");

  if (itsCurrentNode < itsGxNodes.size())
	 widget.setDrawable(IdItem<GxNode>(itsGxNodes[itsCurrentNode]));
}

void Trial::Impl::undoLastResponse() {
DOTRACE("Trial::Impl::undoLastResponse");
  if ( !itsResponses.empty() )
	 itsResponses.pop_back();
}


///////////////////////////////////////////////////////////////////////
//
// Trial member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Trial* Trial::make() {
DOTRACE("Trial::make");
  return new Trial;
}

Trial::Trial() : 
  itsImpl( new Impl(this) )
{
DOTRACE("Trial::Trial()");
}

Trial::~Trial() {
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

void Trial::writeMatlab(STD_IO::ostream& os) const
  { itsImpl->writeMatlab(os); }


int Trial::getCorrectResponse() const
  { return itsImpl->getCorrectResponse(); }

void Trial::setCorrectResponse(int response)
  { itsImpl->setCorrectResponse(response); }


IdItem<ResponseHandler> Trial::getResponseHandler() const
  { return itsImpl->getResponseHandler(); }

void Trial::setResponseHandler(IdItem<ResponseHandler> rh)
  { itsImpl->setResponseHandler(rh); }


IdItem<TimingHdlr> Trial::getTimingHdlr() const
  { return itsImpl->getTimingHdlr(); }

void Trial::setTimingHdlr(IdItem<TimingHdlr> th)
  { itsImpl->setTimingHdlr(th); }


int Trial::trialType() const
  { return itsImpl->trialType(); }

void Trial::setType(int t)
  { itsImpl->setType(t); }


const char* Trial::description() const
  { return itsImpl->description(); }

int Trial::lastResponse() const
  { return itsImpl->lastResponse(); }

void Trial::undoLastResponse()
  { itsImpl->undoLastResponse(); }

int Trial::numResponses() const
  { return itsImpl->numResponses(); }

void Trial::clearResponses()
  { itsImpl->clearResponses(); }

double Trial::avgResponse() const
  { return itsImpl->avgResponse(); }

double Trial::avgRespTime() const
  { return itsImpl->avgRespTime(); }


void Trial::add(Util::UID objid, Util::UID posid)
  { itsImpl->add(objid, posid); }

void Trial::addNode(Util::UID id)
  { itsImpl->addNode(id); }

void Trial::trNextNode()
  { itsImpl->trNextNode(); }

unsigned int Trial::getCurrentNode() const
  { return itsImpl->getCurrentNode(); }

void Trial::setCurrentNode(unsigned int nodeNumber)
  { itsImpl->setCurrentNode(nodeNumber); }

void Trial::clearObjs()
  { itsImpl->clearObjs(); }


void Trial::trDoTrial(GWT::Widget& widget,
							 Util::ErrorHandler& errhdlr, Block& block)
  { itsImpl->trDoTrial(this, widget, errhdlr, block); }

int Trial::trElapsedMsec()
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

void Trial::installSelf(GWT::Widget& widget) const
  { itsImpl->installSelf(widget); }

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
