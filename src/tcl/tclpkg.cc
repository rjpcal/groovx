///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 15 12:33:54 1999
// commit: $Id$
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

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tcl/tclpkg.h"

#include "tcl/tclcmd.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclsafeinterp.h"

#include "util/fstring.h"
#include "util/sharedptr.h"

#include <tcl.h>
#include <cctype>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

#if (TCL_MAJOR_VERSION > 8) || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5)
#  define HAVE_TCL_NAMESPACE_API
#else
#  undef  HAVE_TCL_NAMESPACE_API
#endif

namespace
{
  bool VERBOSE_INIT = false;

  // Construct a capitalization-correct version of the given name that is
  // just how Tcl likes it: first character uppercase, all others
  // lowercase.
  std::string makeCleanPkgName(const std::string& name)
  {
    std::string clean = name;

    clean[0] = char(toupper(clean[0]));

    for (size_t i = 1; i < clean.length(); ++i)
      {
        clean[i] = char(tolower(clean[i]));
      }

    return clean;
  }

  std::string makeCleanVersionString(const std::string& s)
  {
    std::string::size_type dollar1 = s.find_first_of("$");
    std::string::size_type dollar2 = s.find_last_of("$");

    if (dollar1 == dollar2)
      return s;

    const std::string r = s.substr(dollar1,dollar2+1-dollar1);

    std::string::size_type n1 = r.find_first_of("0123456789");
    std::string::size_type n2 = r.find_last_of("0123456789");

    std::string result(s);

    if (n1 != std::string::npos)
      {
        const std::string n = r.substr(n1,n2+1-n1);
        result.replace(dollar1, dollar2+1-dollar1, n);
      }
    else
      {
        result.replace(dollar1, dollar2+1-dollar1, "0");
      }

    return result;
  }

  void exportAll(Tcl::Interp& interp, const char* from)
  {
  DOTRACE("exportAll");
#ifdef HAVE_TCL_NAMESPACE_API
    Tcl_Namespace* namesp =
      Tcl_FindNamespace(interp.intp(), from, 0, TCL_GLOBAL_ONLY);
    Tcl_Export(interp.intp(), namesp, "*", /*resultListFirst*/ false);
#else
    rutz::fstring cmd("namespace eval ", from, " { namespace export * }");
    interp.eval(cmd);
#endif
  }

  void exportInto(Tcl::Interp& interp,
                  const char* from, const char* to,
                  const char* pattern)
  {
  DOTRACE("exportInto");
    rutz::fstring cmd("namespace eval ", to, " { namespace import ::");
    cmd.append(from, "::", pattern, " }");

    interp.eval(cmd);
  }

  Tcl::List getCommandList(Tcl::Interp& interp, const char* namesp)
  {
    Tcl::ObjPtr saveresult = interp.getResult<Tcl::ObjPtr>();
    rutz::fstring cmd("info commands ::", namesp, "::*");
    interp.eval(cmd);
    Tcl::List cmdlist = interp.getResult<Tcl::List>();
    interp.setResult(saveresult);
    return cmdlist;
  }

  const char* getNameTail(const char* cmdname)
  {
    const char* p = cmdname;
    while (*p != 0) ++p; // skip to end of string
    while (p > cmdname)
      {
        if (*p == ':' && p > cmdname && *(p-1) == ':')
          return p-1;
        --p;
      }
    return p;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Helper functions that provide typesafe access to Tcl_LinkVar
//
///////////////////////////////////////////////////////////////////////

struct Tcl::Pkg::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Tcl_Interp* interp, const char* name, const char* version);

  ~Impl() throw();

  Tcl::Interp interp;

  const std::string namespName;
  const std::string pkgName;
  const std::string version;

  int initStatus;

  std::vector<rutz::shared_ptr<int> > ownedInts;
  std::vector<rutz::shared_ptr<double> > ownedDoubles;

  ExitCallback* exitCallback;

  static void exitHandler(ClientData clientData)
  {
    DOTRACE("Tcl::Pkg-exitHandler");
    Tcl::Pkg* pkg = static_cast<Tcl::Pkg*>(clientData);
    dbg_eval_nl(3, pkg->namespName());
    delete pkg;
  }
};

Tcl::Pkg::Impl::Impl(Tcl_Interp* intp,
                     const char* name, const char* vers) :
  interp(intp),
  namespName(name ? name : ""),
  pkgName(makeCleanPkgName(namespName)),
  version(makeCleanVersionString(vers)),
  initStatus(TCL_OK),
  ownedInts(),
  ownedDoubles(),
  exitCallback(0)
{
DOTRACE("Tcl::Pkg::Impl::Impl");
}

Tcl::Pkg::Impl::~Impl() throw()
{
DOTRACE("Tcl::Pkg::Impl::~Impl");
  if (exitCallback != 0)
    exitCallback();
}

Tcl::Pkg::Pkg(Tcl_Interp* interp,
              const char* name, const char* version) :
  rep(0)
{
DOTRACE("Tcl::Pkg::Pkg");

  rep = new Impl(interp, name, version);

  if ( !rep->pkgName.empty() && !rep->version.empty() )
    {
      Tcl_PkgProvideEx(interp, rep->pkgName.c_str(), rep->version.c_str(),
                       static_cast<ClientData>(this));
    }

  Tcl_CreateExitHandler(&Impl::exitHandler, static_cast<ClientData>(this));
}

Tcl::Pkg::~Pkg() throw()
{
DOTRACE("Tcl::Pkg::~Pkg");

  // To avoid double-deletion:
  Tcl_DeleteExitHandler(&Impl::exitHandler, static_cast<ClientData>(this));

  try
    {
#ifndef HAVE_TCL_NAMESPACE_API
      Tcl::List cmdnames = getCommandList(rep->interp,
                                          rep->namespName.c_str());

      for (unsigned int i = 0; i < cmdnames.length(); ++i)
        {
          Tcl_DeleteCommand(rep->interp.intp(),
                            cmdnames.get<const char*>(i));
        }
#else
      Tcl_Namespace* namesp =
        Tcl_FindNamespace(rep->interp.intp(), rep->namespName.c_str(),
                          0, TCL_GLOBAL_ONLY);
      if (namesp != 0)
        Tcl_DeleteNamespace(namesp);
#endif
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::~Pkg", SRC_POS);
    }

  delete rep;
}

void Tcl::Pkg::onExit(ExitCallback* callback)
{
DOTRACE("Tcl::Pkg::onExit");
  rep->exitCallback = callback;
}

int Tcl::Pkg::unloadDestroy(Tcl_Interp* interp, const char* pkgname)
{
DOTRACE("Tcl::Pkg::unloadDestroy");
  Tcl::Pkg* pkg = Tcl::Pkg::lookup(interp, pkgname);
  if (pkg != 0)
    {
      delete pkg;
      return 1; // TCL_OK
    }
  // else...
  return 0; // TCL_ERROR
}

Tcl::Pkg* Tcl::Pkg::lookup(Tcl_Interp* interp, const char* name,
                           const char* version) throw()
{
DOTRACE("Tcl::Pkg::lookup");

  ClientData clientData = 0;

  std::string cleanName = makeCleanPkgName(name);

  const char* ver =
    Tcl_PkgRequireEx(interp, cleanName.c_str(), version, 0, &clientData);

  if (ver != 0)
    {
      Tcl::Pkg* result = static_cast<Tcl::Pkg*>(clientData);

      result = dynamic_cast<Tcl::Pkg*>(result);

      return result;
    }

  return 0;
}

int Tcl::Pkg::initStatus() const throw()
{
DOTRACE("Tcl::Pkg::initStatus");
  if (rep->interp.getResult<const char*>()[0] != '\0')
    {
      rep->initStatus = TCL_ERROR;
    }
  return rep->initStatus;
}

Tcl::Interp& Tcl::Pkg::interp() throw()
{
DOTRACE("Tcl::Pkg::interp");
  return rep->interp;
}

void Tcl::Pkg::handleLiveException(const rutz::file_pos& pos) throw()
{
DOTRACE("Tcl::Pkg::handleLiveException");
  rep->interp.handleLiveException(rep->pkgName.c_str(), pos);
  this->setInitStatusError();
}

void Tcl::Pkg::namespaceAlias(const char* namesp,
                              const char* pattern) throw()
{
DOTRACE("Tcl::Pkg::namespaceAlias");

  try
    {
      exportAll(rep->interp, rep->namespName.c_str());
      exportInto(rep->interp, rep->namespName.c_str(),
                 namesp, pattern);
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::namespaceAlias",
                                      SRC_POS);
    }
}

void Tcl::Pkg::inherit(const char* namesp,
                       const char* pattern) throw()
{
DOTRACE("Tcl::Pkg::inherit");

  try
    {
      exportAll(rep->interp, namesp);
      exportInto(rep->interp, namesp,
                 rep->namespName.c_str(), pattern);
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::inherit", SRC_POS);
    }
}

void Tcl::Pkg::inheritPkg(const char* name, const char* version) throw()
{
DOTRACE("Tcl::Pkg::inheritPkg");

  Tcl::Pkg* other = lookup(rep->interp.intp(), name, version);

  if (other != 0)
    {
      inherit(other->namespName());
    }
}

const char* Tcl::Pkg::namespName() throw()
{
  return rep->namespName.c_str();
}

const char* Tcl::Pkg::pkgName() const throw()
{
  return rep->pkgName.c_str();
}

const char* Tcl::Pkg::version() const throw()
{
  return rep->version.c_str();
}

const char* Tcl::Pkg::makePkgCmdName(const char* cmd_name_cstr)
{
DOTRACE("Tcl::Pkg::makePkgCmdName");
  std::string cmd_name(cmd_name_cstr);

  // Look for a namespace qualifier "::" -- if there is already one,
  // then we assume the caller has something special in mind -- if
  // there is not one, then we do the default thing and prepend the
  // package name as a namespace qualifier.
  if (cmd_name.find("::") != std::string::npos)
    {
      return cmd_name_cstr;
    }
  else
    {
      static std::string name;
      name = namespName();
      name += "::";
      name += cmd_name;
      return name.c_str();
    }
}

void Tcl::Pkg::eval(const char* script) throw()
{
DOTRACE("Tcl::Pkg::eval");

  try
    {
      rep->interp.eval(script);
    }
  catch(...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::eval", SRC_POS);
    }
}

void Tcl::Pkg::linkVar(const char* varName, int& var)
{
DOTRACE("Tcl::Pkg::linkVar int");
  rep->interp.linkInt(varName, &var, false);
}

void Tcl::Pkg::linkVar(const char* varName, double& var)
{
DOTRACE("Tcl::Pkg::linkVar double");
  rep->interp.linkDouble(varName, &var, false);
}

void Tcl::Pkg::linkVarCopy(const char* varName, int var)
{
DOTRACE("Tcl::Pkg::linkVarCopy int");
  rutz::shared_ptr<int> copy(new int(var));
  rep->ownedInts.push_back(copy);
  rep->interp.linkInt(varName, copy.get(), true);
}

void Tcl::Pkg::linkVarCopy(const char* varName, double var)
{
DOTRACE("Tcl::Pkg::linkVarCopy double");
  rutz::shared_ptr<double> copy(new double(var));
  rep->ownedDoubles.push_back(copy);
  rep->interp.linkDouble(varName, copy.get(), true);
}

void Tcl::Pkg::linkVarConst(const char* varName, int& var)
{
DOTRACE("Tcl::Pkg::linkVarConst int");
  rep->interp.linkInt(varName, &var, true);
}

void Tcl::Pkg::linkVarConst(const char* varName, double& var)
{
DOTRACE("Tcl::Pkg::linkVarConst double");
  rep->interp.linkDouble(varName, &var, true);
}

void Tcl::Pkg::setInitStatusError() throw()
{
DOTRACE("Tcl::Pkg::setInitStatusError");
  rep->initStatus = TCL_ERROR;
}

void Tcl::Pkg::verboseInit(bool verbose) throw()
{
DOTRACE("Tcl::Pkg::verboseInit");

  VERBOSE_INIT = verbose;
}

void Tcl::Pkg::finishInit() const
{
DOTRACE("Tcl::Pkg::finishInit");

  if (VERBOSE_INIT)
    std::cerr << pkgName() << " initialized.\n";
}

const char* const Tcl::Pkg::actionUsage = "objref(s)";
const char* const Tcl::Pkg::getterUsage = "objref(s)";
const char* const Tcl::Pkg::setterUsage = "objref(s) new_value(s)";

static const char vcid_tclpkg_cc[] = "$Id$ $URL$";
#endif // !TCLPKG_CC_DEFINED
