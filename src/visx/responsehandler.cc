///////////////////////////////////////////////////////////////////////
//
// responsehandler.cc
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Wed Mar 15 10:17:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_CC_DEFINED
#define RESPONSEHANDLER_CC_DEFINED

#include "responsehandler.h"

#define NO_TRACE
#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler member definitions
//
///////////////////////////////////////////////////////////////////////

ResponseHandler::ResponseHandler()
{
DOTRACE("ResponseHandler::ResponseHandler");
}

ResponseHandler::~ResponseHandler() {
DOTRACE("ResponseHandler::~ResponseHandler");
}

const int ResponseHandler::INVALID_RESPONSE;

static const char vcid_responsehandler_cc[] = "$Header$";
#endif // !RESPONSEHANDLER_CC_DEFINED
