///////////////////////////////////////////////////////////////////////
//
// tclpkgbase.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 14 12:55:27 1999
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

#ifndef TCLPKGBASE_CC_DEFINED
#define TCLPKGBASE_CC_DEFINED

#include "tcl/tclpkgbase.h"

#include "tcl/tclcmd.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/slink_list.h"
#include "util/strings.h"

#include <tcl.h>
#include <cctype>
#include <typeinfo>
#include <string>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

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

  void exitHandler(ClientData clientData)
  {
    DOTRACE("Tcl::PkgBase-exitHandler");
    Tcl::PkgBase* pkg = static_cast<Tcl::PkgBase*>(clientData);
    dbgEvalNL(3, pkg->namespName());
    delete pkg;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Helper functions that provide typesafe access to Tcl_LinkVar
//
///////////////////////////////////////////////////////////////////////

struct Tcl::PkgBase::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Tcl_Interp* interp, const char* name, const char* version);

  ~Impl();

  Tcl::Interp interp;
  slink_list<shared_ptr<Command> > cmds;
  const std::string namespName;
  const std::string pkgName;
  const std::string version;

  int initStatus;

  slink_list<shared_ptr<int> > ownedInts;
  slink_list<shared_ptr<double> > ownedDoubles;
};

Tcl::PkgBase::Impl::Impl(Tcl_Interp* intp,
                         const char* name, const char* vers) :
  interp(intp),
  cmds(),
  namespName(name ? name : ""),
  pkgName(makeCleanPkgName(namespName)),
  version(makeCleanVersionString(vers)),
  initStatus(TCL_OK),
  ownedInts(),
  ownedDoubles()
{
DOTRACE("Tcl::PkgBase::Impl::Impl");
}

Tcl::PkgBase::Impl::~Impl()
{
DOTRACE("Tcl::PkgBase::Impl::~Impl");
}

Tcl::PkgBase::PkgBase(Tcl_Interp* interp,
                      const char* name, const char* version) :
  rep(new Impl(interp, name, version))
{
DOTRACE("Tcl::PkgBase::PkgBase");

  if ( !rep->pkgName.empty() && !rep->version.empty() )
    {
      Tcl_PkgProvideEx(interp, rep->pkgName.c_str(), rep->version.c_str(),
                       static_cast<ClientData>(this));
    }

  Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(this));
}

Tcl::PkgBase::~PkgBase()
{
DOTRACE("Tcl::PkgBase::~PkgBase");
  delete rep;
}

Tcl::PkgBase* Tcl::PkgBase::lookup(Tcl_Interp* interp, const char* name,
                                   const char* version) throw()
{
DOTRACE("Tcl::PkgBase::lookup");

  ClientData clientData = 0;

  std::string cleanName = makeCleanPkgName(name);

  const char* ver =
    Tcl_PkgRequireEx(interp, cleanName.c_str(), version, 0, &clientData);

  if (ver != 0)
    {
      Tcl::PkgBase* result = static_cast<Tcl::PkgBase*>(clientData);

      result = dynamic_cast<Tcl::PkgBase*>(result);

      return result;
    }

  return 0;
}

int Tcl::PkgBase::initStatus() const throw()
{
DOTRACE("Tcl::PkgBase::initStatus");
  if (rep->interp.getResult<const char*>()[0] != '\0')
    {
      rep->initStatus = TCL_ERROR;
    }
  return rep->initStatus;
}

Tcl::Interp& Tcl::PkgBase::interp() throw()
{
DOTRACE("Tcl::PkgBase::interp");
  return rep->interp;
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

void Tcl::PkgBase::namespaceAlias(const char* namesp) throw()
{
DOTRACE("Tcl::PkgBase::namespaceAlias");

  try
    {
      exportAll(rep->interp, rep->namespName.c_str());
      exportInto(rep->interp, rep->namespName.c_str(), namesp);
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::PkgBase::inherit", false);
    }
}

void Tcl::PkgBase::inherit(const char* namesp) throw()
{
DOTRACE("Tcl::PkgBase::inherit");

  try
    {
      exportAll(rep->interp, namesp);
      exportInto(rep->interp, namesp, rep->namespName.c_str());
    }
  catch (...)
    {
      rep->interp.handleLiveException("Tcl::PkgBase::inherit", false);
    }
}

void Tcl::PkgBase::inheritPkg(const char* name, const char* version) throw()
{
DOTRACE("Tcl::PkgBase::inheritPkg");

  Tcl::PkgBase* other = lookup(rep->interp.intp(), name, version);

  if (other != 0)
    {
      inherit(other->namespName());
    }
}

const char* Tcl::PkgBase::namespName() throw()
{
  return rep->namespName.c_str();
}

const char* Tcl::PkgBase::pkgName() throw()
{
  return rep->pkgName.c_str();
}

const char* Tcl::PkgBase::version() throw()
{
  return rep->version.c_str();
}

const char* Tcl::PkgBase::makePkgCmdName(const char* cmd_name_cstr)
{
DOTRACE("Tcl::PkgBase::makePkgCmdName");
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

void Tcl::PkgBase::eval(const char* script) throw()
{
DOTRACE("Tcl::PkgBase::eval");

  try
    {
      rep->interp.eval(script);
    }
  catch(...)
    {
      rep->interp.handleLiveException("Tcl::PkgBase::eval", false);
    }
}

void Tcl::PkgBase::addCommand(Command* cmd)
{
DOTRACE("Tcl::PkgBase::addCommand");
  rep->cmds.push_front(shared_ptr<Command>(cmd));
}

void Tcl::PkgBase::linkVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkVar int");
  rep->interp.linkInt(varName, &var, false);
}

void Tcl::PkgBase::linkVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkVar double");
  rep->interp.linkDouble(varName, &var, false);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, int var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy int");
  shared_ptr<int> copy(new int(var));
  rep->ownedInts.push_front(copy);
  rep->interp.linkInt(varName, copy.get(), true);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, double var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy double");
  shared_ptr<double> copy(new double(var));
  rep->ownedDoubles.push_front(copy);
  rep->interp.linkDouble(varName, copy.get(), true);
}

void Tcl::PkgBase::linkConstVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar int");
  rep->interp.linkInt(varName, &var, true);
}

void Tcl::PkgBase::linkConstVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar double");
  rep->interp.linkDouble(varName, &var, true);
}

void Tcl::PkgBase::setInitStatusError() throw()
{
DOTRACE("Tcl::PkgBase::setInitStatusError");
  rep->initStatus = TCL_ERROR;
}

static const char vcid_tclpkgbase_cc[] = "$Header$";
#endif // !TCLPKGBASE_CC_DEFINED
