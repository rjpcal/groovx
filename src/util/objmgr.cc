///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
// Rob Peters
// created: Fri Apr 23 01:13:16 1999
// written: Fri Jun 11 11:32:06 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "iomgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#include "face.h"
#include "cloneface.h"
#include "fixpt.h"
#include "position.h"
#include "jitter.h"
#include "expt.h"
#include "responsehandler.h"
#include "timinghandler.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

IO* IoMgr::newIO(const char* type) {
DOTRACE("IoMgr::newIO(const char*)");
  // GrObj's
  if ("Face"==string(type)) {
    return new Face();
  }
  else if ("CloneFace"==string(type)) {
    return new CloneFace();
  }
  else if ("FixPt"==string(type)) {
    return new FixPt();
  }
  // Position's
  else if ("Position"==string(type)) {
	 return new Position();
  }
  else if ("Jitter"==string(type)) {
	 return new Jitter();
  }
  // Others
  else if ("Expt"==string(type)) {
	 return new Expt();
  }
  else if ("ResponseHandler"==string(type)) {
	 return new ResponseHandler();
  }
  else if ("TimingHandler"==string(type)) {
	 return new TimingHandler();
  }
  // Oops! IoMgr doesn't recognize the typename of whatever it's
  // trying to read.
  else {
	 throw InputError(string("IoMgr unknown typename \"") + type + "\"");
  }  
}

IO* IoMgr::newIO(istream& is, IO::IOFlag flag) {
DOTRACE("IoMgr::newIo(istream&, IO::IOFlag)");
  string type;
  is >> type;
  // We must turn off the TYPENAME flag since the typename has already
  // been read from the input stream by the present function.
  return newIO(type.c_str(), is, flag & ~IO::TYPENAME);
}

IO* IoMgr::newIO(const char* type, istream& is, IO::IOFlag flag) {
DOTRACE("IoMgr::newIo(const char*, istream&, IO::IOFlag)");
  IO* io = newIO(type);
  io->deserialize(is, flag);
  return io;
}

static const char vcid_iomgr_cc[] = "$Header$";
#endif // !IOMGR_CC_DEFINED
