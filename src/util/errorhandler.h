///////////////////////////////////////////////////////////////////////
//
// errorhandler.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 11 10:15:41 2000
// written: Tue Dec 10 12:40:31 2002
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
  class ThrowingErrorHandler;
}

/// Abstract interface for error-handlers.
class Util::ErrorHandler
{
  ErrorHandler(const ErrorHandler&);
  ErrorHandler& operator=(const ErrorHandler&);

protected:
  /// Default constructor.
  ErrorHandler();

  /// Virtual destructor.
  virtual ~ErrorHandler();

public:
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
protected:
  /// Default constructor.
  NullErrorHandler();

  /// Virtual destructor.
  virtual ~NullErrorHandler();

public:
  /// Singleton accessor.
  static NullErrorHandler* get();

  /// Ignore the error.
  virtual void handleUnknownError();

  /// Ignore the error.
  virtual void handleError(Util::Error& err);

  /// Ignore the error.
  virtual void handleMsg(const char* msg);
};

/// Throwing error handler raises an exception (Util::Error) for all errors.
class Util::ThrowingErrorHandler : public Util::ErrorHandler
{
protected:
  /// Default constructor.
  ThrowingErrorHandler();

  /// Virtual destructor.
  virtual ~ThrowingErrorHandler();

public:
  /// Singleton accessor.
  static ThrowingErrorHandler* get();

  /// Ignore the error.
  virtual void handleUnknownError();

  /// Ignore the error.
  virtual void handleError(Util::Error& err);

  /// Ignore the error.
  virtual void handleMsg(const char* msg);
};

static const char vcid_errorhandler_h[] = "$Header$";
#endif // !ERRORHANDLER_H_DEFINED
