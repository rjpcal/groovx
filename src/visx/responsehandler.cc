///////////////////////////////////////////////////////////////////////
//
// responsehandler.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 18 16:21:09 1999
// written: Fri Jan 18 16:07:00 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_CC_DEFINED
#define RESPONSEHANDLER_CC_DEFINED

#include "visx/responsehandler.h"

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

ResponseHandler::~ResponseHandler()
{
DOTRACE("ResponseHandler::~ResponseHandler");
}

static const char vcid_responsehandler_cc[] = "$Header$";
#endif // !RESPONSEHANDLER_CC_DEFINED
