///////////////////////////////////////////////////////////////////////
//
// tclerror.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sun Jun 20 15:10:26 1999
// written: Tue Dec  7 18:11:22 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#include "error.h"

namespace Tcl {

class TclError : public ::ErrorWithMsg {
public:
  TclError(const string& msg="");
};

}

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
