///////////////////////////////////////////////////////////////////////
//
// tclerror.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Jun 20 15:10:13 1999
// written: Wed May 31 18:50:55 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_CC_DEFINED
#define TCLERROR_CC_DEFINED

#include "tcl/tclerror.h"

#define NO_TRACE
#include "util/trace.h"

namespace Tcl {

TclError::TclError() :
  ::ErrorWithMsg()
{
DOTRACE("TclError::TclError");
}

TclError::TclError(const char* msg) : 
  ::ErrorWithMsg(msg)
{
DOTRACE("TclError::TclError");
}

TclError::~TclError() {}

}

static const char vcid_tclerror_cc[] = "$Header$";
#endif // !TCLERROR_CC_DEFINED
