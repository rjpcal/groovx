///////////////////////////////////////////////////////////////////////
//
// tclveccmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 12 12:14:43 2001
// written: Thu Jul 12 13:02:53 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMD_H_DEFINED
#define TCLVECCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

namespace Tcl
{
  class VecCmd;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::VecCmd reimplements rawInvoke to use a specialized \c
 * Context class that treats each of the arguments as lists, and
 * provide access to slices across those lists, thus allowing
 * "vectorized" command invocations. Subclasses of \c Tcl::VecCmd
 * implement \a invoke() just as they would if inheriting from \c
 * Tcl::TclCmd.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::VecCmd : public Tcl::TclCmd {
public:
  VecCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage,
         unsigned int key_argn,
         int objc_min=0, int objc_max=-1, bool exact_objc=false);

  virtual void invoke(Context& cx) = 0;

protected:
  virtual void rawInvoke(Tcl_Interp* interp, unsigned int objc,
                         Tcl_Obj* const objv[]);

private:
  unsigned int itsKeyArgn;
};

static const char vcid_tclveccmd_h[] = "$Header$";
#endif // !TCLVECCMD_H_DEFINED
