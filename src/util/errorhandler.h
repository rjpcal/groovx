///////////////////////////////////////////////////////////////////////
//
// errorhandler.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 11 10:15:41 2000
// written: Wed Jun 26 11:23:21 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERRORHANDLER_H_DEFINED
#define ERRORHANDLER_H_DEFINED

namespace Util
{
  class Error;
  class ErrorHandler;
  class NullErrorHandler;
}

/// Abstract interface for error-handlers.
class Util::ErrorHandler
{
public:
  /// Virtual destructor.
  virtual ~ErrorHandler();

  /// Handle an error about which no information is known.
  virtual void handleUnknownError() = 0;

  /// Handle an Util::Error exception.
  virtual void handleError(Util::Error& err) = 0;

  /// Handle an error described by a string.
  virtual void handleMsg(const char* msg) = 0;
};

/// Null error handler simply ignores all errors.
class Util::NullErrorHandler : public Util::ErrorHandler
{
public:
  /// Virtual destructor.
  virtual ~NullErrorHandler();

  /// Ignore the error.
  virtual void handleUnknownError();

  /// Ignore the error.
  virtual void handleError(Util::Error& err);

  /// Ignore the error.
  virtual void handleMsg(const char* msg);
};

static const char vcid_errorhandler_h[] = "$Header$";
#endif // !ERRORHANDLER_H_DEFINED
