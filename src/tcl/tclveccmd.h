///////////////////////////////////////////////////////////////////////
//
// tclveccmd.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 12 12:14:43 2001
// written: Mon Jan 13 11:08:26 2003
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
