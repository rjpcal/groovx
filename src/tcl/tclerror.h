///////////////////////////////////////////////////////////////////////
//
// tclerror.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Jun 20 15:10:26 1999
// written: Wed May 31 18:50:41 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

namespace Tcl {
  class TclError;
}

class Tcl::TclError : public ErrorWithMsg {
public:
  TclError();
  TclError(const char* msg);
  virtual ~TclError();
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
