///////////////////////////////////////////////////////////////////////
//
// error.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:48 1999
// written: Tue Sep 28 18:38:42 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_CC_DEFINED
#define ERROR_CC_DEFINED

#include "error.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

Error::Error () {
DOTRACE("Error::Error ");
}

Error::~Error () {
DOTRACE("Error::~Error ");
}

ErrorWithMsg::ErrorWithMsg(const string& str) :
  itsInfo(str)
{
DOTRACE("ErrorWithMsg::ErrorWithMsg");
  DebugEvalNL(str); 
}

const string& ErrorWithMsg::msg() const {
DOTRACE("ErrorWithMsg::msg");
  return itsInfo;
}

void ErrorWithMsg::setMsg(const string& str) {
DOTRACE("ErrorWithMsg::setMsg");
  itsInfo = str;
}

static const char vcid_error_cc[] = "$Header$";
#endif // !ERROR_CC_DEFINED
