///////////////////////////////////////////////////////////////////////
//
// errorhandler.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu May 11 10:15:41 2000
// written: Thu May 11 10:17:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERRORHANDLER_H_DEFINED
#define ERRORHANDLER_H_DEFINED

class Error;
class ErrorWithMsg;

namespace Util {
  class ErrorHandler;
}

class Util::ErrorHandler {
public:
  virtual ~ErrorHandler();

  virtual void handleError(Error& err) = 0;
  virtual void handleErrorWithMsg(ErrorWithMsg& err) = 0;
};

static const char vcid_errorhandler_h[] = "$Header$";
#endif // !ERRORHANDLER_H_DEFINED
