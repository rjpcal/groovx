///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Thu Aug  9 18:25:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLSAFEINTERP_H_DEFINED
#define TCLSAFEINTERP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCONVERT_H_DEFINED)
#include "tcl/tclconvert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

class fstring;

namespace Tcl
{
  class Interp;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::Interp provides a wrapper around calls to the Tcl interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Interp {
  Interp(const Interp&);
  Interp& operator=(const Interp&);

public:
  Interp(Tcl_Interp* interp);
  ~Interp();

  bool hasInterp() const throw() { return itsInterp != 0; }
  Tcl_Interp* intp() const;

  void forgetInterp();

  // Expressions
  bool evalBooleanExpr(Tcl::ObjPtr obj) const;

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

  // Errors
  void handleLiveException(const char* where,
                           bool withBkgdError = false) throw();
  void backgroundError() throw();

  // Events
  static void clearEventQueue();

  // Commands/procedures
  bool hasCommand(const char* cmd_name) const;

  fstring getProcBody(const char* proc_name) const;
  void createProc(const char* namesp, const char* proc_name,
                  const char* args, const char* body);

private:
  Tcl_Obj* getObjResult() const;
  Tcl_Obj* getObjGlobalVar(const char* name1, const char* name2) const;

  void handleError(const char* msg) const;

  borrowed_ptr<Tcl_Interp> itsInterp;
};

static const char vcid_tclsafeinterp_h[] = "$Header$";
#endif // !TCLSAFEINTERP_H_DEFINED
