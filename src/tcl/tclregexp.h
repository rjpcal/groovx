///////////////////////////////////////////////////////////////////////
//
// tclregexp.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 16 13:05:59 2001
// written: Wed Sep 25 18:57:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLREGEXP_H_DEFINED
#define TCLREGEXP_H_DEFINED

#include "tcl/tclobjptr.h"

namespace Tcl
{
  class RegExp;
}

/// Regular-expression class implemented with Tcl's regexp facilities.
class Tcl::RegExp
{
public:
  RegExp() : itsPatternObj() {}

  template <class T>
  RegExp(T val) : itsPatternObj(val) {}

  bool matchesString(const char* str);

private:
  Tcl::ObjPtr itsPatternObj;
};

static const char vcid_tclregexp_h[] = "$Header$";
#endif // !TCLREGEXP_H_DEFINED
