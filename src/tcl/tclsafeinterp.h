///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Tue Dec 10 13:13:29 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLSAFEINTERP_H_DEFINED
#define TCLSAFEINTERP_H_DEFINED

#include "tcl/tclconvert.h"
#include "tcl/tclobjptr.h"

#include "util/pointers.h"

struct Tcl_Interp;
struct Tcl_Obj;

class fstring;

namespace Tcl
{
  class Interp;
}

namespace Util
{
  class ErrorHandler;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::Interp provides a wrapper around calls to the Tcl interpreter.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Interp
{
  Interp& operator=(const Interp&);

public:
  Interp(Tcl_Interp* interp);
  Interp(const Interp& other);
  ~Interp();

  // Interpreter
  bool hasInterp() const throw() { return itsInterp != 0; }
  Tcl_Interp* intp() const;
  bool interpDeleted() const;
  void forgetInterp();

  // Expressions
  bool evalBooleanExpr(const Tcl::ObjPtr& obj) const;

  // Evaluating code
  bool eval(const char* code, Util::ErrorHandler* handler);
  bool eval(const fstring& code, Util::ErrorHandler* handler);
  bool eval(const Tcl::ObjPtr& code, Util::ErrorHandler* handler);

  // Result
  void resetResult() const;
  void appendResult(const char* msg) const;

  template <class T>
  T getResult() const
  {
    return Tcl::toNative<T>(getObjResult());
  }

  template <class T>
  void setResult(const T& x)
  {
    setObjResult(Tcl::toTcl(x).obj());
  }

  // Variables
  void setGlobalVar(const char* var_name, const Tcl::ObjPtr& var) const;
  void unsetGlobalVar(const char* var_name) const;

  template <class T>
  T getGlobalVar(const char* name1, const char* name2=0) const
  {
    return Tcl::toNative<T>(getObjGlobalVar(name1, name2));
  }

  // Errors
  void handleLiveException(const char* where,
                           bool withBkgdError = false) throw();
  void backgroundError() throw();

  // Events
  static void clearEventQueue();

  // Commands/procedures
  bool hasCommand(const char* cmd_name) const;

  fstring getProcBody(const char* proc_name);
  void createProc(const char* namesp, const char* proc_name,
                  const char* args, const char* body);
  void deleteProc(const char* namesp, const char* proc_name);

private:
  Tcl_Obj* getObjResult() const;
  Tcl_Obj* getObjGlobalVar(const char* name1, const char* name2) const;
  void setObjResult(Tcl_Obj* obj);

  void handleError(const char* msg) const;

  borrowed_ptr<Tcl_Interp> itsInterp;
};

static const char vcid_tclsafeinterp_h[] = "$Header$";
#endif // !TCLSAFEINTERP_H_DEFINED
