///////////////////////////////////////////////////////////////////////
//
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Mon Oct 23 17:11:55 2000
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
#include "rhlist.h"
#include "thlist.h"
#include "timinghdlr.h"

#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "gwt/canvas.h"
#include "gwt/widget.h"

#include "util/errorhandler.h"
#include "util/strings.h"

#include <strstream.h>
#include <vector>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#define TIME_TRACE

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const IO::VersionId TRIAL_SERIAL_VERSION_ID = 2;

  const string_literal ioTag("Trial");
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

  class IdPair : public Value {
  public:
	 IdPair(int o = 0, int p = 0) : itsObjid(o), itsPosid(p) {}
	 virtual ~IdPair() {}

	 virtual Value* clone() const { return new IdPair(*this); }
	 virtual Type getNativeType() const { return Value::UNKNOWN; }
	 virtual const char* getNativeTypeName() const { return "Trial::IdPair"; }

	 virtual void printTo(STD_IO::ostream& os) const
		{ os << itsObjid << " " << itsPosid; }

	 virtual void scanFrom(STD_IO::istream& is)
		{ is >> itsObjid >> itsPosid; }

	 int objid() const { return itsObjid; }
	 int posid() const { return itsPosid; }

  private:
	 int itsObjid;
	 int itsPosid;
  };

  Impl(Trial*) :
	 itsCorrectResponse(Response::ALWAYS_CORRECT),
	 itsGrObjs(),
	 itsPositions(),
	 itsResponses(),
	 itsType(-1),
	 itsRhId(0),
	 itsThId(0),
	 itsState(INACTIVE),
	 itsCanvas(0),
	 itsBlock(0)
	 {}

private:
  int itsCorrectResponse;

  std::vector<NullableItemWithId<GrObj> > itsGrObjs;
  std::vector<NullableItemWithId<Position> > itsPositions;

  std::vector<Response> itsResponses;
  int itsType;
  int itsRhId;
  int itsThId;

  TrialState itsState;

  GWT::Canvas* itsCanvas;
  Block* itsBlock;

  bool assertIdsOrHalt()
	 {
		if ( RhList::theRhList().isValidId(itsRhId) &&
			  ThList::theThList().isValidId(itsThId) ) {
		  return true;
		}

		trHaltExpt();

		return false;
	 }

  GWT::Canvas& getCanvas() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition(itsCanvas != 0);
		return *itsCanvas;
	 }

  ResponseHandler& responseHandler() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition( RhList::theRhList().isValidId(itsRhId) );
		return *(RhList::theRhList().getPtr(itsRhId));
	 }

  TimingHdlr& timingHdlr() const
	 {
		Precondition(itsState == ACTIVE);
		Precondition( ThList::theThList().isValidId(itsThId) );
		return *(ThList::theThList().getPtr(itsThId));
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

  itsGrObjs.clear();
  itsPositions.clear();

  if (svid >= 2)
	 {
		std::vector<GrObj*> grobjs;
		IO::ReadUtils::template readObjectSeq<GrObj>(
							reader, "grobjs", std::back_inserter(grobjs));

		for (int i=0; i < grobjs.size(); ++i)
		  itsGrObjs.push_back(
                     NullableItemWithId<GrObj>(grobjs[i]));

		std::vector<Position*> positions;
		IO::ReadUtils::template readObjectSeq<Position>(
							reader, "positions", std::back_inserter(positions));

		for (int j = 0; j < positions.size(); ++j)
		  itsPositions.push_back(
						   NullableItemWithId<Position>(positions[j]));
	 }
  else
	 {
		std::vector<IdPair> idpairs;
		IO::ReadUtils::template readValueObjSeq<IdPair>(reader, "idPairs",
									  std::back_inserter(idpairs));

		for (int i = 0; i < idpairs.size(); ++i)
		  {
			 itsGrObjs.push_back(
             NullableItemWithId<GrObj>(idpairs[i].objid()));
			 itsPositions.push_back(
             NullableItemWithId<Position>(idpairs[i].posid()));
		  }
	 }

  itsResponses.clear();
  IO::ReadUtils::template readValueObjSeq<Response>(reader, "responses",
									  std::back_inserter(itsResponses));

  if (svid >= 1)
	 reader->readValue("correctResponse", itsCorrectResponse);
  else
	 itsCorrectResponse = Response::ALWAYS_CORRECT;

  reader->readValue("type", itsType);
  reader->readValue("rhId", itsRhId);
  reader->readValue("thId", itsThId);
}

void Trial::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("Trial::Impl::writeTo");

  if (TRIAL_SERIAL_VERSION_ID >= 2)
	 {
		std::vector<const GrObj*> grobjs;
		for (int i=0; i < itsGrObjs.size(); ++i)
		  grobjs.push_back(itsGrObjs[i].get());

		IO::WriteUtils::writeObjectSeq(writer, "grobjs",
												 grobjs.begin(), grobjs.end());

		std::vector<const Position*> positions;
		for (int j = 0; j < itsPositions.size(); ++j)
		  positions.push_back(itsPositions[j].get());

		IO::WriteUtils::writeObjectSeq(writer, "positions",
												 positions.begin(), positions.end());
	 }
  else
	 {
		std::vector<IdPair> idpairs;

		Invariant(itsGrObjs.size() == itsPositions.size());

		for (int i = 0; i < itsGrObjs.size(); ++i)
		  {
			 idpairs.push_back(IdPair(itsGrObjs[i].id(), itsPositions[i].id()));
		  }

		IO::WriteUtils::writeValueObjSeq(writer, "idPairs",
										 idpairs.begin(), idpairs.end());
	 }

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
										 itsResponses.begin(), itsResponses.end());

  if (TRIAL_SERIAL_VERSION_ID >= 1)
	 writer->writeValue("correctResponse", itsCorrectResponse);

  writer->writeValue("type", itsType);
  writer->writeValue("rhId", itsRhId);
  writer->writeValue("thId", itsThId);
}

int Trial::Impl::readFromObjidsOnly(STD_IO::istream &is, int offset) {
DOTRACE("Trial::Impl::readFromObjidsOnly");

  itsState = INACTIVE;

  int posid = 0;
  int objid;
  if (offset == 0) {
    while (is >> objid) {
		if ( objid < 0) {
		  throw IO::ValueError(ioTag.c_str());
		}
      add(objid, posid);
      ++posid;
    }
  }
  else { // offset != 0
    while (is >> objid) {
		if ( (objid+offset) < 0 ) {
		  throw IO::ValueError(ioTag.c_str());
		}
      add(objid+offset, posid);
      ++posid;
    }
  }

  // Throw an exception if the stream has failed. However, since
  // istrstream's seem to always fail at eof, even if nothing went
  // wrong, we must only throw the exception if we have fail'ed with
  // out reaching eof. This should catch most mistakes.
  if (is.fail() && !is.eof()) throw IO::InputError(ioTag.c_str());

  // return the number of objid's read
  return posid;
}

///////////////
// accessors //
///////////////

int Trial::Impl::getResponseHandler() const {
DOTRACE("Trial::Impl::getResponseHandler");
  DebugEvalNL(itsRhId);
  return itsRhId;
}

int Trial::Impl::getTimingHdlr() const {
DOTRACE("Trial::Impl::getTimingHdlr");
  DebugEvalNL(itsThId);
  return itsThId;
}

Trial::GrObjItr Trial::Impl::beginGrObjs() const {
DOTRACE("Trial::Impl::beginIdPairs");
  return &itsGrObjs[0];
}

Trial::GrObjItr Trial::Impl::endGrObjs() const {
DOTRACE("Trial::Impl::endIdPairs");
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
  for (std::vector<Response>::const_iterator ii = itsResponses.begin();
		 ii != itsResponses.end();
		 ++ii) {
	 sum += ii->val();
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

double Trial::Impl::avgRespTime() const {
DOTRACE("Trial::Impl::avgRespTime");
  int sum = 0;
  for (std::vector<Response>::const_iterator ii = itsResponses.begin();
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
  itsGrObjs.push_back(NullableItemWithId<GrObj>(objid));
  itsPositions.push_back(NullableItemWithId<Position>(posid));

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
	 throw InvalidIdError("response handler id was negative");
  itsRhId = rhid;
}

void Trial::Impl::setTimingHdlr(int thid) {
DOTRACE("Trial::Impl::setTimingHdlr");
  if (thid < 0)
	 throw InvalidIdError("timing handler id was negative");
  itsThId = thid;
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

  if ( ThList::theThList().isValidId(itsThId) ) { 
	 timeTrace("trHaltExpt");
  }

  trUndrawTrial();
  trAbortTrial();
  trEndTrial();

  if ( RhList::theRhList().isValidId(itsRhId) ) {
	 responseHandler().rhHaltExpt();
  }

  // This must come last in order to ensure that we cancel any timer
  // callbacks that were scheduled in trAbortTrial().
  if ( ThList::theThList().isValidId(itsThId) ) { 
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
