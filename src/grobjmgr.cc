///////////////////////////////////////////////////////////////////////
// grobjmgr.cc
// Rob Peters
// created: Mon Mar  8 00:35:50 1999
// written: Mon Mar  8 17:29:13 1999
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_CC_DEFINED
#define GROBJMGR_CC_DEFINED

#include "grobjmgr.h"

#include <iostream.h>
#include <string>

#include "face.h"
#include "fixpt.h"

GrObj* GrobjMgr::newGrobj(istream &is) {
  string type;
  is >> type;
  return newGrobj(type.c_str(), is);
}

GrObj* GrobjMgr::newGrobj(const char* type, istream &is) {
  if (string(type)==string("Face")) {
	 return new Face(is);
  }
  else if (string(type)==string("FixPt")) {
	 return new FixPt(is);
  }
  else {
	 return NULL;
  }
}

#endif // !GROBJMGR_CC_DEFINED
