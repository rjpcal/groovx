///////////////////////////////////////////////////////////////////////
//
// iotcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 12 15:48:15 2001
// written: Wed Sep 12 15:48:30 2001
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
