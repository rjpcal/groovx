///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Mon Jul 16 10:31:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "tcl/tclitempkg.h"
#include "tcl/objfunctor.h"

#include "util/objfactory.h"

#include "util/trace.h"

///////////////////////////////////////////////////////////////////////
//
// GxNodePkg
//
///////////////////////////////////////////////////////////////////////

namespace GxTcl
{
  bool contains(Ref<GxNode> item, Ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  class GxNodePkg;
}

class GxTcl::GxNodePkg : public Tcl::TclItemPkg {
public:
  GxNodePkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "GxNode", "$Revision$")
    {
      Tcl::defGenericObjCmds<GxNode>(this);

      def( &GxTcl::contains, "contains", "item_id other_id" );
    }
};

///////////////////////////////////////////////////////////////////////
//
// GxSeparatorPkg
//
///////////////////////////////////////////////////////////////////////

namespace GxTcl
{
  class GxSeparatorPkg;
}

class GxTcl::GxSeparatorPkg : public Tcl::TclItemPkg {
public:
  GxSeparatorPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "GxSeparator", "$Revision$")
    {
      Tcl::defGenericObjCmds<GxSeparator>(this);

      def( &GxSeparator::addChild, "addChild", "item_id child_item_id" );
      defGetter("numChildren", &GxSeparator::numChildren);
      defSetter("removeChildId", &GxSeparator::removeChildId);
      defSetter("removeChildUid", &GxSeparator::removeChildUid);
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
