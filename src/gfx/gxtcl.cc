///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Wed Aug 22 18:01:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "itertcl.h"

#include "gfx/gxcolor.h"
#include "gfx/gxnode.h"
#include "gfx/gxseparator.h"

#include "tcl/fieldpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace GxTcl
{
  bool contains(Ref<GxNode> item, Ref<GxNode> other)
  {
    return item->contains(other.get());
  }
}

extern "C"
int Gx_Init(Tcl_Interp* interp)
{
DOTRACE("Gx_Init");

  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "GxNode", "$Revision$");
  Tcl::defGenericObjCmds<GxNode>(pkg1);

  pkg1->def( "contains", "item_id other_id", &GxTcl::contains );
  pkg1->defVec("deepChildren", "item_id(s)", &GxNode::deepChildren);


  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "GxSeparator", "$Revision$");
  Tcl::defGenericObjCmds<GxSeparator>(pkg2);

  pkg2->def( "addChild", "item_id child_item_id", &GxSeparator::addChild );
  pkg2->defGetter("children", &GxSeparator::children);
  pkg2->defGetter("numChildren", &GxSeparator::numChildren);
  pkg2->defSetter("removeChildAt", &GxSeparator::removeChildAt);
  pkg2->defSetter("removeChild", &GxSeparator::removeChild);
  Util::ObjFactory::theOne().registerCreatorFunc(&GxSeparator::make);

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "GxColor", "$Revision$");
  Tcl::defFieldContainer<GxColor>(pkg3);
  Tcl::defCreator<GxColor>(pkg3);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
