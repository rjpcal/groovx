///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 12:55:27 1999
// written: Wed Mar  8 16:09:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tclpkg.h"

#include <tcl.h>
#include <cctype>
#include <cstring>
#include <typeinfo>
#include <vector>
#include <string>

#include "tcllink.h"
#include "tclcmd.h"
#include "tclerror.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace {

void exitHandler(ClientData clientData) {
DOTRACE("exitHandler");
  Tcl::TclPkg* pkg = static_cast<Tcl::TclPkg*>(clientData);
  delete pkg;
}

} // end unnamed namespace

struct Tcl::TclPkg::Impl {
  Impl(Tcl_Interp* interp, const char* name, const char* version);

  ~Impl();

  Tcl_Interp* itsInterp;
  vector<TclCmd *> itsCmds;
  string itsPkgName;
  string itsVersion;

  int itsInitStatus;

  vector<int*> ownedInts;
  vector<double*> ownedDoubles;
  vector<char**> ownedCstrings;
};

Tcl::TclPkg::Impl::Impl(Tcl_Interp* interp,
								const char* name, const char* version) :
  itsInterp(interp),
  itsCmds(),
  itsPkgName(name ? name : ""),
  itsVersion(version ? version : ""),
  itsInitStatus(TCL_OK)
{
DOTRACE("Tcl::TclPkg::Impl::Impl");
  if ( !itsPkgName.empty() && !itsVersion.empty() ) {

	 // First we must construct a capitalization-correct version of
	 // itsPkgName that is just how Tcl likes it: first character
	 // uppercase, all others lowercase.
	 string pkgname = itsPkgName;

	 pkgname[0] = toupper(pkgname[0]);
	 
	 for (size_t i = 1; i < pkgname.length(); ++i) {
		pkgname[i] = tolower(pkgname[i]);
	 }

	 // Fix up itsVersion to remove any extraneous char's (such as
	 // version-control info, where we'd get a string like
	 //   '$Revision 2.8 $'

	 itsVersion.erase(0, itsVersion.find_first_of("0123456789"));
	 itsVersion.erase(itsVersion.find_last_of("0123456789")+1, string::npos);

	 Tcl_PkgProvide(interp, 
						 const_cast<char *>(pkgname.c_str()),
						 const_cast<char *>(itsVersion.c_str()));
  }
}

Tcl::TclPkg::Impl::~Impl() {
DOTRACE("Tcl::TclPkg::Impl::~Impl");
  for (size_t i = 0; i < itsCmds.size(); i++) {
	 DebugEval(i); DebugEval(itsCmds.size()); DebugEvalNL((void*)itsCmds[i]);
	 DebugEvalNL(typeid(*itsCmds[i]).name());

#ifndef GCC_COMPILER
	 delete itsCmds[i];
	 itsCmds[i] = 0;
#endif
  }

  while ( !(ownedInts.empty()) ) {
	 delete ownedInts.back();
	 ownedInts.pop_back();
  }
  while ( !(ownedDoubles.empty()) ) {
	 delete ownedDoubles.back();
	 ownedDoubles.pop_back();
  }
  while ( !(ownedCstrings.empty()) ) {
	 delete [] *(ownedCstrings.back());
	 delete ownedCstrings.back();
	 ownedCstrings.pop_back();
  }
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

int Tcl::TclPkg::initStatus() {
DOTRACE("Tcl::TclPkg::initStatus");
  return itsImpl->itsInitStatus;
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
  static string name;
  name = pkgName();
  name += "::";
  name += cmd_name;
  return name.c_str();
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

void Tcl::TclPkg::linkVar(const char* varName, char*& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVar char*");
  DebugEvalNL(varName);
  if (Tcl_LinkString(itsImpl->itsInterp, varName, &var, 0) != TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVarCopy(const char* varName, int var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVarCopy int");
  DebugEvalNL(varName);
  int* copy = new int(var);
  itsImpl->ownedInts.push_back(copy);
  if (Tcl_LinkInt(itsImpl->itsInterp, varName, copy, TCL_LINK_READ_ONLY)
		!= TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVarCopy(const char* varName, double var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVarCopy double");
  double* copy = new double(var);
  itsImpl->ownedDoubles.push_back(copy);
  if (Tcl_LinkDouble(itsImpl->itsInterp, varName, copy, TCL_LINK_READ_ONLY)
		!= TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::linkVarCopy(const char* varName, const char* var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkVarCopy char*");
  DebugEvalNL(varName);
  char* copy = new char[strlen(var)+1];
  strcpy(copy, var);
  char** copyptr = new char* (copy);
  itsImpl->ownedCstrings.push_back(copyptr);
  if (Tcl_LinkString(itsImpl->itsInterp, varName, copyptr, TCL_LINK_READ_ONLY)
		!= TCL_OK)
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

void Tcl::TclPkg::linkConstVar(const char* varName, char*& var) throw (Tcl::TclError) {
DOTRACE("Tcl::TclPkg::linkConstVar char*");
  DebugEvalNL(varName);
  if (Tcl_LinkString(itsImpl->itsInterp, varName, &var, TCL_LINK_READ_ONLY)
		!= TCL_OK)
	 throw TclError();
}

void Tcl::TclPkg::addCommand(TclCmd* cmd) {
DOTRACE("Tcl::TclPkg::addCommand");
  itsImpl->itsCmds.push_back(cmd);
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
