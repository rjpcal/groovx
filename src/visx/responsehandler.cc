///////////////////////////////////////////////////////////////////////
//
// responsehandler.cc
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Sun Jun 27 16:54:48 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_CC_DEFINED
#define RESPONSEHANDLER_CC_DEFINED

#include "responsehandler.h"

#define NO_TRACE
#include "trace.h"

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler member definitions
//
///////////////////////////////////////////////////////////////////////

ResponseHandler::ResponseHandler() :
  itsInterp(0)
{
DOTRACE("ResponseHandler::ResponseHandler");
}

ResponseHandler::~ResponseHandler() {
DOTRACE("ResponseHandler::~ResponseHandler");
}

const int ResponseHandler::INVALID_RESPONSE;

static const char vcid_responsehandler_cc[] = "$Header$";
#endif // !RESPONSEHANDLER_CC_DEFINED
