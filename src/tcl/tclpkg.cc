///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 15 12:33:54 1999
// commit: $Id$
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

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tcl/tclpkg.h"

#include "tcl/tclcmd.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/slink_list.h"
#include "util/strings.h"

#include <tcl.h>
#include <cctype>
#include <iostream>
#include <typeinfo>
#include <string>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
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

  std::string makeCleanVersionString(const char* ver)
  {
    std::string result = ver ? ver : "";

    // Fix up version to remove any extraneous char's (such as
    // version-control info, where we'd get a string like
    //   '$Revision 2.8 $'

    result.erase(0, result.find_first_of("0123456789"));
    result.erase(result.find_last_of("0123456789")+1, std::string::npos);

    return result;
  }

  bool VERBOSE_INIT = false;
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

  slink_list<shared_ptr<int> > ownedInts;
  slink_list<shared_ptr<double> > ownedDoubles;

  ExitCallback* exitCallback;

  static void exitHandler(ClientData clientData)
  {
    DOTRACE("Tcl::Pkg-exitHandler");
    Tcl::Pkg* pkg = static_cast<Tcl::Pkg*>(clientData);
    dbgEvalNL(3, pkg->namespName());
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
  rep(new Impl(interp, name, version))
{
DOTRACE("Tcl::Pkg::Pkg");

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
  delete rep;
}

void Tcl::Pkg::onExit(ExitCallback* callback)
{
DOTRACE("Tcl::Pkg::onExit");
  rep->exitCallback = callback;
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

void Tcl::Pkg::handleLiveException() throw()
{
DOTRACE("Tcl::Pkg::handleLiveException");
  rep->interp.handleLiveException("");
}

namespace
{
  void exportAll(Tcl::Interp& interp, const char* from)
  {
  DOTRACE("exportAll");
    fstring cmd("namespace eval ", from, " { namespace export * }");

    interp.eval(cmd);
  }

  void exportInto(Tcl::Interp& interp, const char* from, const char* to)
  {
  DOTRACE("exportInto");
    fstring cmd("namespace eval ", to, " { namespace import ::");
    cmd.append(from, "::* }");

    interp.eval(cmd);
  }
}

void Tcl::Pkg::namespaceAlias(const char* namesp) throw()
{
DOTRACE("Tcl::Pkg::namespaceAlias");

  try
    {
      exportAll(rep->interp, rep->namespName.c_str());
      exportInto(rep->interp, rep->namespName.c_str(), namesp);
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::namespaceAlias", false);
    }
}

void Tcl::Pkg::inherit(const char* namesp) throw()
{
DOTRACE("Tcl::Pkg::inherit");

  try
    {
      exportAll(rep->interp, namesp);
      exportInto(rep->interp, namesp, rep->namespName.c_str());
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::Pkg::inherit", false);
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
      rep->interp.handleLiveException("Tcl::Pkg::eval", false);
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
  shared_ptr<int> copy(new int(var));
  rep->ownedInts.push_front(copy);
  rep->interp.linkInt(varName, copy.get(), true);
}

void Tcl::Pkg::linkVarCopy(const char* varName, double var)
{
DOTRACE("Tcl::Pkg::linkVarCopy double");
  shared_ptr<double> copy(new double(var));
  rep->ownedDoubles.push_front(copy);
  rep->interp.linkDouble(varName, copy.get(), true);
}

void Tcl::Pkg::linkConstVar(const char* varName, int& var)
{
DOTRACE("Tcl::Pkg::linkConstVar int");
  rep->interp.linkInt(varName, &var, true);
}

void Tcl::Pkg::linkConstVar(const char* varName, double& var)
{
DOTRACE("Tcl::Pkg::linkConstVar double");
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

const char* Tcl::Pkg::actionUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "objref(s)";
}

const char* Tcl::Pkg::getterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "objref(s)";
}

const char* Tcl::Pkg::setterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "objref(s) new_value(s)";
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
