///////////////////////////////////////////////////////////////////////
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Sun Apr 25 13:19:25 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include <iostream.h>
#include <strstream.h>
#include <string>
#include <typeinfo>
#include <GL/gl.h>

#include "iostl.h"
#include "objlist.h"
#include "poslist.h"
#include "grobj.h"
#include "position.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Trial member functions
///////////////////////////////////////////////////////////////////////

//////////////
// creators //
//////////////

Trial::Trial(istream &is, IOFlag flag, 
             const ObjList& olist, const PosList &plist) :
  itsObjList(olist), itsPosList(plist), 
  itsIdPairs(), itsResponses(), itsType(-1)
{
DOTRACE("Trial::Trial");
  deserialize(is, flag);
}


void Trial::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Trial::serialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << typeid(Trial).name() << sep; }

  // itsIdPairs
  os << itsIdPairs.size() << sep;
  for (ObjGrp::const_iterator ii = itsIdPairs.begin(); 
       ii != itsIdPairs.end(); 
       ii++) {
    os << (*ii).first << sep << (*ii).second << sep << sep;
  }
  // itsResponses
  os << itsResponses.size() << sep << sep;
  for (int i = 0; i < itsResponses.size(); i++) {
	 os << itsResponses[i].val << sep << itsResponses[i].msec << sep;
  }
  // itsType
  os << itsType << endl;

  if (os.fail()) throw OutputError(typeid(Trial));
}

void Trial::deserialize(istream &is, IOFlag flag) {
DOTRACE("Trial::deserialize");
  if (flag & BASES) { /* there are no bases to deserialize */ }
  if (flag & TYPENAME) {
    string name;
    is >> name;
    if (name != typeid(Trial).name()) { 
		throw InputError(typeid(Trial));
	 }
  }
  
  // itsIdPairs
  itsIdPairs.clear();
  int size;
  is >> size;
  if (size < 0) {
	 throw IoValueError(typeid(Trial));
  }
  int objid, posid;
  for (int i = 0; i < size; i++) {
    is >> objid >> posid;
	 if (objid < 0 || posid < 0) {
		throw IoValueError(typeid(Trial));
	 }
    add(objid, posid);
  }
  // itsResponses
  int resp_size;
  is >> resp_size;
  itsResponses.resize(resp_size);
  for (int j = 0; j < resp_size; j++) {
	 is >> itsResponses[j].val >> itsResponses[j].msec;
  }
  // itsType
  is >> itsType;
  if (is.fail()) throw InputError(typeid(Trial));
}

int Trial::readFromObjidsOnly(istream &is, int offset) {
DOTRACE("Trial::readFromObjidsOnly");
  int posid = 0;
  int objid;
  if (offset == 0) {
    while (is >> objid) {
		if (objid < 0) {
		  throw IoValueError(typeid(Trial));
		}
      add(objid, posid);
      posid++;
    }
  }
  else { // offset != 0
    while (is >> objid) {
		if ( (objid+offset) < 0) {
		  throw IoValueError(typeid(Trial));
		}
      add(objid+offset, posid);
      posid++;
    }
  }

  // Throw an exception if the stream has failed. However, since
  // istrstream's seem to always fail at eof, even if nothing went
  // wrong, we must only throw the exception if we have fail'ed with
  // out reaching eof. This should catch most mistakes.
  if (is.fail() && !is.eof()) throw InputError(typeid(Trial));

  // return the number of objid's read
  return posid;
}

///////////////
// accessors //
///////////////

const char* Trial::description() const {
DOTRACE("Trial::description");
  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];

  ostrstream ost(buf, BUF_SIZE);
  
  ost << "trial type == " << trialType()
      << ", objs ==";
  for (int i = 0; i < itsIdPairs.size(); i++) {
    ost << " " << itsIdPairs[i].first;
  }
  ost << ", categories ==";
  for (int j = 0; j < itsIdPairs.size(); j++) {
    GrObj *obj = itsObjList.getObj(itsIdPairs[j].first);
#ifdef LOCAL_DEBUG
    DUMP_VAL2(itsIdPairs[j].first);
#endif
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
		 ii++) {
	 sum += ii->val;
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

double Trial::avgRespTime() const {
DOTRACE("Trial::avgRespTime");
  int sum = 0;
  for (vector<Response>::const_iterator ii = itsResponses.begin();
		 ii != itsResponses.end();
		 ii++) {
	 sum += ii->msec;
#ifdef LOCAL_DEBUG
	 DUMP_VAL1(sum);
	 DUMP_VAL2(sum/itsResponses.size());
#endif
  }
  return (itsResponses.size() > 0) ? double(sum)/itsResponses.size() : 0.0;
}

/////////////
// actions //
/////////////

void Trial::action() const {
DOTRACE("Trial::action");
  for (int i = 0; i < itsIdPairs.size(); i++) {
    GrObj *obj = itsObjList.getObj(itsIdPairs[i].first);
    Assert(obj);
    Position *pos = itsPosList.getPos(itsIdPairs[i].second);
    Assert(pos);
#ifdef LOCAL_DEBUG
    DUMP_VAL1(itsIdPairs[i].first);
    DUMP_VAL2((void *) obj);
    DUMP_VAL1(itsIdPairs[i].second);
    DUMP_VAL2((void *) pos);
#endif

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//       pos->translate();
//       pos->scale();
//       pos->rotate();
	   pos->go();
      obj->grAction();
    glPopMatrix();
  }
}

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
