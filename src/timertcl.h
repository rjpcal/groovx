///////////////////////////////////////////////////////////////////////
// timertcl.h
// Rob Peters
// created: Wed Feb 10 19:44:59 1999
// written: Fri Mar 12 12:53:11 1999
static const char vcid_timertcl_h[] = "$Id$";
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
