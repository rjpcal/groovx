///////////////////////////////////////////////////////////////////////
//
// tclpkgbase.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 14 12:55:27 1999
// written: Thu Nov 21 15:03:34 2002
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
    dbgEvalNL(3, typeid(*pkg).name());
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

  Tcl_Interp* itsInterp;
  slink_list<shared_ptr<Command> > itsCmds;
  std::string itsPkgName;
  std::string itsVersion;

  int itsInitStatus;

  slink_list<shared_ptr<int> > ownedInts;
  slink_list<shared_ptr<double> > ownedDoubles;
};

Tcl::PkgBase::Impl::Impl(Tcl_Interp* interp,
                         const char* name, const char* version) :
  itsInterp(interp),
  itsCmds(),
  itsPkgName(name ? name : ""),
  itsVersion(version ? version : ""),
  itsInitStatus(TCL_OK),
  ownedInts(),
  ownedDoubles()
{
DOTRACE("Tcl::PkgBase::Impl::Impl");
  if ( !itsPkgName.empty() && !itsVersion.empty() )
  {

    // First we must construct a capitalization-correct version of
    // itsPkgName that is just how Tcl likes it: first character
    // uppercase, all others lowercase.
    std::string pkgname = itsPkgName;

    pkgname[0] = char(toupper(pkgname[0]));

    for (size_t i = 1; i < pkgname.length(); ++i)
    {
      pkgname[i] = char(tolower(pkgname[i]));
    }

    // Fix up itsVersion to remove any extraneous char's (such as
    // version-control info, where we'd get a string like
    //   '$Revision 2.8 $'

    itsVersion.erase(0, itsVersion.find_first_of("0123456789"));
    itsVersion.erase(itsVersion.find_last_of("0123456789")+1, std::string::npos);

    Tcl_PkgProvide(interp, pkgname.c_str(), itsVersion.c_str());
  }
}

Tcl::PkgBase::Impl::~Impl()
{
DOTRACE("Tcl::PkgBase::Impl::~Impl");
}

Tcl::PkgBase::PkgBase(Tcl_Interp* interp,
                      const char* name, const char* version) :
  itsImpl(new Impl(interp, name, version))
{
DOTRACE("Tcl::PkgBase::PkgBase");
  Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(this));
}

Tcl::PkgBase::~PkgBase()
{
DOTRACE("Tcl::PkgBase::~PkgBase");
  delete itsImpl;
}

int Tcl::PkgBase::initStatus() const
{
DOTRACE("Tcl::PkgBase::initStatus");
  return itsImpl->itsInitStatus;
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
 return itsImpl->itsInterp;
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

  exportAll(itsImpl->itsInterp, itsImpl->itsPkgName.c_str());
  exportInto(itsImpl->itsInterp, itsImpl->itsPkgName.c_str(), namesp);
}

void Tcl::PkgBase::inherit(const char* namesp)
{
DOTRACE("Tcl::PkgBase::inherit");

  exportAll(itsImpl->itsInterp, namesp);
  exportInto(itsImpl->itsInterp, namesp, itsImpl->itsPkgName.c_str());
}

const char* Tcl::PkgBase::pkgName()
{
  return itsImpl->itsPkgName.c_str();
}

const char* Tcl::PkgBase::version()
{
  return itsImpl->itsVersion.c_str();
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
  code.invoke(itsImpl->itsInterp);
}

void Tcl::PkgBase::addCommand(Command* cmd)
{
DOTRACE("Tcl::PkgBase::addCommand");
  itsImpl->itsCmds.push_front(shared_ptr<Command>(cmd));
}

void Tcl::PkgBase::linkVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkVar int");
  linkInt(itsImpl->itsInterp, varName, &var, 0);
}

void Tcl::PkgBase::linkVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkVar double");
  linkDouble(itsImpl->itsInterp, varName, &var, 0);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, int var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy int");
  shared_ptr<int> copy(new int(var));
  itsImpl->ownedInts.push_front(copy);
  linkInt(itsImpl->itsInterp, varName, copy.get(), TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkVarCopy(const char* varName, double var)
{
DOTRACE("Tcl::PkgBase::linkVarCopy double");
  shared_ptr<double> copy(new double(var));
  itsImpl->ownedDoubles.push_front(copy);
  linkDouble(itsImpl->itsInterp, varName, copy.get(), TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkConstVar(const char* varName, int& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar int");
  linkInt(itsImpl->itsInterp, varName, &var, TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::linkConstVar(const char* varName, double& var)
{
DOTRACE("Tcl::PkgBase::linkConstVar double");
  linkDouble(itsImpl->itsInterp, varName, &var, TCL_LINK_READ_ONLY);
}

void Tcl::PkgBase::setInitStatusError()
{
DOTRACE("Tcl::PkgBase::setInitStatusError");
  itsImpl->itsInitStatus = TCL_ERROR;
}

static const char vcid_tclpkgbase_cc[] = "$Header$";
#endif // !TCLPKGBASE_CC_DEFINED
