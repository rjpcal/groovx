///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Tue Nov  6 16:07:42 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gfx/gxcolor.h"
#include "gfx/gxdrawstyle.h"
#include "gfx/gxnode.h"
#include "gfx/gxseparator.h"
#include "gfx/pscanvas.h"

#include "tcl/fieldpkg.h"
#include "tcl/itertcl.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace GxTcl
{
  bool contains(Ref<GxNode> item, Ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  void savePS(Ref<GxNode> item, const char* filename)
  {
    Gfx::PSCanvas canvas(filename);

    item->draw(canvas);
  }

  void addChildren(Ref<GxSeparator> sep, Tcl::List objs)
  {
    Tcl::List::Iterator<Ref<GxNode> >
      itr = objs.begin<Ref<GxNode> >(),
      end = objs.end<Ref<GxNode> >();

    while (itr != end)
      {
        sep->addChild(*itr);
        ++itr;
      }
  }

  fstring gxsepFields()
  {
    static fstring result =
      "{addChildren 0 10 1 {NEW_GROUP STRING NO_GET}} "
      "{removeChild 0 10 1 {STRING NO_GET}} "
      "{children 0 20 1 {STRING NO_SET}} ";
    return result;
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
  pkg1->def( "savePS", "item_id filename", &GxTcl::savePS );


  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "GxSeparator", "$Revision$");
  Tcl::defGenericObjCmds<GxSeparator>(pkg2);

  pkg2->def( "fields", "", &GxTcl::gxsepFields );
  pkg2->def( "allFields", "", &GxTcl::gxsepFields );

  pkg2->def( "addChild", "item_id child_item_id", &GxSeparator::addChild );
  pkg2->def( "addChildren", "item_id children_id(s)", &GxTcl::addChildren );
  pkg2->defGetter("children", &GxSeparator::children);
  pkg2->defGetter("numChildren", &GxSeparator::numChildren);
  pkg2->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt);
  pkg2->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild);
  Util::ObjFactory::theOne().registerCreatorFunc(&GxSeparator::make);

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "GxColor", "$Revision$");
  Tcl::defFieldContainer<GxColor>(pkg3);
  Tcl::defCreator<GxColor>(pkg3);

  Tcl::Pkg* pkg4 = new Tcl::Pkg(interp, "GxDrawStyle", "$Revision$");
  Tcl::defFieldContainer<GxDrawStyle>(pkg4);
  Tcl::defCreator<GxDrawStyle>(pkg4);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3, pkg4);
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
