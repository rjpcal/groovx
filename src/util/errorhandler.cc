///////////////////////////////////////////////////////////////////////
//
// errorhandler.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu May 11 10:17:31 2000
// written: Thu Jun  1 14:20:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERRORHANDLER_CC_DEFINED
#define ERRORHANDLER_CC_DEFINED

#include "errorhandler.h"

Util::ErrorHandler::~ErrorHandler() {}

Util::NullErrorHandler::~NullErrorHandler() {}

void Util::NullErrorHandler::handleUnknownError() {}
void Util::NullErrorHandler::handleError(Error&) {}
void Util::NullErrorHandler::handleErrorWithMsg(ErrorWithMsg&) {}
void Util::NullErrorHandler::handleMsg(const char*) {}

static const char vcid_errorhandler_cc[] = "$Header$";
#endif // !ERRORHANDLER_CC_DEFINED
