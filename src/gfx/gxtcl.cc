///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Wed Jul 11 11:02:58 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "tcl/genericobjpkg.h"

#include "util/objfactory.h"

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
    Ref<GxNode> other(getIntFromArg(2));
    GxNode* sep = getItem();
    returnVal(sep->contains(other.get()));
  }
};

class GxTcl::GxNodePkg : public Tcl::GenericObjPkg<GxNode> {
public:
  GxNodePkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<GxNode>(interp, "GxNode", "$Revision$")
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
    returnVal(child_id);
  }
};

class GxTcl::GxSeparatorPkg : public Tcl::GenericObjPkg<GxSeparator> {
public:
  GxSeparatorPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<GxSeparator>(interp, "GxSeparator", "$Revision$")
    {
      addCommand( new AddChildCmd(this, makePkgCmdName("addChild")) );
      declareCGetter("numChildren", &GxSeparator::numChildren);
      declareCSetter("removeChildId", &GxSeparator::removeChildId);
      declareCSetter("removeChildUid", &GxSeparator::removeChildUid);
      Util::ObjFactory::theOne().registerCreatorFunc(&GxSeparator::make);
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
