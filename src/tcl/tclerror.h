///////////////////////////////////////////////////////////////////////
//
// tclerror.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Jun 20 15:10:26 1999
// written: Fri Mar  3 16:37:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#ifndef STRINGFWD_H_DEFINED
#include "stringfwd.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

namespace Tcl {
  class TclError;
}

class Tcl::TclError : public ErrorWithMsg {
public:
  TclError();
  TclError(const string& msg);
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
