///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Wed Jul 18 11:27:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gx/gxnode.h"
#include "gx/gxseparator.h"

#include "tcl/tclpkg.h"

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

class GxTcl::GxNodePkg : public Tcl::Pkg {
public:
  GxNodePkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "GxNode", "$Revision$")
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

class GxTcl::GxSeparatorPkg : public Tcl::Pkg {
public:
  GxSeparatorPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "GxSeparator", "$Revision$")
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
int Gx_Init(Tcl_Interp* interp)
{
DOTRACE("Gx_Init");
  Tcl::Pkg* pkg1 = new GxTcl::GxNodePkg(interp);
  Tcl::Pkg* pkg2 = new GxTcl::GxSeparatorPkg(interp);

  return Tcl::Pkg::initStatus(pkg1, pkg2);
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
