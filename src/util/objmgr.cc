///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
// Rob Peters
// created: Fri Apr 23 01:13:16 1999
// written: Wed Jun 30 16:56:58 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "iomgr.h"

#include <iostream.h>
#include <typeinfo>
#include <string>

#if 0
#include "bitmap.h"
#include "face.h"
#include "cloneface.h"
#include "fixpt.h"
#include "position.h"
#include "jitter.h"
#include "block.h"
#include "kbdresponsehdlr.h"
#include "nullresponsehdlr.h"
#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"
#endif

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

IO* IoMgr::newIO(const char* type) {
DOTRACE("IoMgr::newIO(const char*)");

  return IoFactory::theOne().newCheckedObject(type);

#if 0
  // ObjList --> GrObj's
  if (false) {} 
  else if ("Bitmap"==string(type)) {
	 return new Bitmap();
  }
  else if ("Face"==string(type)) {
    return new Face();
  }
  else if ("CloneFace"==string(type)) {
    return new CloneFace();
  }
  else if ("FixPt"==string(type)) {
    return new FixPt();
  }
  // PosList --> Position's
  else if ("Position"==string(type)) {
	 return new Position();
  }
  else if ("Jitter"==string(type)) {
	 return new Jitter();
  }
  // BlockList --> Block's
  else if ("Block"==string(type)) {
	 return new Block();
  }
  // RhList --> ResponseHandler's
  else if ("KbdResponseHdlr"==string(type)) {
	 return new KbdResponseHdlr();
  }
  // ThList --> TimingHdlr's
  else if ("TimingHdlr"==string(type)) {
	 return new TimingHdlr();
  }
  else if ("TimingHandler"==string(type)) {
	 return new TimingHandler();
  }
  // TrialEvent's
  else if ("AbortTrialEvent"==string(type)) {
	 return new AbortTrialEvent();
  }
  else if ("DrawEvent"==string(type)) {
	 return new DrawEvent();
  }
  else if ("UndrawEvent"==string(type)) {
	 return new UndrawEvent();
  }
  else if ("EndTrialEvent"==string(type)) {
	 return new EndTrialEvent();
  }
  else if ("SwapBuffersEvent"==string(type)) {
	 return new SwapBuffersEvent();
  }
  else if ("RenderBackEvent"==string(type)) {
	 return new RenderBackEvent();
  }
  else if ("RenderFrontEvent"==string(type)) {
	 return new RenderFrontEvent();
  }
  else if ("ClearBufferEvent"==string(type)) {
	 return new ClearBufferEvent();
  }
  // Oops! IoMgr doesn't recognize the typename of whatever it's
  // trying to read.
  else {
	 throw InputError(string("IoMgr unknown typename \"") + type + "\"");
  }  
#endif
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
