///////////////////////////////////////////////////////////////////////
//
// tclveccmd.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jul 12 12:14:43 2001
// written: Wed Mar 19 12:45:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMD_H_DEFINED
#define TCLVECCMD_H_DEFINED

namespace Tcl
{
  class Command;

  void useVecDispatch(Tcl::Command* cmd, unsigned int key_argn);
}

static const char vcid_tclveccmd_h[] = "$Header$";
#endif // !TCLVECCMD_H_DEFINED
