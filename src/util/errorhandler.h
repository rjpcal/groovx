///////////////////////////////////////////////////////////////////////
//
// errorhandler.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu May 11 10:15:41 2000
// written: Fri Jan 18 16:06:55 2002
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

class Util::ErrorHandler
{
public:
  virtual ~ErrorHandler();

  virtual void handleUnknownError() = 0;
  virtual void handleError(Util::Error& err) = 0;
  virtual void handleMsg(const char* msg) = 0;
};

class Util::NullErrorHandler : public Util::ErrorHandler
{
public:
  virtual ~NullErrorHandler();

  virtual void handleUnknownError();
  virtual void handleError(Util::Error& err);
  virtual void handleMsg(const char* msg);
};

static const char vcid_errorhandler_h[] = "$Header$";
#endif // !ERRORHANDLER_H_DEFINED
