///////////////////////////////////////////////////////////////////////
// trial.cc
// Rob Peters
// created: Fri Mar 12 17:43:21 1999
// written: Sat Mar 13 15:41:13 1999
///////////////////////////////////////////////////////////////////////

#ifndef TRIAL_CC_DEFINED
#define TRIAL_CC_DEFINED

#include "trial.h"

#include <iostream.h>
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
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Trial class declaration
///////////////////////////////////////////////////////////////////////

IOResult Trial::serialize(ostream &os, IOFlag flag = NO_FLAGS) const {
DOTRACE("Trial::serialize");
  if (flag & IO::BASES) { /* there are no bases to deserialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << typeid(Position).name() << sep; }

  // itsObjs
  os << itsObjs.size() << sep;
  for (ObjGrp::iterator ii = itsObjs.begin(); ii != itsObjs.end(); ii++) {
	 os << (*ii).first << sep << (*ii).second << sep << sep;
  }
  // itsResponses
  serializeVecInt(os, itsResponses);
  // itsType
  os << itsType << endl;

  return checkStream(os);
}

IOResult Trial::deserialize(istream &is, IOFlag flag = NO_FLAGS) {
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
	 itsObjs.push_back(ObjPos(objid, posid));
  }
  // itsResponses
  deserializeVecInt(is, itsResponses);
  // itsType
  is >> itsType;
  return checkStream(is);
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

#endif // !TRIAL_CC_DEFINED
