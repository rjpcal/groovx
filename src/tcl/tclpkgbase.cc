///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 12:55:27 1999
// written: Tue Jul 13 18:50:57 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tclpkg.h"

#include <cstring>

#include "tcllink.h"
#include "tclcmd.h"
#include "tclerror.h"

struct TclPkg::PrivateRep {
  vector<int*> ownedInts;
  vector<double*> ownedDoubles;
  vector<char**> ownedCstrings;

  ~PrivateRep() {
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
};

TclPkg::TclPkg(Tcl_Interp* interp, const char* name, const char* version) :
  itsInterp(interp),
  itsCmds(),
  itsPkgName(name ? name : ""),
  itsVersion(version ? version : ""),
  itsPrivateRep(new PrivateRep)
{
  if ( !itsPkgName.empty() && !itsVersion.empty() ) {

	 // First we must construct a capitalization-correct version of
	 // itsPkgName that is just how Tcl likes it: first character
	 // uppercase, all others lowercase.
	 string pkgname = itsPkgName;

	 pkgname[0] = toupper(pkgname[0]);
	 
	 for (int i = 1; i < pkgname.length(); ++i) {
		pkgname[i] = tolower(pkgname[i]);
	 }

	 Tcl_PkgProvide(interp, 
						 const_cast<char *>(pkgname.c_str()),
						 const_cast<char *>(itsVersion.c_str()));
  }

  Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(this));
}

TclPkg::~TclPkg() {
  for (int i = 0; i < itsCmds.size(); i++) {
	 delete itsCmds[i];
	 itsCmds[i] = 0;
  }
  delete itsPrivateRep;
}

const char* TclPkg::makePkgCmdName(const char* cmd_name) {
  static string name;
  name = pkgName() + "::" + cmd_name;
  return name.c_str();
}

void TclPkg::linkVar(const char* varName, int& var) throw (TclError) {
  if (Tcl_LinkInt(itsInterp, varName, &var, 0) != TCL_OK) throw TclError();
}

void TclPkg::linkVar(const char* varName, double& var) throw (TclError) {
  if (Tcl_LinkDouble(itsInterp, varName, &var, 0) != TCL_OK) throw TclError();
}

void TclPkg::linkVar(const char* varName, char*& var) throw (TclError) {
  if (Tcl_LinkString(itsInterp, varName, &var, 0) != TCL_OK) throw TclError();
}

void TclPkg::linkVarCopy(const char* varName, int var) throw (TclError) {
  int* copy = new int(var);
  itsPrivateRep->ownedInts.push_back(copy);
  if (Tcl_LinkInt(itsInterp, varName, copy, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::linkVarCopy(const char* varName, double var) throw (TclError) {
  double* copy = new double(var);
  itsPrivateRep->ownedDoubles.push_back(copy);
  if (Tcl_LinkDouble(itsInterp, varName, copy, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::linkVarCopy(const char* varName, const char* var) throw (TclError) {
  char* copy = new char[strlen(var)+1];
  strcpy(copy, var);
  char** copyptr = new char* (copy);
  itsPrivateRep->ownedCstrings.push_back(copyptr);
  if (Tcl_LinkString(itsInterp, varName, copyptr, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::linkConstVar(const char* varName, int& var) throw (TclError) {
  if (Tcl_LinkInt(itsInterp, varName, &var, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::linkConstVar(const char* varName, double& var) throw (TclError) {
  if (Tcl_LinkDouble(itsInterp, varName, &var, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::linkConstVar(const char* varName, char*& var) throw (TclError) {
  if (Tcl_LinkString(itsInterp, varName, &var, TCL_LINK_READ_ONLY) != TCL_OK)
	 throw TclError();
}

void TclPkg::exitHandler(ClientData clientData) {
  TclPkg* pkg = static_cast<TclPkg*>(clientData);
  delete pkg;
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
