///////////////////////////////////////////////////////////////////////
// posmgr.cc
// Rob Peters
// created: Wed Apr  7 14:32:43 1999
// written: Fri Apr  9 12:10:17 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSMGR_CC_DEFINED
#define POSMGR_CC_DEFINED

#include "posmgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#include "position.h"
#include "jitter.h"

#define NO_TRACE
#include "trace.h"

#define LOCAL_ASSERT
#include "debug.h"

Position* PosMgr::newPosition(istream &is, IO::IOFlag flag) {
DOTRACE("PosMgr::newPosition(istream &is, IO::IOFlag flag)");
  string type;
  is >> type;

  return newPosition(type.c_str(), is, IO::IOFlag(flag & (~IO::TYPENAME)));
}

Position* PosMgr::newPosition(const char* type, istream &is, IO::IOFlag flag) {
DOTRACE("PosMgr::newPosition(const char* type, istream &is, IO::IOFlag flag)");
  if (string(type)==string(typeid(Position).name())) {
	 return new Position(is, flag);
  }
  else if (string(type)==string(typeid(Jitter).name())) {
	 return new Jitter(is, flag);
  }
  else {
	 throw InputError("PosMgr");
  }
}


static const char vcid_posmgr_cc[] = "$Header$";
#endif // !POSMGR_CC_DEFINED
