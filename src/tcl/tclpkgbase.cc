///////////////////////////////////////////////////////////////////////
//
// tclpkgbase.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 14 12:55:27 1999
// written: Fri Nov 22 16:16:00 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKGBASE_CC_DEFINED
#define TCLPKGBASE_CC_DEFINED

#include "tcl/tclpkgbase.h"

#include "tcl/tclcmd.h"
#include "tcl/tclcode.h"
#include "tcl/tclerror.h"

#include "util/pointers.h"
#include "util/slink_list.h"
#include "util/strings.h"

#include <tcl.h>
#include <cctype>
#include <typeinfo>
#include <string>

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  void exitHandler(ClientData clientData)
  {
    DOTRACE("Tcl::PkgBase-exitHandler");
    Tcl::PkgBase* pkg = static_cast<Tcl::PkgBase*>(clientData);
    dbgEvalNL(3, pkg->pkgName());
    delete pkg;
  }
}

///////////////////////////////////////////////////////////////////////
//
// Helper functions that provide typesafe access to Tcl_LinkVar
//
///////////////////////////////////////////////////////////////////////

namespace Tcl
{
  inline void linkInt(Tcl_Interp* interp, const char* varName,
                      int* addr, int flag)
  {
    dbgEvalNL(3, varName);
    fstring temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    if ( Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                     flag | TCL_LINK_INT) != TCL_OK )
      throw TclError("error while linking int variable");
  }

  inline void linkDouble(Tcl_Interp* interp, const char* varName,
                         double* addr, int flag)
  {
    dbgEvalNL(3, varName);
    fstring temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    if ( Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                     flag | TCL_LINK_DOUBLE) != TCL_OK )
      throw TclError("error while linking double variable");
  }

  inline void linkBoolean(Tcl_Interp* interp, const char* varName,
                          int* addr, int flag)
  {
    dbgEvalNL(3, varName);
    fstring temp = varName;
    flag &= TCL_LINK_READ_ONLY;
    if ( Tcl_LinkVar(interp, temp.data(), reinterpret_cast<char *>(addr),
                     flag | TCL_LINK_BOOLEAN) != TCL_OK )
      throw TclError("error while linking boolean variable");
  }
}

struct Tcl::PkgBase::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Tcl_Interp* interp, const char* name, const char* version);

  ~Impl();

  Tcl_Interp* interp;
  slink_list<shared_ptr<Command> > cmds;
  std::string pkgName;
  std::string version;

  int initStatus;

  slink_list<shared_ptr<int> > ownedInts;
  slink_list<shared_ptr<double> > ownedDoubles;
};

Tcl::PkgBase::Impl::Impl(Tcl_Interp* intp,
                         const char* name, const char* vers) :
  interp(intp),
  cmds(),
  pkgName(name ? name : ""),
  version(vers ? vers : ""),
  initStatus(TCL_OK),
  ownedInts(),
  ownedDoubles()
{
DOTRACE("Tcl::PkgBase::Impl::Impl");
  if ( !pkgName.empty() && !version.empty() )
  {

    // First we must construct a capitalization-correct version of
    // pkgName that is just how Tcl likes it: first character
    // uppercase, all others lowercase.
    std::string pkgname = pkgName;

    pkgname[0] = char(toupper(pkgname[0]));

    for (size_t i = 1; i < pkgname.length(); ++i)
    {
      pkgname[i] = char(tolower(pkgname[i]));
    }

    // Fix up version to remove any extraneous char's (such as
    // version-control info, where we'd get a string like
    //   '$Revision 2.8 $'

    version.erase(0, version.find_first_of("0123456789"));
    version.erase(version.find_last_of("0123456789")+1, std::string::npos);

    Tcl_PkgProvide(interp, pkgname.c_str(), version.c_str());
  }
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
  Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(this));
}

Tcl::PkgBase::~PkgBase()
{
DOTRACE("Tcl::PkgBase::~PkgBase");
  delete rep;
}

int Tcl::PkgBase::initStatus() const
{
DOTRACE("Tcl::PkgBase::initStatus");
  return rep->initStatus;
}

int Tcl::PkgBase::initStatus(PkgBase* pkg)
{
  return pkg ? pkg->initStatus() : TCL_OK;
}

int Tcl::PkgBase::combineStatus(int status1, int status2)
{
DOTRACE("Tcl::PkgBase::combineStatus");
  if (TCL_ERROR == status1 || TCL_ERROR == status2)
    return TCL_ERROR;
  else if (TCL_OK != status1)
    return status1;
  else if (TCL_OK != status2)
    return status2;

  return TCL_OK;
}

Tcl_Interp* Tcl::PkgBase::interp()
{
DOTRACE("Tcl::PkgBase::interp");
 return rep->interp;
}

namespace
{
  void exportAll(Tcl_Interp* interp, const char* from)
  {
    fstring cmd("namespace eval ", from, " { namespace export * }");

    Tcl::Code code(Tcl::toTcl(cmd), Tcl::Code::THROW_EXCEPTION);
    code.invoke(interp);
  }

  void exportInto(Tcl_Interp* interp, const char* from, const char* to)
  {
    fstring cmd("namespace eval ", to, " { namespace import ::");
    cmd.append(from, "::* }");

    Tcl::Code code(Tcl::toTcl(cmd), Tcl::Code::THROW_EXCEPTION);
    code.invoke(interp);
  }
}

void Tcl::PkgBase::namespaceAlias(const char* namesp)
{
DOTRACE("Tcl::PkgBase::namespaceAlias");

  exportAll(rep->interp, rep->pkgName.c_str());
  exportInto(rep->interp, rep->pkgName.c_str(), namesp);
}

void Tcl::PkgBase::inherit(const char* namesp)
{
DOTRACE("Tcl::PkgBase::inherit");

  exportAll(rep->interp, namesp);
  exportInto(rep->interp, namesp, rep->pkgName.c_str());
}

const char* Tcl::PkgBase::pkgName()
{
  return rep->pkgName.c_str();
}

const char* Tcl::PkgBase::version()
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
      name = pkgName();
      name += "::";
      name += cmd_name;
      return name.c_str();
    }
}

void Tcl::PkgBase::eval(const char* script)
{
DOTRACE("Tcl::PkgBase::eval");

  Tcl::Code code(Tcl::toTcl(script), Tcl::Code::THROW_EXCEPTION);
  code.invoke(rep->interp);
}

void Tcl::PkgBase::addCommand(Command* cmd)
{
DOTRACE("Tcl::PkgBase::addCommand");
  rep->cmds.push_front(shared_ptr<Command>(cmd));
}

void Tcl::PkgBase::linkVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkVar int");
  linkInt(rep->interp, varName, &var, 0);
}

void Tcl::PkgBase::linkVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkVar double");
  linkDouble(rep->interp, varName, &var, 0);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, int var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy int");
  shared_ptr<int> copy(new int(var));
  rep->ownedInts.push_front(copy);
  linkInt(rep->interp, varName, copy.get(), TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, double var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy double");
  shared_ptr<double> copy(new double(var));
  rep->ownedDoubles.push_front(copy);
  linkDouble(rep->interp, varName, copy.get(), TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkConstVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar int");
  linkInt(rep->interp, varName, &var, TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkConstVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar double");
  linkDouble(rep->interp, varName, &var, TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::setInitStatusError()
{
DOTRACE("Tcl::PkgBase::setInitStatusError");
  rep->initStatus = TCL_ERROR;
}

static const char vcid_tclpkgbase_cc[] = "$Header$";
#endif // !TCLPKGBASE_CC_DEFINED
