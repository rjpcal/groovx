///////////////////////////////////////////////////////////////////////
//
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Mon Dec  6 20:05:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <GL/gl.h>

#include "objlist.h"
#include "poslist.h"
#include "grobj.h"
#include "position.h"
#include "reader.h"
#include "readutils.h"
#include "writer.h"
#include "writeutils.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "Trial";
}

///////////////////////////////////////////////////////////////////////
//
// Trial::Response member functions
//
///////////////////////////////////////////////////////////////////////

Value* Trial::Response::clone() const {
DOTRACE("Trial::Response::clone");
  return new Response(*this); 
}

Value::Type Trial::Response::getNativeType() const {
DOTRACE("Trial::Response::getNativeType");
  return Value::UNKNOWN;
}

const char* Trial::Response::getNativeTypeName() const {
DOTRACE("Trial::Response::getNativeTypeName");
  return "Trial::Response";
}

void Trial::Response::printTo(ostream& os) const {
DOTRACE("Trial::Response::printTo");
  os << itsVal << " " << itsMsec; 
}

void Trial::Response::scanFrom(istream& is) {
DOTRACE("Trial::Response::scanFrom");
  is >> itsVal >> itsMsec;
}

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
  itsIdPairs(), itsResponses(), itsType(-1),
  itsRhId(0), itsThId(0)
{
DOTRACE("Trial::Trial()");
}

Trial::Trial(istream &is, IOFlag flag) :
  itsIdPairs(), itsResponses(), itsType(-1),
  itsRhId(0), itsThId(0)
{
DOTRACE("Trial::Trial(istream&, IOFlag)");
  deserialize(is, flag);
}

Trial::~Trial() {
DOTRACE("Trial::~Trial");
}

void Trial::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Trial::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  // itsIdPairs
  os << itsIdPairs.size() << sep;
  for (ObjGrp::const_iterator ii = itsIdPairs.begin(); 
       ii != itsIdPairs.end(); 
       ++ii) {
    os << ii->objid << sep << ii->posid << sep << sep;
  }
  // itsResponses
  os << itsResponses.size() << sep << sep;
  for (size_t i = 0; i < itsResponses.size(); ++i) {
	 os << itsResponses[i].val() << sep << itsResponses[i].msec() << sep;
  }
  // itsType
  os << itsType << sep;

  // itsRhId
  os << itsRhId << sep;
  // itsThId
  os << itsThId << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void Trial::deserialize(istream &is, IOFlag flag) {
DOTRACE("Trial::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }
  
  // itsIdPairs
  itsIdPairs.clear();
  int size;
  is >> size;
  if (size < 0) {
	 throw IoValueError(ioTag);
  }
  int objid;
  int posid;
  for (int i = 0; i < size; ++i) {
    is >> objid >> posid;
	 if ( !ObjList::theObjList().isValidId(objid) ||
			!PosList::thePosList().isValidId(posid) ) {
		throw IoValueError(ioTag);
	 }
    add(objid, posid);
  }
  // itsResponses
  int resp_size;
  is >> resp_size;
  itsResponses.resize(resp_size);
  for (int j = 0; j < resp_size; ++j) {
	 is >> itsResponses[j].val() >> itsResponses[j].msec();
  }
  // itsType
  is >> itsType;

  // itsRhId
  is >> itsRhId; DebugEvalNL(itsRhId);
  // itsThId
  is >> itsThId; DebugEvalNL(itsThId);

  if (is.fail()) { throw InputError(ioTag); }
}

int Trial::charCount() const {
  int count = (ioTag.length() + 1
					+ gCharCount<int>(itsIdPairs.size()) + 1);
  for (ObjGrp::const_iterator ii = itsIdPairs.begin(); 
       ii != itsIdPairs.end(); 
       ++ii) {
	 count += (gCharCount<int>(ii->objid) + 1
				  + gCharCount<int>(ii->posid) + 2);
  }
  count += (gCharCount<int>(itsResponses.size()) + 2);
  for (size_t i = 0; i < itsResponses.size(); ++i) {
	 count += (gCharCount<int>(itsResponses[i].val()) + 1
				  + gCharCount<int>(itsResponses[i].msec()) + 2);
  }
  count += 
	 gCharCount<int>(itsType) + 1
	 + gCharCount<int>(itsRhId) + 1
	 + gCharCount<int>(itsThId) + 1;
  return (count + 1);// fudge factor (1)
}					

void Trial::readFrom(Reader* reader) {
DOTRACE("Trial::readFrom");
  reader->readValue("type", itsType);
  reader->readValue("rhId", itsRhId);
  reader->readValue("thId", itsThId);

  itsResponses.clear();
  ReadUtils::readValueObjSeq(reader, "responses",
									  back_inserter(itsResponses), (Response*) 0);

  itsIdPairs.clear();
  ReadUtils::readValueObjSeq(reader, "idPairs",
									  back_inserter(itsIdPairs), (IdPair*) 0);
}

void Trial::writeTo(Writer* writer) const {
DOTRACE("Trial::writeTo");
  writer->writeValue("type", itsType);
  writer->writeValue("rhId", itsRhId);
  writer->writeValue("thId", itsThId);

  WriteUtils::writeValueObjSeq(writer, "responses",
										 itsResponses.begin(), itsResponses.end());
  WriteUtils::writeValueObjSeq(writer, "idPairs",
										 itsIdPairs.begin(), itsIdPairs.end());
}

int Trial::readFromObjidsOnly(istream &is, int offset) {
DOTRACE("Trial::readFromObjidsOnly");
  int posid = 0;
  int objid;
  if (offset == 0) {
    while (is >> objid) {
		if ( objid < 0) {
		  throw IoValueError(ioTag);
		}
      add(objid, posid);
      ++posid;
    }
  }
  else { // offset != 0
    while (is >> objid) {
		if ( (objid+offset) < 0 ) {
		  throw IoValueError(ioTag);
		}
      add(objid+offset, posid);
      ++posid;
    }
  }

  // Throw an exception if the stream has failed. However, since
  // istrstream's seem to always fail at eof, even if nothing went
  // wrong, we must only throw the exception if we have fail'ed with
  // out reaching eof. This should catch most mistakes.
  if (is.fail() && !is.eof()) throw InputError(ioTag);

  // return the number of objid's read
  return posid;
}

///////////////
// accessors //
///////////////

int Trial::getResponseHandler() const {
DOTRACE("Trial::getResponseHandler");
  DebugEvalNL(itsRhId);
  return itsRhId;
}

int Trial::getTimingHdlr() const {
DOTRACE("Trial::getTimingHdlr");
  DebugEvalNL(itsThId);
  return itsThId;
}

const char* Trial::description() const {
DOTRACE("Trial::description");
  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];

  ostrstream ost(buf, BUF_SIZE);
  
  ost << "trial type == " << trialType()
      << ", objs ==";
  for (size_t i = 0; i < itsIdPairs.size(); ++i) {
    ost << " " << itsIdPairs[i].objid;
  }
  ost << ", categories ==";
  for (size_t j = 0; j < itsIdPairs.size(); ++j) {
    DebugEvalNL(itsIdPairs[j].objid);

    GrObj* obj = ObjList::theObjList().getCheckedPtr(itsIdPairs[j].objid);
    Assert(obj);

    ost << " " << obj->getCategory();
  }
  ost << '\0';

  return buf;
}

double Trial::avgResponse() const {
DOTRACE("Trial::avgResponse");
  int sum = 0;
  for (vector<Response>::const_iterator ii = itsResponses.begin();
		 ii != itsResponses.end();
		 ++ii) {
	 sum += ii->val();
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

double Trial::avgRespTime() const {
DOTRACE("Trial::avgRespTime");
  int sum = 0;
  for (vector<Response>::const_iterator ii = itsResponses.begin();
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

void Trial::clearObjs() {
DOTRACE("Trial::clearObjs");
  itsIdPairs.clear(); 
}

void Trial::setResponseHandler(int rhid) {
DOTRACE("Trial::setResponseHandler");
  itsRhId = rhid;
}

void Trial::setTimingHdlr(int thid) {
DOTRACE("setTimingHdlr");
  itsThId = thid;
}

void Trial::clearResponses() {
DOTRACE("Trial::clearResponses");
  itsResponses.clear();
}

/////////////
// actions //
/////////////

void Trial::trDraw(Canvas& canvas, bool flush) const {
DOTRACE("Trial::trDraw");
  for (size_t i = 0; i < itsIdPairs.size(); ++i) {
    GrObj* obj =
		ObjList::theObjList().getCheckedPtr(itsIdPairs[i].objid);
    Position* pos =
		PosList::thePosList().getCheckedPtr(itsIdPairs[i].posid);

    DebugEval(itsIdPairs[i].objid);
    DebugEvalNL((void *) obj);
    DebugEval(itsIdPairs[i].posid);
    DebugEvalNL((void *) pos);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	   pos->go();
      obj->draw();
    glPopMatrix();
  }

  if (flush) glFlush();
}

void Trial::trUndraw(Canvas& canvas, bool flush) const {
DOTRACE("Trial::trUndraw");
  for (size_t i = 0; i < itsIdPairs.size(); ++i) {
    GrObj* obj =
		ObjList::theObjList().getCheckedPtr(itsIdPairs[i].objid);
    Position* pos =
		PosList::thePosList().getCheckedPtr(itsIdPairs[i].posid);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	   pos->rego();
      obj->undraw();
    glPopMatrix();
  }

  if (flush) glFlush();
}

void Trial::undoLastResponse() {
DOTRACE("Trial::undoLastResponse");
  if ( !itsResponses.empty() )
	 itsResponses.pop_back();
}

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
