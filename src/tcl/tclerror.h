///////////////////////////////////////////////////////////////////////
//
// tclerror.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Jun 20 15:10:26 1999
// written: Wed Sep 25 18:56:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#include "util/error.h"
#include "util/errorhandler.h"

struct Tcl_Interp;

namespace Tcl
{
  class TclError;
  class BkdErrorHandler;
}

/// An exception class for Tcl errors.
class Tcl::TclError : public Util::Error
{
public:
  TclError(const fstring& msg) : Util::Error(msg) {};

  virtual ~TclError();
};

/// An error-handler that raises Tcl background errors.
class Tcl::BkdErrorHandler : public Util::ErrorHandler
{
private:
  Tcl_Interp* itsInterp;

  void raiseBackgroundError(const char* msg);

  BkdErrorHandler(const BkdErrorHandler&);
  BkdErrorHandler& operator=(const BkdErrorHandler&);

public:
  BkdErrorHandler(Tcl_Interp* interp);

  virtual ~BkdErrorHandler();

  virtual void handleUnknownError();
  virtual void handleError(Util::Error& err);
  virtual void handleMsg(const char* msg);
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
