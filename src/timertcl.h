///////////////////////////////////////////////////////////////////////
// timertcl.h
// Rob Peters
// created: Wed Feb 10 19:44:59 1999
// written: Tue Mar 16 19:24:53 1999
// $Id$
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

static const char vcid_timertcl_h[] = "$Headf: timertcl.h,v 1.3 1999/03/12 20:53:13 rjpeters Exp rjpeters $";
#endif // !TIMERTCL_H_DEFINED
