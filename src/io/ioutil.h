///////////////////////////////////////////////////////////////////////
//
// stringifycmd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:43 1999
// written: Wed Jul 18 14:17:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_H_DEFINED
#define STRINGIFYCMD_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCMD_H_DEFINED)
#include "tcl/tclcmd.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class fixed_string;

namespace Util
{
  template <class T> class Ref;
}

namespace Tcl
{
  Tcl::ObjPtr   stringify(Util::Ref<IO::IoObject> obj);
  void        destringify(Util::Ref<IO::IoObject> obj, const char* buf);

  Tcl::ObjPtr write(Util::Ref<IO::IoObject> obj);
  void         read(Util::Ref<IO::IoObject> obj, const char* buf);

  void saveASW(Util::Ref<IO::IoObject> obj, fixed_string filename);
  void loadASR(Util::Ref<IO::IoObject> obj, fixed_string filename);

} // end namespace Tcl

static const char vcid_stringifycmd_h[] = "$Header$";
#endif // !STRINGIFYCMD_H_DEFINED
