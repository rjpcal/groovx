///////////////////////////////////////////////////////////////////////
//
// ptrhandle.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Oct 22 15:57:20 2000
// written: Sun Oct 22 15:57:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRHANDLE_CC_DEFINED
#define PTRHANDLE_CC_DEFINED

#include "util/ptrhandle.h"

#include "util/error.h"

#define NO_TRACE
#include "util/trace.h"

void PtrHandleUtil::throwErrorWithMsg(const char* msg) {
DOTRACE("PtrHandleUtil::throwErrorWithMsg");
  throw ErrorWithMsg(msg);
}

static const char vcid_ptrhandle_cc[] = "$Header$";
#endif // !PTRHANDLE_CC_DEFINED
