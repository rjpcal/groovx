///////////////////////////////////////////////////////////////////////
//
// tclutil.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Thu May 10 12:04:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_H_DEFINED
#define TCLUTIL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLERROR_H_DEFINED)
#include "tcl/tclerror.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

namespace Tcl {
  class SafeInterface;
  class SafeInterp;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::SafeInterface includes functionality that is safe even when
 * the embedded Tcl_Interp* is null.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::SafeInterface {
private:
  SafeInterface(const SafeInterface&);
  SafeInterface& operator=(const SafeInterface&);

protected:
  Tcl_Interp* const itsInterp;

  void handleError(const char* msg) const;

public:
  SafeInterface(Tcl_Interp* interp);
  ~SafeInterface();

  bool hasInterp() const { return itsInterp != 0; }
  Tcl_Interp* intp() const { return itsInterp; }

  // Lists
  int listLength(Tcl_Obj* tcllist) const throw(TclError);

  Tcl_Obj* listElement(Tcl_Obj* tcllist, int index) const throw(TclError);

  void splitList(Tcl_Obj* tcllist, Tcl_Obj**& elements_out,
					  int& length_out) const throw(TclError);

  // Ints
  int getInt(Tcl_Obj* intObj) const throw(TclError);
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::SafeInterp extends Tcl::SafeInterface with operations that are
 * allowed only with a valid Tcl_Interp*.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::SafeInterp : public Tcl::SafeInterface {
public:

  SafeInterp(Tcl_Interp* interp);
  ~SafeInterp();

  ///////////////////////////////////////////////////////////////////////
  //
  // Tcl API wrappers
  //
  ///////////////////////////////////////////////////////////////////////

  // Expressions
  bool evalBooleanExpr(Tcl_Obj* obj) const;

  // Interpreter
  bool interpDeleted() const;

  // Result
  void appendResult(const char* msg) const;

  // Variables
  void setGlobalVar(const char* var_name, Tcl_Obj* var) const;
  void unsetGlobalVar(const char* var_name) const;
};

static const char vcid_tclutil_h[] = "$Header$";
#endif // !TCLUTIL_H_DEFINED
