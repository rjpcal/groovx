///////////////////////////////////////////////////////////////////////
//
// tclutil.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Mon Jul 16 13:46:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_H_DEFINED
#define TCLUTIL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLERROR_H_DEFINED)
#include "tcl/tclerror.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

namespace Tcl
{
  class SafeInterp;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::SafeInterp provides a wrapper around calls to the Tcl
 * interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::SafeInterp {
public:

  SafeInterp(Tcl_Interp* interp);
  ~SafeInterp();

  bool hasInterp() const { return itsInterp != 0; }
  Tcl_Interp* intp() const { return itsInterp; }

  // Expressions
  bool evalBooleanExpr(Tcl_Obj* obj) const;

  // Interpreter
  bool interpDeleted() const;

  // Result
  void resetResult() const;
  void appendResult(const char* msg) const;

  template <class Cue>
  typename Cue::Type getResult(Cue) const
  {
	 return Tcl::Convert<typename Cue::Type>::fromTcl(getObjResult());
  }

  // Variables
  void setGlobalVar(const char* var_name, Tcl::ObjPtr var) const;
  void unsetGlobalVar(const char* var_name) const;

  // Events
  static void clearEventQueue();

private:
  Tcl_Obj* getObjResult() const;

  void handleError(const char* msg) const;

  SafeInterp(const SafeInterp&);
  SafeInterp& operator=(const SafeInterp&);

  Tcl_Interp* const itsInterp;
};

static const char vcid_tclutil_h[] = "$Header$";
#endif // !TCLUTIL_H_DEFINED
