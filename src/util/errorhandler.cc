///////////////////////////////////////////////////////////////////////
//
// errorhandler.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 11 10:17:31 2000
// written: Wed Aug  8 12:27:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERRORHANDLER_CC_DEFINED
#define ERRORHANDLER_CC_DEFINED

#include "util/errorhandler.h"

Util::ErrorHandler::~ErrorHandler() {}

Util::NullErrorHandler::~NullErrorHandler() {}

void Util::NullErrorHandler::handleUnknownError() {}
void Util::NullErrorHandler::handleError(Util::Error&) {}
void Util::NullErrorHandler::handleMsg(const char*) {}

static const char vcid_errorhandler_cc[] = "$Header$";
#endif // !ERRORHANDLER_CC_DEFINED
