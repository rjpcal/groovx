///////////////////////////////////////////////////////////////////////
//
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Wed May 10 12:35:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include "experiment.h"
#include "objlist.h"
#include "poslist.h"
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

#include "util/strings.h"

#include <iostream.h>
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
  const string_literal ioTag("Trial");
}

///////////////////////////////////////////////////////////////////////
//
// Trial::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Trial::Impl {
public:
  Impl() :
	 itsIdPairs(), itsResponses(), itsType(-1),
	 itsRhId(0), itsThId(0)
	 {}

  vector<IdPair> itsIdPairs;
  vector<Response> itsResponses;
  int itsType;
  int itsRhId;
  int itsThId;

  bool checkIds() const
	 {
		if ( RhList::theRhList().isValidId(itsRhId) &&
			  ThList::theThList().isValidId(itsThId) ) {
		  return true;
		}
		return false;
	 }

  bool assertIdsOrHalt(Experiment& expt) const
	 {
		if ( checkIds() ) return true;

		// ...else halt any of the participants for which we have valid id's
		expt.edHaltExpt();

		return false;
	 }

  ResponseHandler& responseHandler() const
	 {
		Assert( RhList::theRhList().isValidId(itsRhId) );
		return *(RhList::theRhList().getPtr(itsRhId));
	 }

  TimingHdlr& timingHdlr() const
	 {
		Assert( ThList::theThList().isValidId(itsThId) );
		return *(ThList::theThList().getPtr(itsThId));
	 }


#ifdef TIME_TRACE
  inline void timeTrace(const char* loc) {
	 cerr << "in " << loc
			<< ", elapsed time == " << timingHdlr().getElapsedMsec() << endl;
  }
#else
  inline void timeTrace(const char*) {}
#endif
};

///////////////////////////////////////////////////////////////////////
//
// Trial::IdPair member functions
//
///////////////////////////////////////////////////////////////////////

Value* Trial::IdPair::clone() const {
DOTRACE("Trial::IdPair::clone");
  return new IdPair(*this); 
}

Value::Type Trial::IdPair::getNativeType() const {
DOTRACE("Trial::IdPair::getNativeType");
  return Value::UNKNOWN;
}

const char* Trial::IdPair::getNativeTypeName() const {
DOTRACE("Trial::IdPair::getNativeTypeName");
  return "Trial::IdPair";
}

void Trial::IdPair::printTo(ostream& os) const {
DOTRACE("Trial::IdPair::printTo");
  os << objid << " " << posid;
}

void Trial::IdPair::scanFrom(istream& is) {
DOTRACE("Trial::IdPair::scanFrom");
  is >> objid >> posid;
}

///////////////////////////////////////////////////////////////////////
//
// Trial member functions
//
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Trial::Trial() : 
  itsImpl( new Impl )
{
DOTRACE("Trial::Trial()");
}

Trial::Trial(istream &is, IO::IOFlag flag) :
  itsImpl( new Impl )
{
DOTRACE("Trial::Trial(istream&, IO::IOFlag)");
  deserialize(is, flag);
}

Trial::~Trial() {
DOTRACE("Trial::~Trial");
  delete itsImpl;
}

void Trial::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("Trial::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }

  // itsImpl->itsIdPairs
  os << itsImpl->itsIdPairs.size() << sep;
  for (vector<IdPair>::const_iterator ii = itsImpl->itsIdPairs.begin(); 
       ii != itsImpl->itsIdPairs.end(); 
       ++ii) {
    os << ii->objid << sep << ii->posid << sep << sep;
  }
  // itsImpl->itsResponses
  os << itsImpl->itsResponses.size() << sep << sep;
  for (size_t i = 0; i < itsImpl->itsResponses.size(); ++i) {
	 itsImpl->itsResponses[i].printTo(os);
  }
  // itsImpl->itsType
  os << itsImpl->itsType << sep;

  // itsImpl->itsRhId
  os << itsImpl->itsRhId << sep;
  // itsImpl->itsThId
  os << itsImpl->itsThId << endl;

  if (os.fail()) throw IO::OutputError(ioTag.c_str());
}

void Trial::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("Trial::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag.c_str()); }
  
  // itsImpl->itsIdPairs
  itsImpl->itsIdPairs.clear();
  int size;
  is >> size;
  if (size < 0) {
	 throw IO::ValueError(ioTag.c_str());
  }
  int objid;
  int posid;
  for (int i = 0; i < size; ++i) {
    is >> objid >> posid;
	 if ( !ObjList::theObjList().isValidId(objid) ||
			!PosList::thePosList().isValidId(posid) ) {
		throw IO::ValueError(ioTag.c_str());
	 }
    add(objid, posid);
  }
  // itsImpl->itsResponses
  int resp_size;
  is >> resp_size;
  itsImpl->itsResponses.resize(resp_size);
  for (int j = 0; j < resp_size; ++j) {
	 itsImpl->itsResponses[j].scanFrom(is);
  }
  // itsImpl->itsType
  is >> itsImpl->itsType;

  // itsImpl->itsRhId
  is >> itsImpl->itsRhId; DebugEvalNL(itsImpl->itsRhId);
  // itsImpl->itsThId
  is >> itsImpl->itsThId; DebugEvalNL(itsImpl->itsThId);

  if (is.fail()) { throw IO::InputError(ioTag.c_str()); }
}

int Trial::charCount() const {
  int count = (ioTag.length() + 1
					+ IO::gCharCount<int>(itsImpl->itsIdPairs.size()) + 1);
  for (vector<IdPair>::const_iterator ii = itsImpl->itsIdPairs.begin(); 
       ii != itsImpl->itsIdPairs.end(); 
       ++ii) {
	 count += (IO::gCharCount<int>(ii->objid) + 1
				  + IO::gCharCount<int>(ii->posid) + 2);
  }
  count += (IO::gCharCount<int>(itsImpl->itsResponses.size()) + 2);
  for (size_t i = 0; i < itsImpl->itsResponses.size(); ++i) {
	 count += (IO::gCharCount<int>(itsImpl->itsResponses[i].val()) + 1
				  + IO::gCharCount<int>(itsImpl->itsResponses[i].msec()) + 2);
  }
  count += 
	 IO::gCharCount<int>(itsImpl->itsType) + 1
	 + IO::gCharCount<int>(itsImpl->itsRhId) + 1
	 + IO::gCharCount<int>(itsImpl->itsThId) + 1;
  return (count + 1);// fudge factor (1)
}					

void Trial::readFrom(IO::Reader* reader) {
DOTRACE("Trial::readFrom");
  reader->readValue("type", itsImpl->itsType);
  reader->readValue("rhId", itsImpl->itsRhId);
  reader->readValue("thId", itsImpl->itsThId);

  itsImpl->itsResponses.clear();
  IO::ReadUtils::readValueObjSeq(reader, "responses",
									  back_inserter(itsImpl->itsResponses), (Response*) 0);

  itsImpl->itsIdPairs.clear();
  IO::ReadUtils::readValueObjSeq(reader, "idPairs",
									  back_inserter(itsImpl->itsIdPairs), (IdPair*) 0);
}

void Trial::writeTo(IO::Writer* writer) const {
DOTRACE("Trial::writeTo");
  writer->writeValue("type", itsImpl->itsType);
  writer->writeValue("rhId", itsImpl->itsRhId);
  writer->writeValue("thId", itsImpl->itsThId);

  IO::WriteUtils::writeValueObjSeq(writer, "responses",
										 itsImpl->itsResponses.begin(), itsImpl->itsResponses.end());
  IO::WriteUtils::writeValueObjSeq(writer, "idPairs",
										 itsImpl->itsIdPairs.begin(), itsImpl->itsIdPairs.end());
}

int Trial::readFromObjidsOnly(istream &is, int offset) {
DOTRACE("Trial::readFromObjidsOnly");
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

int Trial::getAutosavePeriod() const {
DOTRACE("Trial::getAutosavePeriod");
  if ( itsImpl->checkIds() )
	 return itsImpl->timingHdlr().getAutosavePeriod();
  else return 0;
}

int Trial::getResponseHandler() const {
DOTRACE("Trial::getResponseHandler");
  DebugEvalNL(itsImpl->itsRhId);
  return itsImpl->itsRhId;
}

int Trial::getTimingHdlr() const {
DOTRACE("Trial::getTimingHdlr");
  DebugEvalNL(itsImpl->itsThId);
  return itsImpl->itsThId;
}

Trial::IdPairItr Trial::beginIdPairs() const {
DOTRACE("Trial::beginIdPairs");
  return &itsImpl->itsIdPairs[0];
}

Trial::IdPairItr Trial::endIdPairs() const {
DOTRACE("Trial::endIdPairs");
  return beginIdPairs() + itsImpl->itsIdPairs.size();
}

int Trial::trialType() const {
DOTRACE("Trial::trialType");
  return itsImpl->itsType;
} 

const char* Trial::description() const {
DOTRACE("Trial::description");
  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];

  ostrstream ost(buf, BUF_SIZE);
  
  ost << "trial type == " << trialType()
      << ", objs ==";
  for (size_t i = 0; i < itsImpl->itsIdPairs.size(); ++i) {
    ost << " " << itsImpl->itsIdPairs[i].objid;
  }
  ost << ", categories ==";
  for (size_t j = 0; j < itsImpl->itsIdPairs.size(); ++j) {
    DebugEvalNL(itsImpl->itsIdPairs[j].objid);

    ObjList::Ptr obj = ObjList::theObjList().getCheckedPtr(itsImpl->itsIdPairs[j].objid);
    Assert(obj.get() != 0);

    ost << " " << obj->getCategory();
  }
  ost << '\0';

  return buf;
}

int Trial::lastResponse() const {
DOTRACE("Trial::lastResponse");
  return itsImpl->itsResponses.back().val();
}

int Trial::numResponses() const {
DOTRACE("Trial::numResponses");
  return itsImpl->itsResponses.size();
}

double Trial::avgResponse() const {
DOTRACE("Trial::avgResponse");
  int sum = 0;
  for (vector<Response>::const_iterator ii = itsImpl->itsResponses.begin();
		 ii != itsImpl->itsResponses.end();
		 ++ii) {
	 sum += ii->val();
  }
  return (itsImpl->itsResponses.size() > 0) ? double(sum)/itsImpl->itsResponses.size() : 0.0;
}

double Trial::avgRespTime() const {
DOTRACE("Trial::avgRespTime");
  int sum = 0;
  for (vector<Response>::const_iterator ii = itsImpl->itsResponses.begin();
		 ii != itsImpl->itsResponses.end();
		 ++ii) {
	 sum += ii->msec();

	 DebugEval(sum);
	 DebugEvalNL(sum/itsImpl->itsResponses.size());
  }
  return (itsImpl->itsResponses.size() > 0) ? double(sum)/itsImpl->itsResponses.size() : 0.0;
}

//////////////////
// manipulators //
//////////////////

void Trial::add(int objid, int posid) {
DOTRACE("Trial::add");
  itsImpl->itsIdPairs.push_back(IdPair(objid, posid));
}

void Trial::clearObjs() {
DOTRACE("Trial::clearObjs");
  itsImpl->itsIdPairs.clear(); 
}

void Trial::setType(int t) {
DOTRACE("Trial::setType");
  itsImpl->itsType = t;
}

void Trial::setResponseHandler(int rhid) {
DOTRACE("Trial::setResponseHandler");
  if (rhid < 0)
	 throw InvalidIdError("response handler id was negative");
  itsImpl->itsRhId = rhid;
}

void Trial::setTimingHdlr(int thid) {
DOTRACE("setTimingHdlr");
  if (thid < 0)
	 throw InvalidIdError("timing handler id was negative");
  itsImpl->itsThId = thid;
}

void Trial::recordResponse(const Response& response) {
DOTRACE("Trial::recordResponse"); 
  itsImpl->timeTrace("recordResponse");
  itsImpl->itsResponses.push_back(response);
}

void Trial::clearResponses() {
DOTRACE("Trial::clearResponses");
  itsImpl->itsResponses.clear();
}

/////////////
// actions //
/////////////

void Trial::trDoTrial(Experiment& expt) {
DOTRACE("Trial::trDoTrial");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return;

  itsImpl->timeTrace("trDoTrial"); 

  itsImpl->timingHdlr().thBeginTrial(&expt);
  itsImpl->responseHandler().rhBeginTrial(&expt);
}

int Trial::trElapsedMsec(Experiment& expt) {
DOTRACE("Trial::trElapsedMsec");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return -1;

  return itsImpl->timingHdlr().getElapsedMsec();
}

void Trial::trAbortTrial(Experiment& expt) {
DOTRACE("Trial::trAbortTrial");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return;

  itsImpl->timeTrace("trAbortTrial");

  itsImpl->responseHandler().rhAbortTrial(&expt);
  itsImpl->timingHdlr().thAbortTrial(&expt);
}

void Trial::trEndTrial(Experiment& expt) {
DOTRACE("Trial::trEndTrial");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return;

  itsImpl->timeTrace("trEndTrial");

  itsImpl->responseHandler().rhEndTrial(&expt);
}

void Trial::trHaltExpt(Experiment& expt) {
DOTRACE("Trial::trHaltExpt");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return;

  if ( ThList::theThList().isValidId(itsImpl->itsThId) ) { 
	 itsImpl->timeTrace("edHaltExpt");
  }

  if ( RhList::theRhList().isValidId(itsImpl->itsRhId) ) {
	 itsImpl->responseHandler().rhHaltExpt(&expt);
  }
  if ( ThList::theThList().isValidId(itsImpl->itsThId) ) {
	 itsImpl->timingHdlr().thHaltExpt(&expt);
  }
}

void Trial::trResponseSeen(Experiment& expt) {
DOTRACE("Trial::trResponseSeen");
  if ( !itsImpl->assertIdsOrHalt(expt) ) return;

  itsImpl->timeTrace("trResponseSeen");

  itsImpl->timingHdlr().thResponseSeen(&expt);
}

void Trial::trDraw(GWT::Canvas& canvas, bool flush) const {
DOTRACE("Trial::trDraw");
  for (size_t i = 0; i < itsImpl->itsIdPairs.size(); ++i) {
    ObjList::Ptr obj =
		ObjList::theObjList().getCheckedPtr(itsImpl->itsIdPairs[i].objid);
    PosList::Ptr pos =
		PosList::thePosList().getCheckedPtr(itsImpl->itsIdPairs[i].posid);

    DebugEval(itsImpl->itsIdPairs[i].objid);
    DebugEvalNL((void *) obj);
    DebugEval(itsImpl->itsIdPairs[i].posid);
    DebugEvalNL((void *) pos);


	 { 
		GWT::Canvas::StateSaver state(canvas);
		pos->go();
		obj->draw(canvas);
	 }
  }

  if (flush) canvas.flushOutput();
}

void Trial::trUndraw(GWT::Canvas& canvas, bool flush) const {
DOTRACE("Trial::trUndraw");
  for (size_t i = 0; i < itsImpl->itsIdPairs.size(); ++i) {
    ObjList::Ptr obj =
		ObjList::theObjList().getCheckedPtr(itsImpl->itsIdPairs[i].objid);
    PosList::Ptr pos =
		PosList::thePosList().getCheckedPtr(itsImpl->itsIdPairs[i].posid);

	 {
		GWT::Canvas::StateSaver state(canvas);
		pos->rego();
		obj->undraw(canvas);
	 }
  }

  if (flush) canvas.flushOutput();
}

void Trial::undoLastResponse() {
DOTRACE("Trial::undoLastResponse");
  if ( !itsImpl->itsResponses.empty() )
	 itsImpl->itsResponses.pop_back();
}

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
