///////////////////////////////////////////////////////////////////////
//
// tclerror.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Jun 20 15:10:26 1999
// written: Wed Mar 19 12:45:44 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLERROR_H_DEFINED
#define TCLERROR_H_DEFINED

#include "util/error.h"

namespace Tcl
{
  class TclError;
}

/// An exception class for Tcl errors.
class Tcl::TclError : public Util::Error
{
public:
  TclError(const fstring& msg) : Util::Error(msg) {};

  virtual ~TclError();
};

static const char vcid_tclerror_h[] = "$Header$";
#endif // !TCLERROR_H_DEFINED
