///////////////////////////////////////////////////////////////////////
//
// tcllistpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 12:49:18 1999
// written: Mon Jun 14 13:04:12 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLISTPKG_CC_DEFINED
#define TCLLISTPKG_CC_DEFINED

#include "tcllistpkg.h"

#include "tclcmd.h"
#include "stringifycmd.h"

class TclListPkgCmd : public TclCmd {
public:
  TclListPkgCmd(TclListPkg* pkg, const char* cmd_name, const char* usage,
					 int objc_min=0, int objc_max=100000, bool exact_objc=false) :
	 TclCmd(pkg->interp(), cmd_name, usage, objc_min, objc_max, exact_objc),
	 itsPkg(pkg) {}

  virtual void invoke() = 0;

protected:
  TclListPkg* pkg() { return itsPkg; }

private:
  TclListPkg* itsPkg;
};

class ListResetCmd : public TclListPkgCmd {
public:
  ListResetCmd(TclListPkg* pkg) :
	 TclListPkgCmd(pkg, pkg->makePkgCmdName("reset"), NULL, 1, 1, true) {}

  virtual void invoke() { pkg()->reset(); }
};

class ListCountCmd : public TclListPkgCmd {
public:
  ListCountCmd(TclListPkg* pkg) :
	 TclListPkgCmd(pkg, pkg->makePkgCmdName("count"), NULL, 1, 1, true) {}

  virtual void invoke() { returnInt(pkg()->count()); }
};

class ListStringifyCmd : public StringifyCmd {
public:
  ListStringifyCmd(TclListPkg* pkg) :
	 StringifyCmd(pkg->interp(), pkg->makePkgCmdName("stringify"), NULL, 1),
	 itsPkg(pkg) {}

protected:
  virtual IO& getIO() { return itsPkg->getList(); }
  virtual int getBufSize() { return itsPkg->getBufSize(); }

private:
  TclListPkg* itsPkg;
};

class ListDestringifyCmd : public DestringifyCmd {
public:
  ListDestringifyCmd(TclListPkg* pkg) :
	 DestringifyCmd(pkg->interp(), pkg->makePkgCmdName("destringify"), 
						 "string", 2),
	 itsPkg(pkg) {}

protected:
  virtual IO& getIO() { return itsPkg->getList(); }

private:
  TclListPkg* itsPkg;
};

TclListPkg::TclListPkg(Tcl_Interp* interp, 
							  const char* name, const char* version) :
  TclPkg(interp, name, version)
{
  addCommand( new ListResetCmd(this) );
  addCommand( new ListCountCmd(this) );
  addCommand( new ListStringifyCmd(this) );
  addCommand( new ListDestringifyCmd(this) );
}

static const char vcid_tcllistpkg_cc[] = "$Header$";
#endif // !TCLLISTPKG_CC_DEFINED
