///////////////////////////////////////////////////////////////////////
//
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Tue Oct 31 11:42:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include "block.h"
#include "grobj.h"
#include "position.h"
#include "response.h"
#include "responsehandler.h"
#include "timinghdlr.h"

#include "io/iditem.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "gwt/canvas.h"
#include "gwt/widget.h"

#include "util/errorhandler.h"
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
  const IO::VersionId TRIAL_SERIAL_VERSION_ID = 2;

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
	 itsGrObjs(),
	 itsPositions(),
	 itsResponses(),
	 itsType(-1),
	 itsRh(-1),
	 itsTh(0),
	 itsState(INACTIVE),
	 itsCanvas(0),
	 itsBlock(0)
	 {}

private:
  int itsCorrectResponse;

  minivec<IdItem<GrObj> > itsGrObjs;
  minivec<IdItem<Position> > itsPositions;

  minivec<Response> itsResponses;
  int itsType;
  MaybeIdItem<ResponseHandler> itsRh;
  MaybeIdItem<TimingHdlr> itsTh;

  TrialState itsState;

  GWT::Canvas* itsCanvas;
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

  GWT::Canvas& getCanvas() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition(itsCanvas != 0);
		return *itsCanvas;
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
  int readFromObjidsOnly(STD_IO::istream &is, int offset);

  int getCorrectResponse() const { return itsCorrectResponse; }
  int getResponseHandler() const;
  int getTimingHdlr() const;
  Trial::GrObjItr beginGrObjs() const;
  Trial::GrObjItr endGrObjs() const;
  int trialType() const;
  const char* description() const;
  int lastResponse() const;
  int numResponses() const;
  double avgResponse() const;
  double avgRespTime() const;

  void setCorrectResponse(int response) { itsCorrectResponse = response; }
  void add(int objid, int posid);
  void clearObjs();
  void setType(int t);
  void setResponseHandler(int rhid);
  void setTimingHdlr(int thid);
  void clearResponses();

  void trDoTrial(Trial* self, GWT::Widget& widget,
					  Util::ErrorHandler& errhdlr, Block& block);
  int trElapsedMsec();
  void trAbortTrial();
  void trEndTrial();
  void trNextTrial();
  void trHaltExpt();
  void trResponseSeen();
  void trRecordResponse(Response& response);
  void trDrawTrial() const;
  void trUndrawTrial() const;
  void trDraw(GWT::Canvas& canvas, bool flush) const;
  void trUndraw(GWT::Canvas& canvas, bool flush) const;
  void undoLastResponse();
};

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void Trial::Impl::readFrom(IO::Reader* reader) {
DOTRACE("Trial::Impl::readFrom");

  itsState = INACTIVE;

  IO::VersionId svid = reader->readSerialVersionId();

  if (svid < 2)
	 {
		throw IO::ReadVersionError("Trial", svid, 2, "Try grsh0.8a3");
	 }

  Assert(svid >= 2);

  itsGrObjs.clear();
  IO::ReadUtils::readObjectSeq<GrObj>(
	       reader, "grobjs",
			 IdItem<GrObj>::makeInserter(itsGrObjs));

  itsPositions.clear();
  IO::ReadUtils::readObjectSeq<Position>(
          reader, "positions",
			 IdItem<Position>::makeInserter(itsPositions));

  itsResponses.clear();
  IO::ReadUtils::readValueObjSeq<Response>(reader, "responses",
									  std::back_inserter(itsResponses));

  reader->readValue("correctResponse", itsCorrectResponse);

  reader->readValue("type", itsType);

  IO::IoObject* rhio = reader->readObject("rh");
  ResponseHandler* rh = dynamic_cast<ResponseHandler*>(rhio);
  itsRh = MaybeIdItem<ResponseHandler>(rh);

  IO::IoObject* thio = reader->readObject("th");
  TimingHdlr* th = dynamic_cast<TimingHdlr*>(thio);
  itsTh = MaybeIdItem<TimingHdlr>(th);
}

void Trial::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("Trial::Impl::writeTo");

  if (TRIAL_SERIAL_VERSION_ID < 2)
	 {
		throw IO::WriteVersionError("Trial", TRIAL_SERIAL_VERSION_ID, 2,
											 "Try grsh0.8a3");
	 }

  Assert(TRIAL_SERIAL_VERSION_ID >= 2);

  IO::WriteUtils::writeSmartPtrSeq(writer, "grobjs",
											  itsGrObjs.begin(), itsGrObjs.end());

  IO::WriteUtils::writeSmartPtrSeq(writer, "positions",
											  itsPositions.begin(), itsPositions.end());

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
										 itsResponses.begin(), itsResponses.end());

  writer->writeValue("correctResponse", itsCorrectResponse);

  writer->writeValue("type", itsType);

  writer->writeObject("rh", itsRh.isValid() ? itsRh.get() : 0);
  writer->writeObject("th", itsTh.isValid() ? itsTh.get() : 0);
}

int Trial::Impl::readFromObjidsOnly(STD_IO::istream &is, int offset) {
DOTRACE("Trial::Impl::readFromObjidsOnly");

  itsState = INACTIVE;

  int posid = 0;
  int objid;
  if (offset == 0) {
    while (is >> objid) {
		if ( objid < 0) {
		  throw IO::ValueError(ioTag);
		}
      add(objid, posid);
      ++posid;
    }
  }
  else { // offset != 0
    while (is >> objid) {
		if ( (objid+offset) < 0 ) {
		  throw IO::ValueError(ioTag);
		}
      add(objid+offset, posid);
      ++posid;
    }
  }

  // Throw an exception if the stream has failed. However, since
  // istrstream's seem to always fail at eof, even if nothing went
  // wrong, we must only throw the exception if we have fail'ed with
  // out reaching eof. This should catch most mistakes.
  if (is.fail() && !is.eof()) throw IO::InputError(ioTag);

  // return the number of objid's read
  return posid;
}

///////////////
// accessors //
///////////////

int Trial::Impl::getResponseHandler() const {
DOTRACE("Trial::Impl::getResponseHandler");
  DebugEvalNL(itsRh.id());
  return itsRh.id();
}

int Trial::Impl::getTimingHdlr() const {
DOTRACE("Trial::Impl::getTimingHdlr");
  DebugEvalNL(itsTh.id());
  return itsTh.id();
}

Trial::GrObjItr Trial::Impl::beginGrObjs() const {
DOTRACE("Trial::Impl::beginGrObjs");
  return &itsGrObjs[0];
}

Trial::GrObjItr Trial::Impl::endGrObjs() const {
DOTRACE("Trial::Impl::endGrObjs");
  return beginGrObjs() + itsGrObjs.size();
}

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
  for (size_t i = 0; i < itsGrObjs.size(); ++i) {
    ost << " " << itsGrObjs[i].id();
  }

  ost << ", categories ==";
  for (size_t j = 0; j < itsGrObjs.size(); ++j) {
    DebugEvalNL(itsGrObjs[j].id());

    Assert(itsGrObjs[j].get() != 0);

    ost << " " << itsGrObjs[j]->getCategory();
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

void Trial::Impl::add(int objid, int posid) {
DOTRACE("Trial::Impl::add");
  itsGrObjs.push_back(IdItem<GrObj>(objid));
  itsPositions.push_back(IdItem<Position>(posid));

  Invariant(itsGrObjs.size() == itsPositions.size());
}

void Trial::Impl::clearObjs() {
DOTRACE("Trial::Impl::clearObjs");
  itsGrObjs.clear(); 
  itsPositions.clear();
}

void Trial::Impl::setType(int t) {
DOTRACE("Trial::Impl::setType");
  itsType = t;
}

void Trial::Impl::setResponseHandler(int rhid) {
DOTRACE("Trial::Impl::setResponseHandler");
  if (rhid < 0)
	 throw ErrorWithMsg("response handler id was negative");
  itsRh = MaybeIdItem<ResponseHandler>(rhid);
}

void Trial::Impl::setTimingHdlr(int thid) {
DOTRACE("Trial::Impl::setTimingHdlr");
  if (thid < 0)
	 throw ErrorWithMsg("timing handler id was negative");
  itsTh = MaybeIdItem<TimingHdlr>(thid);
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

  itsCanvas = widget.getCanvas();
  itsBlock = &block;

  Assert(itsBlock != 0);
  Assert(itsCanvas != 0);

  if ( !assertIdsOrHalt() ) return;

  itsState = ACTIVE;

  timingHdlr().thBeginTrial(widget, errhdlr, *self);
  timeTrace("trDoTrial"); 

  responseHandler().rhBeginTrial(widget, *self);
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

  trUndrawTrial();
  trAbortTrial();
  trEndTrial();

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

void Trial::Impl::trDrawTrial() const {
DOTRACE("Trial::Impl::trDrawTrial");
  if (INACTIVE == itsState) return;

  trDraw(getCanvas(), true);

  getBlock().drawTrialHook();
}

void Trial::Impl::trUndrawTrial() const {
DOTRACE("Trial::Impl::trUndrawTrial");
  if (INACTIVE == itsState) return;

  trUndraw(getCanvas(), true);
}

void Trial::Impl::trDraw(GWT::Canvas& canvas, bool flush) const {
DOTRACE("Trial::Impl::trDraw");

  Invariant(itsGrObjs.size() == itsPositions.size());

  for (size_t i = 0; i < itsGrObjs.size(); ++i) {

    DebugEval(itsGrObjs[i].id());
    DebugEvalNL((void*)itsGrObjs[i].get());
    DebugEval(itsPositions[i].id());
    DebugEvalNL((void*)itsPositions[i].get());

	 Assert(itsGrObjs[i].get() != 0);
	 Assert(itsPositions[i].get() != 0);

	 { 
		GWT::Canvas::StateSaver state(canvas);
		itsPositions[i]->go();
		itsGrObjs[i]->draw(canvas);
	 }
  }

  if (flush) canvas.flushOutput();
}

void Trial::Impl::trUndraw(GWT::Canvas& canvas, bool flush) const {
DOTRACE("Trial::Impl::trUndraw");

  Invariant(itsGrObjs.size() == itsPositions.size());

  for (size_t i = 0; i < itsGrObjs.size(); ++i) {

	 Assert(itsGrObjs[i].get() != 0);
	 Assert(itsPositions[i].get() != 0);

	 {
		GWT::Canvas::StateSaver state(canvas);
		itsPositions[i]->rego();
		itsGrObjs[i]->undraw(canvas);
	 }
  }

  if (flush) canvas.flushOutput();
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

int Trial::readFromObjidsOnly(STD_IO::istream &is, int offset)
  { return itsImpl->readFromObjidsOnly(is, offset); }

int Trial::getCorrectResponse() const
  { return itsImpl->getCorrectResponse(); }

int Trial::getResponseHandler() const
  { return itsImpl->getResponseHandler(); }

int Trial::getTimingHdlr() const
  { return itsImpl->getTimingHdlr(); }

Trial::GrObjItr Trial::beginGrObjs() const
  { return itsImpl->beginGrObjs(); }

Trial::GrObjItr Trial::endGrObjs() const
  { return itsImpl->endGrObjs(); }

int Trial::trialType() const
  { return itsImpl->trialType(); }

const char* Trial::description() const
  { return itsImpl->description(); }

int Trial::lastResponse() const
  { return itsImpl->lastResponse(); }

int Trial::numResponses() const
  { return itsImpl->numResponses(); }

double Trial::avgResponse() const
  { return itsImpl->avgResponse(); }

double Trial::avgRespTime() const
  { return itsImpl->avgRespTime(); }

void Trial::setCorrectResponse(int response)
  { itsImpl->setCorrectResponse(response); }

void Trial::add(int objid, int posid)
  { itsImpl->add(objid, posid); }

void Trial::clearObjs()
  { itsImpl->clearObjs(); }

void Trial::setType(int t)
  { itsImpl->setType(t); }

void Trial::setResponseHandler(int rhid)
  { itsImpl->setResponseHandler(rhid); }

void Trial::setTimingHdlr(int thid)
  { itsImpl->setTimingHdlr(thid); }

void Trial::clearResponses()
  { itsImpl->clearResponses(); }

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

void Trial::trDrawTrial() const
  { itsImpl->trDrawTrial(); }

void Trial::trUndrawTrial() const
  { itsImpl->trUndrawTrial(); }

void Trial::trDraw(GWT::Canvas& canvas, bool flush) const
  { itsImpl->trDraw(canvas, flush); }

void Trial::trUndraw(GWT::Canvas& canvas, bool flush) const
  { itsImpl->trUndraw(canvas, flush); }

void Trial::undoLastResponse()
  { itsImpl->undoLastResponse(); }

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
