///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 12:55:27 1999
// written: Sat Jul  3 10:32:40 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tclpkg.h"

#include <cstring>

#include "tclcmd.h"

TclPkg::TclPkg(Tcl_Interp* interp, const char* name, const char* version) :
  itsInterp(interp),
  itsCmds(),
  itsPkgName(name ? name : ""),
  itsVersion(version ? version : "")
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
}

const char* TclPkg::makePkgCmdName(const char* cmd_name) {
  static string name;
  name = pkgName() + "::" + cmd_name;
  return name.c_str();
}

void TclPkg::exitHandler(ClientData clientData) {
  TclPkg* pkg = static_cast<TclPkg*>(clientData);
  delete pkg;
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
