///////////////////////////////////////////////////////////////////////
//
// tclsafeinterp.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Oct 11 10:25:36 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_TCL_INTERP_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_INTERP_H_UTC20050628162420_DEFINED

#include "tcl/conversions.h"
#include "tcl/obj.h"

#include "rutz/fileposition.h"
#include "rutz/sharedptr.h"

struct Tcl_Interp;
typedef struct Tcl_Obj Tcl_Obj;

namespace rutz
{
  class fstring;
}

namespace Tcl
{
  class Interp;
  class List;

  /// Different error-handling strategies for Tcl::Interp::eval().
  enum ErrorStrategy
    {
      THROW_ERROR,
      IGNORE_ERROR
    };
}


//  ########################################################
/// Tcl::Interp provides a wrapper around calls to the Tcl interpreter.
/** The advantage over using the raw Tcl C API is that certain error
    conditions are handled in a more C++-ish way, by throwing
    exceptions. */

class Tcl::Interp
{
  Interp& operator=(const Interp&);

public:
  Interp(Tcl_Interp* interp);
  Interp(const Interp& other) throw();
  ~Interp() throw();

  // Interpreter
  bool hasInterp() const throw() { return itsInterp != 0; }

  /// Get the interpreter (if valid), otherwise throw an exception.
  Tcl_Interp* intp() const;

  bool interpDeleted() const throw();
  void forgetInterp() throw();
  void destroy() throw();

  /// Wrapper around Tcl_PkgProvide().
  void pkgProvide(const char* name, const char* version);

  /// Evaluate the given expression, return its result as a bool.
  bool evalBooleanExpr(const Tcl::Obj& obj) const;

  /// Evaluates code.
  /** If strategy is THROW_ERROR, then an exception is thrown if the
      evaluation produces an error. If strategy is IGNORE_ERROR, then
      a return value of true indicates a successful evaluation, and a
      return value of false indicates an error during evaluation. */
  bool eval(const char* code, ErrorStrategy strategy = THROW_ERROR);

  /// Evaluates code.
  /** If strategy is THROW_ERROR, then an exception is thrown if the
      evaluation produces an error. If strategy is IGNORE_ERROR, then
      a return value of true indicates a successful evaluation, and a
      return value of false indicates an error during evaluation. */
  bool eval(const rutz::fstring& code, ErrorStrategy strategy = THROW_ERROR);

  /// Evaluates code.
  /** If strategy is THROW_ERROR, then an exception is thrown if the
      evaluation produces an error. If strategy is IGNORE_ERROR, then
      a return value of true indicates a successful evaluation, and a
      return value of false indicates an error during evaluation. */
  bool eval(const Tcl::Obj& code, ErrorStrategy strategy = THROW_ERROR);

  /// Evaluates code using Tcl_EvalObjv(), exploiting the fact that the object is already a list.
  /** If strategy is THROW_ERROR, then an exception is thrown if the
      evaluation produces an error. If strategy is IGNORE_ERROR, then
      a return value of true indicates a successful evaluation, and a
      return value of false indicates an error during evaluation. */
  bool evalObjv(const Tcl::List& objv, ErrorStrategy strategy = THROW_ERROR);

  /// Evaluate the tcl code in the named file.
  /** Returns true on success, or false on failure. */
  bool evalFile(const char* fname);

  void sourceRCFile();

  // Result
  void resetResult() const;
  void appendResult(const char* msg) const;
  void appendResult(const rutz::fstring& msg) const;

  template <class T>
  T getResult() const
  {
    return Tcl::fromTcl<T>(getObjResult());
  }

  template <class T>
  void setResult(const T& x)
  {
    setObjResult(Tcl::toTcl(x).obj());
  }

  // Variables
  void setGlobalVar(const char* var_name, const Tcl::Obj& var) const;
  void unsetGlobalVar(const char* var_name) const;

  template <class T>
  T getGlobalVar(const char* name1, const char* name2=0) const
  {
    return Tcl::fromTcl<T>(getObjGlobalVar(name1, name2));
  }

  void linkInt(const char* varName, int* addr, bool readOnly);
  void linkDouble(const char* varName, double* addr, bool readOnly);
  void linkBoolean(const char* varName, int* addr, bool readOnly);

  // Errors
  void handleLiveException(const char* where,
                           const rutz::file_pos& pos) throw();
  void backgroundError() throw();

  void addErrorInfo(const char* info);

  // Events
  static void clearEventQueue();

  // Commands/procedures
  bool hasCommand(const char* cmd_name) const;
  void deleteCommand(const char* cmd_name);

  rutz::fstring getProcBody(const char* proc_name);
  void createProc(const char* namesp, const char* proc_name,
                  const char* args, const char* body);
  void deleteProc(const char* namesp, const char* proc_name);

private:
  Tcl_Obj* getObjResult() const;
  Tcl_Obj* getObjGlobalVar(const char* name1, const char* name2) const;
  void setObjResult(Tcl_Obj* obj);

  Tcl_Interp* itsInterp;
};

static const char vcid_groovx_tcl_interp_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_INTERP_H_UTC20050628162420_DEFINED
