///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 14 12:55:27 1999
// written: Mon Jun 11 15:08:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tcl/tclpkg.h"

#include "tcl/tcllink.h"
#include "tcl/tclcmd.h"
#include "tcl/tclerror.h"

#include "util/lists.h"
#include "util/pointers.h"
#include "util/strings.h"

#include <tcl.h>
#include <cctype>
#include <cstring>
#include <typeinfo>
#include <string>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {

void exitHandler(ClientData clientData) {
DOTRACE("TclPkg-exitHandler");
  Tcl::TclPkg* pkg = static_cast<Tcl::TclPkg*>(clientData);
  DebugEvalNL(typeid(*pkg).name());
  delete pkg;
}

} // end unnamed namespace

struct Tcl::TclPkg::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(Tcl_Interp* interp, const char* name, const char* version);

  ~Impl();

  Tcl_Interp* itsInterp;
  slink_list<shared_ptr<TclCmd> > itsCmds;
  std::string itsPkgName;
  std::string itsVersion;

  int itsInitStatus;

  slink_list<shared_ptr<int> > ownedInts;
  slink_list<shared_ptr<double> > ownedDoubles;
};

Tcl::TclPkg::Impl::Impl(Tcl_Interp* interp,
								const char* name, const char* version) :
  itsInterp(interp),
  itsCmds(),
  itsPkgName(name ? name : ""),
  itsVersion(version ? version : ""),
  itsInitStatus(TCL_OK),
  ownedInts(),
  ownedDoubles()
{
DOTRACE("Tcl::TclPkg::Impl::Impl");
  if ( !itsPkgName.empty() && !itsVersion.empty() ) {

	 // First we must construct a capitalization-correct version of
	 // itsPkgName that is just how Tcl likes it: first character
	 // uppercase, all others lowercase.
	 std::string pkgname = itsPkgName;

	 pkgname[0] = char(toupper(pkgname[0]));

	 for (size_t i = 1; i < pkgname.length(); ++i) {
		pkgname[i] = char(tolower(pkgname[i]));
	 }

	 // Fix up itsVersion to remove any extraneous char's (such as
	 // version-control info, where we'd get a string like
	 //   '$Revision 2.8 $'

	 itsVersion.erase(0, itsVersion.find_first_of("0123456789"));
	 itsVersion.erase(itsVersion.find_last_of("0123456789")+1, std::string::npos);

	 Tcl_PkgProvide(interp, 
						 const_cast<char *>(pkgname.c_str()),
						 const_cast<char *>(itsVersion.c_str()));
  }
}

Tcl::TclPkg::Impl::~Impl() {
DOTRACE("Tcl::TclPkg::Impl::~Impl");
}

Tcl::TclPkg::TclPkg(Tcl_Interp* interp,
						  const char* name, const char* version) :
  itsImpl(new Impl(interp, name, version))
{
DOTRACE("TclPkg::TclPkg");
  Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(this));
}

Tcl::TclPkg::~TclPkg() {
DOTRACE("TclPkg::~TclPkg");
  delete itsImpl;
}

int Tcl::TclPkg::initStatus() const {
DOTRACE("Tcl::TclPkg::initStatus");
  return itsImpl->itsInitStatus;
}

int Tcl::TclPkg::combineStatus(int other_status) const {
DOTRACE("Tcl::TclPkg::initStatus");
  if (TCL_ERROR == itsImpl->itsInitStatus || TCL_ERROR == other_status)
	 return TCL_ERROR;
  else if (TCL_OK != itsImpl->itsInitStatus)
	 return itsImpl->itsInitStatus;
  else if (TCL_OK != other_status)
	 return other_status;

  return TCL_OK;
}

bool Tcl::TclPkg::initedOk() const {
DOTRACE("Tcl::TclPkg::initedOk");
  return itsImpl->itsInitStatus == TCL_OK;
}

Tcl_Interp* Tcl::TclPkg::interp() {
DOTRACE("Tcl::TclPkg::interp");
 return itsImpl->itsInterp;
}

const char* Tcl::TclPkg::pkgName() { 
  return itsImpl->itsPkgName.c_str();
}

const char* Tcl::TclPkg::version() {
  return itsImpl->itsVersion.c_str();
}

const char* Tcl::TclPkg::makePkgCmdName(const char* cmd_name) {
DOTRACE("Tcl::TclPkg::makePkgCmdName");
  static std::string name;
  name = pkgName();
  name += "::";
  name += cmd_name;
  return name.c_str();
}

void Tcl::TclPkg::eval(const char* script) {
DOTRACE("Tcl::TclPkg::eval");
  fixed_string script_copy(script); 
  Tcl_Eval(itsImpl->itsInterp, script_copy.data());
}

void Tcl::TclPkg::addCommand(TclCmd* cmd) {
DOTRACE("Tcl::TclPkg::addCommand");
  itsImpl->itsCmds.push_front(shared_ptr<TclCmd>(cmd));
}

void Tcl::TclPkg::linkVar(const char* varName, int& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVar int");
  DebugEvalNL(varName);
  if (Tcl_LinkInt(itsImpl->itsInterp, varName, &var, 0) != TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVar(const char* varName, double& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVar double");
  DebugEvalNL(varName);
  if (Tcl_LinkDouble(itsImpl->itsInterp, varName, &var, 0) != TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVarCopy(const char* varName, int var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVarCopy int");
  DebugEvalNL(varName);
  shared_ptr<int> copy(new int(var));
  itsImpl->ownedInts.push_front(copy);
  if (Tcl_LinkInt(itsImpl->itsInterp, varName,
						copy.get(), TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVarCopy(const char* varName, double var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVarCopy double");
  shared_ptr<double> copy(new double(var));
  itsImpl->ownedDoubles.push_front(copy);
  if (Tcl_LinkDouble(itsImpl->itsInterp, varName,
							copy.get(), TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkConstVar(const char* varName, int& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkConstVar int");
  DebugEvalNL(varName);
  if (Tcl_LinkInt(itsImpl->itsInterp, varName, &var, TCL_LINK_READ_ONLY)
		!= TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkConstVar(const char* varName, double& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkConstVar double");
  DebugEvalNL(varName);
  if (Tcl_LinkDouble(itsImpl->itsInterp, varName, &var, TCL_LINK_READ_ONLY)
		!= TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::setInitStatusOk() {
DOTRACE("Tcl::TclPkg::setInitStatusOk");
  itsImpl->itsInitStatus = TCL_OK;
}

void Tcl::TclPkg::setInitStatusError() {
DOTRACE("Tcl::TclPkg::setInitStatusOk");
  itsImpl->itsInitStatus = TCL_ERROR;
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
