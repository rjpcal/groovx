///////////////////////////////////////////////////////////////////////
//
// convert.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 08:57:31 2001
// written: Wed Jul 11 09:23:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CONVERT_H_DEFINED
#define CONVERT_H_DEFINED

struct Tcl_Obj;

namespace Tcl
{

  template <class T>
  T fromTcl(Tcl_Obj* obj);

  template <class T>
  Tcl_Obj* toTcl(T val);

}

static const char vcid_convert_h[] = "$Header$";
#endif // !CONVERT_H_DEFINED
