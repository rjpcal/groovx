///////////////////////////////////////////////////////////////////////
//
// errorhandler.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 11 10:17:31 2000
// written: Tue Dec 10 12:28:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERRORHANDLER_CC_DEFINED
#define ERRORHANDLER_CC_DEFINED

#include "util/errorhandler.h"

#include "util/error.h"

Util::ErrorHandler::ErrorHandler() {}
Util::ErrorHandler::~ErrorHandler() {}

Util::NullErrorHandler::NullErrorHandler() {}
Util::NullErrorHandler::~NullErrorHandler() {}

Util::NullErrorHandler* Util::NullErrorHandler::get()
{
  static Util::NullErrorHandler* p = 0;
  if (p == 0)
    p = new Util::NullErrorHandler;
  return p;
}

void Util::NullErrorHandler::handleUnknownError() {}
void Util::NullErrorHandler::handleError(Util::Error&) {}
void Util::NullErrorHandler::handleMsg(const char*) {}

Util::ThrowingErrorHandler::ThrowingErrorHandler() {}
Util::ThrowingErrorHandler::~ThrowingErrorHandler() {}

Util::ThrowingErrorHandler* Util::ThrowingErrorHandler::get()
{
  static Util::ThrowingErrorHandler* p = 0;
  if (p == 0)
    p = new Util::ThrowingErrorHandler;
  return p;
}

void Util::ThrowingErrorHandler::handleUnknownError()
{ throw Util::Error("an unknown error occured"); }
void Util::ThrowingErrorHandler::handleError(Util::Error& err)
{ throw err; }
void Util::ThrowingErrorHandler::handleMsg(const char* msg)
{ throw Util::Error(msg); }

static const char vcid_errorhandler_cc[] = "$Header$";
#endif // !ERRORHANDLER_CC_DEFINED
