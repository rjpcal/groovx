///////////////////////////////////////////////////////////////////////
//
// tclutil.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Thu Jul 19 20:27:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_H_DEFINED
#define TCLUTIL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

class fixed_string;

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

  SafeInterp(const SafeInterp& other) : itsInterp(other.itsInterp) {}

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

  template <class Cue>
  typename Cue::Type getGlobalVar(Cue, const char* name1,
                                  const char* name2=0) const
  {
    return Tcl::Convert<typename Cue::Type>::fromTcl
      (getObjGlobalVar(name1, name2));
  }

  // Events
  static void clearEventQueue();

  // Commands/procedures
  bool hasCommand(const char* cmd_name) const;

  fixed_string getProcBody(const char* proc_name) const;
  void createProc(const char* namesp, const char* proc_name,
                  const char* args, const char* body);

private:
  Tcl_Obj* getObjResult() const;
  Tcl_Obj* getObjGlobalVar(const char* name1, const char* name2) const;

  void handleError(const char* msg) const;

  borrowed_ptr<Tcl_Interp> const itsInterp;
};

static const char vcid_tclutil_h[] = "$Header$";
#endif // !TCLUTIL_H_DEFINED
