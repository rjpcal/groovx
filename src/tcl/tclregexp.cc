///////////////////////////////////////////////////////////////////////
//
// tclregexp.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:09:12 2001
// written: Wed Aug  8 15:29:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLREGEXP_CC_DEFINED
#define TCLREGEXP_CC_DEFINED

#include "tcl/tclregexp.h"

#include "tcl/tclerror.h"

#include <tcl.h>

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace
{
  Tcl_RegExp getCheckedRegexp(Tcl::ObjPtr patternObj)
  {
    const int flags = 0;
    // OK to pass Tcl_Interp*==0
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(0, patternObj, flags);
    if (!regexp)
      {
        throw Tcl::TclError("error getting a Tcl_RegExp from '",
                            Tcl_GetString(patternObj), "'");
      }
    return regexp;
  }
}

bool Tcl::RegExp::matchesString(const char* str)
{
  static const int REGEX_ERROR        = -1;
  static const int REGEX_NO_MATCH     =  0;
  static const int REGEX_FOUND_MATCH  =  1;

  Tcl_RegExp regexp = getCheckedRegexp(itsPatternObj);

  // OK to pass Tcl_Interp*==0
  int regex_result = Tcl_RegExpExec(0, regexp, str, str);

  switch (regex_result) {
  case REGEX_ERROR:
    throw TclError("error executing regular expression");

  case REGEX_NO_MATCH:
    return false;

  case REGEX_FOUND_MATCH:
    return true;

  default: // "can't happen"
    Assert(false);
  }

  return false;
}

static const char vcid_tclregexp_cc[] = "$Header$";
#endif // !TCLREGEXP_CC_DEFINED
