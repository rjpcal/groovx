///////////////////////////////////////////////////////////////////////
//
// tclerror.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Jun 20 15:10:26 1999
// written: Fri Nov 10 17:03:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERRORHANDLER_H_DEFINED)
#include "util/errorhandler.h"
#endif

struct Tcl_Interp;

namespace Tcl {
  class TclError;
  class BkdErrorHandler;
}

class Tcl::TclError : public ErrorWithMsg {
public:
  TclError();
  TclError(const char* msg);
  virtual ~TclError();
};

class Tcl::BkdErrorHandler : public Util::ErrorHandler {
private:
  Tcl_Interp* itsInterp;

  void raiseBackgroundError(const char* msg);

  BkdErrorHandler(const BkdErrorHandler&);
  BkdErrorHandler& operator=(const BkdErrorHandler&);

public:
  BkdErrorHandler(Tcl_Interp* interp);

  virtual ~BkdErrorHandler();

  virtual void handleUnknownError();
  virtual void handleError(Error& err);
  virtual void handleErrorWithMsg(ErrorWithMsg& err);
  virtual void handleMsg(const char* msg);
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
