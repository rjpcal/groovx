///////////////////////////////////////////////////////////////////////
// grobjmgr.cc
// Rob Peters
// created: Mon Mar  8 00:35:50 1999
// written: Fri Apr  9 12:24:22 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef GROBJMGR_CC_DEFINED
#define GROBJMGR_CC_DEFINED

#include "grobjmgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#include "face.h"
#include "fixpt.h"

GrObj* GrobjMgr::newGrobj(istream &is, IO::IOFlag flag) {
  string type;
  is >> type;
  // we must turn off the TYPENAME flag since the typename has already
  // been read from the input stream by the present function
  return newGrobj(type.c_str(), is, IO::IOFlag(flag & (~IO::TYPENAME)));
}

GrObj* GrobjMgr::newGrobj(const char* type, istream &is, IO::IOFlag flag) {
  if (string(type)==string(typeid(Face).name())) {
    return new Face(is, flag);
  }
  else if (string(type)==string(typeid(FixPt).name())) {
    return new FixPt(is, flag);
  }
  else {
	 throw InputError("GrobjMgr");
  }
}

static const char vcid_grobjmgr_cc[] = "$Header$";
#endif // !GROBJMGR_CC_DEFINED
