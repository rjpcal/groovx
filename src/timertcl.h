///////////////////////////////////////////////////////////////////////
// timertcl.h
// Rob Peters
// created: Wed Feb 10 19:44:59 1999
// written: Fri Feb 12 13:54:33 1999
///////////////////////////////////////////////////////////////////////

#ifndef TIMERTCL_H_DEFINED
#define TIMERTCL_H_DEFINED

#define NO_TRACE
#include "trace.h"

#include "debug.h"

#include <tcl.h>

namespace TimerTcl {
  int Timer_Init(Tcl_Interp *interp);
};

#endif // !TIMERTCL_H_DEFINED
