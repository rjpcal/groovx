///////////////////////////////////////////////////////////////////////
//
// iotcl.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Sep 12 15:48:15 2001
// written: Wed Mar 19 12:45:46 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_H_DEFINED
#define IOTCL_H_DEFINED

namespace Tcl
{
  class Pkg;

  void defIoCommands(Pkg* pkg);
}

static const char vcid_iotcl_h[] = "$Header$";
#endif // !IOTCL_H_DEFINED
