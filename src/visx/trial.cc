///////////////////////////////////////////////////////////////////////
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Tue Mar 16 19:21:13 1999
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

Trial::Trial(istream &is, IOFlag flag, 
             const ObjList& olist, const PosList &plist) :
  itsObjList(olist), itsPosList(plist), 
  itsIdPairs(), itsResponses(), itsType(-1)
{
DOTRACE("Trial::Trial");
  deserialize(is, flag);
}


IOResult Trial::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Trial::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Trial).name() << sep; }

  // itsIdPairs
  os << itsIdPairs.size() << sep;
  for (ObjGrp::const_iterator ii = itsIdPairs.begin(); 
       ii != itsIdPairs.end(); 
       ii++) {
    os << (*ii).first << sep << (*ii).second << sep << sep;
  }
  // itsResponses
  serializeVecInt(os, itsResponses);
  // itsType
  os << itsType << endl;

  return checkStream(os);
}

IOResult Trial::deserialize(istream &is, IOFlag flag) {
DOTRACE("Trial::deserialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }
  if (flag & IO::TYPENAME) {
    string name;
    is >> name;
    if (name != string(typeid(Trial).name())) { return IO_ERROR; }
  }
  
  // itsIdPairs
  itsIdPairs.clear();
  int size;
  is >> size;
  int objid, posid;
  for (int i = 0; i < size; i++) {
    is >> objid >> posid;
    add(objid, posid);
  }
  // itsResponses
  deserializeVecInt(is, itsResponses);
  // itsType
  is >> itsType;
  return checkStream(is);
}

int Trial::readFromObjidsOnly(istrstream &ist, int offset) {
DOTRACE("Trial::readFromObjidsOnly");
  int posid = 0;
  int objid;
  if (offset == 0) {
    while (ist >> objid) {
      add(objid, posid);
      posid++;
    }
  }
  else { // offset != 0
    while (ist >> objid) {
      add(objid+offset, posid);
      posid++;
    }
  }
  return posid;                 // return the number of objid's read
}

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

void Trial::action() const {
DOTRACE("Trial::action");
  for (int i = 0; i < itsIdPairs.size(); i++) {
    GrObj *obj = itsObjList.getObj(itsIdPairs[i].first);
    Position *pos = itsPosList.getPos(itsIdPairs[i].second);
#ifdef LOCAL_DEBUG
    DUMP_VAL1(itsIdPairs[i].first);
    DUMP_VAL2((void *) obj);
    DUMP_VAL1(itsIdPairs[i].second);
    DUMP_VAL2((void *) pos);
#endif
    Assert(obj);
    Assert(pos);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
      pos->translate();
      pos->scale();
      pos->rotate();
      obj->grAction();
    glPopMatrix();
  }
}

static const char vcid_trial_cc[] = "$Header$";
#endif // !TRIAL_CC_DEFINED
