///////////////////////////////////////////////////////////////////////
//
// responsehandler.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 18 16:21:09 1999
// written: Sun Aug 26 08:53:53 2001
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

ResponseHandler::~ResponseHandler()
{
DOTRACE("ResponseHandler::~ResponseHandler");
}

static const char vcid_responsehandler_cc[] = "$Header$";
#endif // !RESPONSEHANDLER_CC_DEFINED
