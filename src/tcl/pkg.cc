///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun 15 12:33:54 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_PKG_CC_UTC20050628162420_DEFINED
#define GROOVX_TCL_PKG_CC_UTC20050628162420_DEFINED

#include "tcl/pkg.h"

#include "tcl/command.h"
#include "tcl/list.h"
#include "tcl/interp.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sharedptr.h"

#include <tcl.h>
#include <tclInt.h> // for Tcl_FindNamespace() etc.
#include <cctype>
#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

#if (TCL_MAJOR_VERSION > 8) || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 5)
#  define HAVE_TCL_NAMESPACE_API
#else
#  undef  HAVE_TCL_NAMESPACE_API
#endif

namespace
{
  bool VERBOSE_INIT = false;

  int INIT_DEPTH = 0;

  // Construct a capitalization-correct version of the given name that
  // is just how Tcl likes it: first character uppercase, all others
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

  void exportInto(Tcl::Interp& interp,
                  const char* from, const char* to,
                  const char* pattern)
  {
  GVX_TRACE("exportInto");
    rutz::fstring cmd("namespace eval ", to, " { namespace import ::");
    cmd.append(from, "::", pattern, " }");

    interp.eval(cmd);
  }

  Tcl::List getCommandList(Tcl::Interp& interp, const char* namesp)
  {
    Tcl::Obj saveresult = interp.getResult<Tcl::Obj>();
    rutz::fstring cmd("info commands ::", namesp, "::*");
    interp.eval(cmd);
    Tcl::List cmdlist = interp.getResult<Tcl::List>();
    interp.setResult(saveresult);
    return cmdlist;
  }

  const char* getNameTail(const char* name)
  {
    const char* p = name;
    while (*p != '\0') ++p; // skip to end of string
    while (--p > name) {
      if ((*p == ':') && (*(p-1) == ':')) {
        ++p;
        break;
      }
    }
    GVX_ASSERT(p >= name);
    return p;
  }
}

const int Tcl::Pkg::STATUS_OK = TCL_OK;
const int Tcl::Pkg::STATUS_ERR = TCL_ERROR;

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

  Tcl_Namespace* tclNamespace() const
  {
  GVX_TRACE("Tcl::Pkg::Impl::tclNamespace");

    Tcl_Namespace* namesp =
      Tcl_FindNamespace(this->interp.intp(), this->namespName.c_str(),
                        0 /* namespaceContextPtr*/, TCL_GLOBAL_ONLY);

    if (namesp == 0)
      {
        namesp = Tcl_CreateNamespace(this->interp.intp(),
                                     this->namespName.c_str(),
                                     0 /*clientData*/,
                                     0 /*deleteProc*/);
      }

    GVX_ASSERT(namesp != 0);

    return namesp;
  }

  static void exitHandler(ClientData clientData)
  {
    GVX_TRACE("Tcl::Pkg-exitHandler");
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
GVX_TRACE("Tcl::Pkg::Impl::Impl");
}

Tcl::Pkg::Impl::~Impl() throw()
{
GVX_TRACE("Tcl::Pkg::Impl::~Impl");
  if (exitCallback != 0)
    exitCallback();
}

Tcl::Pkg::Pkg(Tcl_Interp* interp,
              const char* name, const char* version) :
  rep(0)
{
GVX_TRACE("Tcl::Pkg::Pkg");

  rep = new Impl(interp, name, version);

  ++INIT_DEPTH;
}

Tcl::Pkg::~Pkg() throw()
{
GVX_TRACE("Tcl::Pkg::~Pkg");

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
GVX_TRACE("Tcl::Pkg::onExit");
  rep->exitCallback = callback;
}

int Tcl::Pkg::unloadDestroy(Tcl_Interp* intp, const char* pkgname)
{
GVX_TRACE("Tcl::Pkg::unloadDestroy");
  Tcl::Interp interp(intp);
  Tcl::Pkg* pkg = Tcl::Pkg::lookup(interp, pkgname);
  if (pkg != 0)
    {
      delete pkg;
      return 1; // TCL_OK
    }
  // else...
  return 0; // TCL_ERROR
}

Tcl::Pkg* Tcl::Pkg::lookup(Tcl::Interp& interp, const char* name,
                           const char* version) throw()
{
GVX_TRACE("Tcl::Pkg::lookup");

  ClientData clientData = 0;

  const std::string cleanName = makeCleanPkgName(name);

  Tcl::Obj saveresult = interp.getResult<Tcl::Obj>();

  const char* ver =
    Tcl_PkgRequireEx(interp.intp(), cleanName.c_str(),
                     version, 0, &clientData);

  interp.setResult(saveresult);

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
GVX_TRACE("Tcl::Pkg::initStatus");
  if (rep->interp.getResult<const char*>()[0] != '\0')
    {
      rep->initStatus = TCL_ERROR;
    }
  return rep->initStatus;
}

Tcl::Interp& Tcl::Pkg::interp() throw()
{
GVX_TRACE("Tcl::Pkg::interp");
  return rep->interp;
}

void Tcl::Pkg::handleLiveException(const rutz::file_pos& pos) throw()
{
GVX_TRACE("Tcl::Pkg::handleLiveException");
  rep->interp.handleLiveException(rep->pkgName.c_str(), pos);
  this->setInitStatusError();
}

void Tcl::Pkg::namespaceAlias(const char* namesp, const char* pattern)
{
GVX_TRACE("Tcl::Pkg::namespaceAlias");

  exportInto(rep->interp, rep->namespName.c_str(), namesp, pattern);
}

void Tcl::Pkg::inherit(const char* namesp, const char* pattern)
{
GVX_TRACE("Tcl::Pkg::inherit");

  // (1) export commands from 'namesp' into this Tcl::Pkg's namespace
  exportInto(rep->interp, namesp, rep->namespName.c_str(), pattern);

  // (2) get the export patterns from 'namesp' and include those as
  // export patterns for this Tcl::Pkg's namespace
  Tcl_Interp* const interp = rep->interp.intp();

  Tcl_Namespace* const othernsptr =
    Tcl_FindNamespace(interp, namesp, 0, TCL_GLOBAL_ONLY);

  if (othernsptr == 0)
    throw rutz::error(rutz::fstring("no Tcl namespace '", namesp, "'"),
                      SRC_POS);

  Tcl::Obj obj;
  Tcl_AppendExportList(interp, othernsptr, obj.obj());

  Tcl_Namespace* const thisnsptr = rep->tclNamespace();

  Tcl::List exportlist(obj);
  for (unsigned int i = 0; i < exportlist.size(); ++i)
    {
      Tcl_Export(interp, thisnsptr,
                 exportlist.get<const char*>(i),
                 /*resetExportListFirst*/ false);
    }
}

void Tcl::Pkg::inheritPkg(const char* name, const char* version)
{
GVX_TRACE("Tcl::Pkg::inheritPkg");

  Tcl::Pkg* other = lookup(rep->interp, name, version);

  if (other == 0)
    throw rutz::error(rutz::fstring("no Tcl::Pkg named '", name, "'"),
                      SRC_POS);

  inherit(other->namespName());
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

const char* Tcl::Pkg::makePkgCmdName(const char* cmd_name_cstr,
                                     int flags)
{
GVX_TRACE("Tcl::Pkg::makePkgCmdName");
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
      if (!(flags & NO_EXPORT))
        {
          Tcl_Namespace* const namesp = rep->tclNamespace();

          Tcl_Export(rep->interp.intp(), namesp, cmd_name_cstr,
                     /*resetExportListFirst*/ false);
        }

      static std::string name;
      name = namespName();
      name += "::";
      name += cmd_name;
      return name.c_str();
    }
}

void Tcl::Pkg::eval(const char* script)
{
GVX_TRACE("Tcl::Pkg::eval");
  rep->interp.eval(script);
}

void Tcl::Pkg::linkVar(const char* varName, int& var)
{
GVX_TRACE("Tcl::Pkg::linkVar int");
  rep->interp.linkInt(varName, &var, false);
}

void Tcl::Pkg::linkVar(const char* varName, double& var)
{
GVX_TRACE("Tcl::Pkg::linkVar double");
  rep->interp.linkDouble(varName, &var, false);
}

void Tcl::Pkg::linkVarCopy(const char* varName, int var)
{
GVX_TRACE("Tcl::Pkg::linkVarCopy int");
  rutz::shared_ptr<int> copy(new int(var));
  rep->ownedInts.push_back(copy);
  rep->interp.linkInt(varName, copy.get(), true);
}

void Tcl::Pkg::linkVarCopy(const char* varName, double var)
{
GVX_TRACE("Tcl::Pkg::linkVarCopy double");
  rutz::shared_ptr<double> copy(new double(var));
  rep->ownedDoubles.push_back(copy);
  rep->interp.linkDouble(varName, copy.get(), true);
}

void Tcl::Pkg::linkVarConst(const char* varName, int& var)
{
GVX_TRACE("Tcl::Pkg::linkVarConst int");
  rep->interp.linkInt(varName, &var, true);
}

void Tcl::Pkg::linkVarConst(const char* varName, double& var)
{
GVX_TRACE("Tcl::Pkg::linkVarConst double");
  rep->interp.linkDouble(varName, &var, true);
}

void Tcl::Pkg::setInitStatusError() throw()
{
GVX_TRACE("Tcl::Pkg::setInitStatusError");
  rep->initStatus = TCL_ERROR;
}

void Tcl::Pkg::verboseInit(bool verbose) throw()
{
GVX_TRACE("Tcl::Pkg::verboseInit");

  VERBOSE_INIT = verbose;
}

int Tcl::Pkg::finishInit() throw()
{
GVX_TRACE("Tcl::Pkg::finishInit");

  --INIT_DEPTH;

  if (rep->initStatus == TCL_OK)
    {
      if (VERBOSE_INIT)
        {
          for (int i = 0; i < INIT_DEPTH; ++i)
            std::cerr << "    ";
          std::cerr << pkgName() << " initialized.\n";
        }

      if ( !rep->pkgName.empty() && !rep->version.empty() )
        {
          Tcl_PkgProvideEx(rep->interp.intp(),
                           rep->pkgName.c_str(), rep->version.c_str(),
                           static_cast<ClientData>(this));
        }

      Tcl_CreateExitHandler(&Impl::exitHandler,
                            static_cast<ClientData>(this));

      return rep->initStatus;
    }

  // else (rep->initStatus != TCL_OK)

  delete this;
  return TCL_ERROR;
}

const char* const Tcl::Pkg::actionUsage = "objref(s)";
const char* const Tcl::Pkg::getterUsage = "objref(s)";
const char* const Tcl::Pkg::setterUsage = "objref(s) new_value(s)";

static const char vcid_groovx_tcl_pkg_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_PKG_CC_UTC20050628162420_DEFINED
