///////////////////////////////////////////////////////////////////////
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Sun Mar 14 19:31:39 1999
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

#include "trace.h"
#define LOCAL_DEBUG
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Trial member functions
///////////////////////////////////////////////////////////////////////

Trial::Trial(istream &is, IOFlag flag, 
				 const ObjList& olist, const PosList &plist) :
  itsObjList(olist), itsPosList(plist), 
  itsObjs(), itsResponses(), itsType(-1)
{
DOTRACE("Trial::Trial");
  deserialize(is, flag);
}


IOResult Trial::serialize(ostream &os, IOFlag flag) const {
DOTRACE("Trial::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Trial).name() << sep; }

  // itsObjs
  os << itsObjs.size() << sep;
  for (ObjGrp::const_iterator ii = itsObjs.begin(); ii != itsObjs.end(); ii++) {
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
  
  // itsObjs
  itsObjs.clear();
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

bool Trial::readFromObjidsOnly(istrstream &is) {
DOTRACE("Trial::readFromObjidsOnly");
  int posid = 0;
  int objid;
  while (ist >> objid) {
	 add(objid, posid);
	 posid++;
  }
  return posid;					  // return the number of objid's read
}

const char* Trial::description() const {
DOTRACE("Trial::description");
  const int BUF_SIZE = 200;
  static char buf[BUF_SIZE];

  ostrstream ost(buf, BUF_SIZE);
  
  ost << "trial type == " << trialType()
		<< ", objs ==";
  for (int i = 0; i < itsObjs.size(); i++) {
	 ost << " " << itsObjs[i].first;
  }
  ost << ", categories ==";
  for (int j = 0; j < itsObjs.size(); j++) {
	 GrObj *obj = itsObjList.getObj(itsObjs[j].first);
	 ost << " " << obj->getCategory();
  }
  ost << '\0';

  return buf;
}

void Trial::action() const {
DOTRACE("Trial::action");
  for (int i = 0; i < itsObjs.size(); i++) {
	 GrObj *obj = itsObjList.getObj(itsObjs[i].first);
	 Position *pos = itsPosList.getPos(itsObjs[i].second);
#ifdef LOCAL_DEBUG
	 DUMP_VAL1((void *) obj);
	 DUMP_VAL2((void *) pos);
#endif
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
