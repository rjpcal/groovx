///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Mon Dec 11 14:29:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "io/iofactory.h"

#include "tcl/ioitempkg.h"

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// GxNodePkg
//
///////////////////////////////////////////////////////////////////////

namespace GxTcl {
  class ContainsCmd;
  class GxNodePkg;
}

class GxTcl::ContainsCmd : public Tcl::TclItemCmd<GxNode> {
public:
  ContainsCmd(Tcl::CTclItemPkg<GxNode>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<GxNode>(pkg, cmd_name, "item_id other_id", 3, 3) {}

protected:
  virtual void invoke() {
	 IdItem<GxNode> other(getIntFromArg(2));
	 GxNode* sep = getItem();
	 returnBool(sep->contains(other.get()));
  }
};

class GxTcl::GxNodePkg : public Tcl::IoItemPkg<GxNode> {
public:
  GxNodePkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<GxNode>(interp, "GxNode", "$Revision$")
	 {
		addCommand( new ContainsCmd(this, makePkgCmdName("contains")) );
	 }
};

///////////////////////////////////////////////////////////////////////
//
// GxSeparatorPkg
//
///////////////////////////////////////////////////////////////////////

namespace GxTcl {
  class AddChildCmd;
  class GxSeparatorPkg;
}

class GxTcl::AddChildCmd : public Tcl::TclItemCmd<GxSeparator> {
public:
  AddChildCmd(Tcl::CTclItemPkg<GxSeparator>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<GxSeparator>(pkg, cmd_name,
											"item_id child_item_id", 3, 3) {}

protected:
  virtual void invoke() {
	 int child = getIntFromArg(2);
	 GxSeparator* sep = getItem();
	 int child_id = sep->addChild(child);
	 returnInt(child_id);
  }
};

class GxTcl::GxSeparatorPkg : public Tcl::IoItemPkg<GxSeparator> {
public:
  GxSeparatorPkg(Tcl_Interp* interp) :
	 Tcl::IoItemPkg<GxSeparator>(interp, "GxSeparator", "$Revision$")
	 {
		addCommand( new AddChildCmd(this, makePkgCmdName("addChild")) );
		declareCGetter("numChildren", &GxSeparator::numChildren);
		declareCSetter("removeChildId", &GxSeparator::removeChildId);
		declareCSetter("removeChildUid", &GxSeparator::removeChildUid);
		IO::IoFactory::theOne().registerCreatorFunc(&GxSeparator::make);
	 }
};

extern "C"
int Gx_Init(Tcl_Interp* interp) {
DOTRACE("Gx_Init");
  Tcl::TclPkg* pkg1 = new GxTcl::GxNodePkg(interp);
  Tcl::TclPkg* pkg2 = new GxTcl::GxSeparatorPkg(interp);

  return pkg1->combineStatus(pkg2->initStatus());
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
