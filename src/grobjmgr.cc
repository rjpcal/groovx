///////////////////////////////////////////////////////////////////////
// grobjmgr.cc
// Rob Peters
// created: Mon Mar  8 00:35:50 1999
// written: Fri Mar 12 11:03:17 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_CC_DEFINED
#define GROBJMGR_CC_DEFINED

#include "grobjmgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#include "face.h"
#include "fixpt.h"

GrObj* GrobjMgr::newGrobj(istream &is) {
  string type;
  is >> type;
  return newGrobj(type.c_str(), is);
}

GrObj* GrobjMgr::newGrobj(const char* type, istream &is) {
  if (string(type)==string(typeid(Face).name())) {
	 return new Face(is);
  }
  else if (string(type)==string(typeid(FixPt).name())) {
	 return new FixPt(is);
  }
  else {
	 return NULL;
  }
}

#endif // !GROBJMGR_CC_DEFINED
