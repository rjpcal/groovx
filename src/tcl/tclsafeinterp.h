///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Oct 11 10:25:36 2000
// written: Wed Mar 19 17:58:05 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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

  enum ErrorStrategy
    {
      THROW_ERROR,
      IGNORE_ERROR
    };
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
  void destroy();

  // Packages
  void pkgProvide(const char* name, const char* version);

  // Expressions
  bool evalBooleanExpr(const Tcl::ObjPtr& obj) const;

  // Evaluating code
  bool eval(const char* code, ErrorStrategy strategy = THROW_ERROR);
  bool eval(const fstring& code, ErrorStrategy strategy = THROW_ERROR);
  bool eval(const Tcl::ObjPtr& code, ErrorStrategy strategy = THROW_ERROR);

  bool evalFile(const char* fname);
  void sourceRCFile();

  // Result
  void resetResult() const;
  void appendResult(const char* msg) const;
  void appendResult(const fstring& msg) const;

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

  void linkInt(const char* varName, int* addr, bool readOnly);
  void linkDouble(const char* varName, double* addr, bool readOnly);
  void linkBoolean(const char* varName, int* addr, bool readOnly);

  // Errors
  void handleLiveException(const char* where,
                           bool withBkgdError = false) throw();
  void backgroundError() throw();

  void addErrorInfo(const char* info);

  // Events
  static void clearEventQueue();

  // Commands/procedures
  bool hasCommand(const char* cmd_name) const;
  void deleteCommand(const char* cmd_name);

  fstring getProcBody(const char* proc_name);
  void createProc(const char* namesp, const char* proc_name,
                  const char* args, const char* body);
  void deleteProc(const char* namesp, const char* proc_name);

private:
  Tcl_Obj* getObjResult() const;
  Tcl_Obj* getObjGlobalVar(const char* name1, const char* name2) const;
  void setObjResult(Tcl_Obj* obj);

  Tcl_Interp* itsInterp;
};

static const char vcid_tclsafeinterp_h[] = "$Header$";
#endif // !TCLSAFEINTERP_H_DEFINED
