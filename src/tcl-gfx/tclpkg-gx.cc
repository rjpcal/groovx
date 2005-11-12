/** @file gfx/tclpkg-gx.cc tcl interface packages for all of the
    fundamental graphic object classes */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov  2 14:39:14 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_TCLPKG_GX_CC_UTC20050628170310_DEFINED
#define GROOVX_GFX_TCLPKG_GX_CC_UTC20050628170310_DEFINED

#include "tcl-gfx/tclpkg-gx.h"

#include "geom/bezier4.h"

#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxcamera.h"
#include "gfx/gxcolor.h"
#include "gfx/gxcylinder.h"
#include "gfx/gxdisk.h"
#include "gfx/gxdrawstyle.h"
#include "gfx/gxemptynode.h"
#include "gfx/gxlighting.h"
#include "gfx/gxline.h"
#include "gfx/gxmaterial.h"
#include "gfx/gxnode.h"
#include "gfx/gxpixmap.h"
#include "gfx/gxpointset.h"
#include "gfx/gxscaler.h"
#include "gfx/gxseparator.h"
#include "gfx/gxshapekit.h"
#include "gfx/gxsphere.h"
#include "gfx/gxtext.h"
#include "gfx/gxtransform.h"
#include "gfx/pscanvas.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/list.h"
#include "tcl/channelbuf.h"
#include "tcl/pkg.h"
#include "tcl/interp.h"
#include "tcl/tracertcl.h"

#include "tcl-gfx/recttcl.h"
#include "tcl-gfx/vectcl.h"
#include "tcl-io/fieldpkg.h"

#include "rutz/error.h"

#include "rutz/trace.h"

namespace GxTcl
{
  bool contains(nub::ref<GxNode> item, nub::ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  void savePS(nub::ref<GxNode> item, const char* filename)
  {
    Gfx::PSCanvas canvas(filename);

    item->draw(canvas);
  }

  void addChildren(nub::ref<GxSeparator> sep, tcl::list objs)
  {
    tcl::list::iterator<nub::ref<GxNode> >
      itr = objs.begin<nub::ref<GxNode> >(),
      end = objs.end<nub::ref<GxNode> >();

    while (itr != end)
      {
        sep->addChild(*itr);
        ++itr;
      }
  }

  void removeAllChildren(nub::ref<GxSeparator> sep)
  {
    while (sep->numChildren() > 0)
      sep->removeChildAt(0);
  }

  geom::rect<double> boundingBox(nub::ref<GxNode> obj,
                                 nub::soft_ref<Gfx::Canvas> canvas)
  {
    return obj->getBoundingBox(*canvas);
  }

  rutz::fstring gxsepFields()
  {
    static rutz::fstring result =
      "{addChildren 0 10 1 {NEW_GROUP STRING NO_GET}} "
      "{removeChild 0 10 1 {STRING NO_GET}} "
      "{children 0 20 1 {STRING NO_SET}} "
      "{debugMode 0 1 1 {BOOLEAN TRANSIENT}} ";
    return result;
  }

  // This is gone for now because the bitmap cache node is gone from
  // GxShapeKit, but we can resurrect a cleaner saveBitmap() function
  // pretty easily, just by captuing the screen bounds into a
  // meida::bmap_data object and then saving that.
#if 0
  void saveBitmap(nub::ref<GxNode> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif

  tcl::list bezier4(double p1, double p2, double p3, double p4,
                    unsigned int N)
  {
    GVX_TRACE("<gxtcl.cc>::bezier4");

    if (N < 4)
      {
        throw rutz::error("N must be at least 4", SRC_POS);
      }

    tcl::list result;

    geom::bezier4 bez(p1, p2, p3, p4);

    for (unsigned int i = 0; i < N; ++i)
      {
        const double u = double(i) / double(N-1);
        result.append(bez.eval(u));
      }

    return result;
  }
}

extern "C"
int Gx_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gx_Init");

  GVX_PKG_CREATE(pkg, interp, "Gx", "4.$Revision$");

  pkg->def( "bezier4", "p1 p2 p3 p4 N", &GxTcl::bezier4, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxnode_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxnode_Init");

  GVX_PKG_CREATE(pkg, interp, "GxNode", "4.$Revision$");
  pkg->inherit_pkg("io");
  tcl::def_basic_type_cmds<GxNode>(pkg, SRC_POS);

  pkg->def( "contains", "objref other_id", &GxTcl::contains, SRC_POS );
  pkg->def_vec("deepChildren", "objref(s)", &GxNode::deepChildren, 1, SRC_POS);
  pkg->def_vec("boundingBox", "objref(s) canvas", &GxTcl::boundingBox, 1, SRC_POS);
  pkg->def( "savePS", "objref filename", &GxTcl::savePS, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxseparator_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxseparator_Init");

  GVX_PKG_CREATE(pkg, interp, "GxSeparator", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_basic_type_cmds<GxSeparator>(pkg, SRC_POS);

  pkg->def( "fields", "", &GxTcl::gxsepFields, SRC_POS );
  pkg->def( "allFields", "", &GxTcl::gxsepFields, SRC_POS );

  pkg->def( "addChild", "objref child_objref", &GxSeparator::addChild, SRC_POS );
  pkg->def( "addChildren", "objref children_objref(s)", &GxTcl::addChildren, SRC_POS );
  pkg->def_getter("children", &GxSeparator::children, SRC_POS);
  pkg->def_get_set("debugMode",
                   &GxSeparator::getDebugMode,
                   &GxSeparator::setDebugMode,
                   SRC_POS);
  pkg->def_getter("numChildren", &GxSeparator::numChildren, SRC_POS);
  pkg->def("removeAllChildren", "sep_id(s)", &GxTcl::removeAllChildren, SRC_POS);
  pkg->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt, SRC_POS);
  pkg->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild, SRC_POS);
  nub::obj_factory::instance().register_creator(&GxSeparator::make);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxcolor_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxcolor_Init");

  GVX_PKG_CREATE(pkg, interp, "GxColor", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxColor>(pkg, SRC_POS);
  tcl::def_creator<GxColor>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxdrawstyle_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxdrawstyle_Init");

  GVX_PKG_CREATE(pkg, interp, "GxDrawStyle", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxDrawStyle>(pkg, SRC_POS);
  tcl::def_creator<GxDrawStyle>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxline_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxline_Init");

  GVX_PKG_CREATE(pkg, interp, "GxLine", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxLine>(pkg, SRC_POS);
  tcl::def_creator<GxLine>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxcylinder_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxcylinder_Init");

  GVX_PKG_CREATE(pkg, interp, "GxCylinder", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxCylinder>(pkg, SRC_POS);
  tcl::def_creator<GxCylinder>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxsphere_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxsphere_Init");

  GVX_PKG_CREATE(pkg, interp, "GxSphere", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxSphere>(pkg, SRC_POS);
  tcl::def_creator<GxSphere>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxlighting_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxlighting_Init");

  GVX_PKG_CREATE(pkg, interp, "GxLighting", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxLighting>(pkg, SRC_POS);
  tcl::def_creator<GxLighting>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxmaterial_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxmaterial_Init");

  GVX_PKG_CREATE(pkg, interp, "GxMaterial", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxMaterial>(pkg, SRC_POS);
  tcl::def_creator<GxMaterial>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxpointset_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpointset_Init");

  GVX_PKG_CREATE(pkg, interp, "GxPointSet", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::defFieldContainer<GxPointSet>(pkg, SRC_POS);
  tcl::def_creator<GxPointSet>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxscaler_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxscaler_Init");

  GVX_PKG_CREATE(pkg, interp, "GxScaler", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_basic_type_cmds<GxScaler>(pkg, SRC_POS);
  tcl::def_creator<GxScaler>(pkg);
  pkg->def_get_set("child", &GxScaler::child, &GxScaler::setChild, SRC_POS);
  pkg->def_get_set("mode", &GxScaler::getMode, &GxScaler::setMode, SRC_POS);
  pkg->def_get_set("width", &GxScaler::scaledWidth, &GxScaler::setWidth, SRC_POS);
  pkg->def_get_set("height", &GxScaler::scaledHeight, &GxScaler::setHeight, SRC_POS);
  pkg->def_get_set("maxDim", &GxScaler::scaledMaxDim, &GxScaler::setMaxDim, SRC_POS);
  pkg->def_get_set("aspect", &GxScaler::aspectRatio, &GxScaler::setAspectRatio, SRC_POS);
  pkg->def_get_set("widthFactor", &GxScaler::widthFactor, &GxScaler::setWidthFactor, SRC_POS);
  pkg->def_get_set("heightFactor", &GxScaler::heightFactor, &GxScaler::setHeightFactor, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxemptynode_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxemptynode_Init");

  GVX_PKG_CREATE(pkg, interp, "GxEmptyNode", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_basic_type_cmds<GxEmptyNode>(pkg, SRC_POS);
  tcl::def_creator<GxEmptyNode>(pkg);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxtransform_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxtransform_Init");

  GVX_PKG_CREATE(pkg, interp, "GxTransform", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_creator<GxTransform>(pkg);
  tcl::defFieldContainer<GxTransform>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxshapekit_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxshapekit_Init");

  GVX_PKG_CREATE(pkg, interp, "GxShapeKit", "4.$Revision$");
  pkg->inherit_pkg("GxNode");

  tcl::def_tracing(pkg, GxShapeKit::tracer);

  tcl::defFieldContainer<GxShapeKit>(pkg, SRC_POS);

#if 0
  pkg->def_vec( "saveBitmap", "objref(s) filename(s)", &GxTcl::saveBitmap, 1, SRC_POS );
#endif

  pkg->def_get_set("category", &GxShapeKit::category, &GxShapeKit::setCategory, SRC_POS);

  pkg->link_var_copy("GxShapeKit::DIRECT", GxCache::DIRECT);
  pkg->link_var_copy("GxShapeKit::GLCOMPILE", GxCache::GLCOMPILE);

  pkg->link_var_copy("GxShapeKit::NATIVE_SCALING", GxScaler::NATIVE_SCALING);
  pkg->link_var_copy("GxShapeKit::MAINTAIN_ASPECT_SCALING", GxScaler::MAINTAIN_ASPECT_SCALING);
  pkg->link_var_copy("GxShapeKit::FREE_SCALING", GxScaler::FREE_SCALING);

  pkg->link_var_copy("GxShapeKit::NATIVE_ALIGNMENT", GxAligner::NATIVE_ALIGNMENT);
  pkg->link_var_copy("GxShapeKit::CENTER_ON_CENTER", GxAligner::CENTER_ON_CENTER);
  pkg->link_var_copy("GxShapeKit::NW_ON_CENTER", GxAligner::NW_ON_CENTER);
  pkg->link_var_copy("GxShapeKit::NE_ON_CENTER", GxAligner::NE_ON_CENTER);
  pkg->link_var_copy("GxShapeKit::SW_ON_CENTER", GxAligner::SW_ON_CENTER);
  pkg->link_var_copy("GxShapeKit::SE_ON_CENTER", GxAligner::SE_ON_CENTER);
  pkg->link_var_copy("GxShapeKit::ARBITRARY_ON_CENTER", GxAligner::ARBITRARY_ON_CENTER);

  GVX_PKG_RETURN(pkg);
}

namespace
{
  void scramble1(nub::ref<GxPixmap> pixmap,
                 int numsubcols, int numsubrows)
  {
    pixmap->scramble(numsubcols, numsubrows, 0);
  }

  void scramble2(nub::ref<GxPixmap> pixmap,
                 int numsubcols, int numsubrows, int seed)
  {
    pixmap->scramble(numsubcols, numsubrows, seed);
  }

  void loadImageStream(tcl::call_context& ctx)
  {
    using rutz::shared_ptr;
    nub::ref<GxPixmap> pixmap = ctx.get_arg<nub::ref<GxPixmap> >(1);
    const char* channame = ctx.get_arg<const char*>(2);
    Tcl_Interp* interp = ctx.interp().intp();
    shared_ptr<std::istream> ist(tcl::ichanopen(interp, channame));
    pixmap->loadImageStream(*ist);
  }
}

extern "C"
int Gxpixmap_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpixmap_Init");

  GVX_PKG_CREATE(pkg, interp, "GxPixmap", "4.$Revision$");
  pkg->inherit_pkg("GxShapeKit");
  tcl::def_creator<GxPixmap>(pkg, "Bitmap");
  tcl::def_basic_type_cmds<GxPixmap>(pkg, SRC_POS);


  pkg->def_get_set("asBitmap", &GxPixmap::getAsBitmap, &GxPixmap::setAsBitmap, SRC_POS);
  pkg->def_getter("checkSum", &GxPixmap::checkSum, SRC_POS);
  pkg->def_getter("filename", &GxPixmap::filename, SRC_POS );
  pkg->def_action("flipContrast", &GxPixmap::flipContrast, SRC_POS);
  pkg->def_action("flipVertical", &GxPixmap::flipVertical, SRC_POS);
  pkg->def_vec("grabScreenRect", "objref(s) canvas {left top right bottom}",
               &GxPixmap::grabScreenRect, 1,
               SRC_POS );
  pkg->def_vec("grabScreen", "objref(s) canvas", &GxPixmap::grabScreen, 1, SRC_POS );
  pkg->def_vec("grabWorldRect", "objref(s) canvas {left top right bottom}",
               &GxPixmap::grabWorldRect, 1,
               SRC_POS );
  pkg->def_raw("loadImageStream", tcl::arg_spec(3),
               "objref channame", &loadImageStream, SRC_POS);
  pkg->def_vec("loadImage", "objref(s) filename(s)", &GxPixmap::loadImage, 1, SRC_POS );
  pkg->def_get_set("purgeable", &GxPixmap::isPurgeable, &GxPixmap::setPurgeable, SRC_POS);
  pkg->def_vec("queueImage", "objref(s) filename(s)", &GxPixmap::queueImage, 1, SRC_POS );
  pkg->def_action("reload", &GxPixmap::reload, SRC_POS);
  pkg->def_vec("saveImage", "objref(s) filename(s)", &GxPixmap::saveImage, 1, SRC_POS );
  pkg->def_vec("scramble", "numcols numrows", &scramble1, 1, SRC_POS);
  pkg->def_vec("scramble", "numcols numrows ?seed?", &scramble2, 1, SRC_POS);
  pkg->def_vec("scramble", "numcols numrows ?seed moveParts flipLR flipTB?",
               &GxPixmap::scramble, 1,
               SRC_POS);
  pkg->def_getter("size", &GxPixmap::size, SRC_POS);
  pkg->def_get_set("usingZoom", &GxPixmap::getUsingZoom, &GxPixmap::setUsingZoom, SRC_POS);
  pkg->def_get_set("zoom", &GxPixmap::getZoom, &GxPixmap::setZoom, SRC_POS);
  pkg->def_setter("zoomTo", &GxPixmap::zoomTo, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxtext_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxtext_Init");

  GVX_PKG_CREATE(pkg, interp, "GxText", "4.$Revision$");
  pkg->inherit_pkg("GxShapeKit");
  tcl::def_creator<GxText>(pkg, "Gtext");
  tcl::defFieldContainer<GxText>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}


//
// Cameras
//

extern "C"
int Gxfixedscalecamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxfixedscalecamera_Init");

  GVX_PKG_CREATE(pkg, interp, "GxFixedScaleCamera", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_creator<GxFixedScaleCamera>(pkg);
  tcl::defFieldContainer<GxFixedScaleCamera>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxpsyphycamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpsyphycamera_Init");

  GVX_PKG_CREATE(pkg, interp, "GxPsyphyCamera", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_creator<GxPsyphyCamera>(pkg);
  tcl::defFieldContainer<GxPsyphyCamera>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxperspectivecamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxperspectivecamera_Init");

  GVX_PKG_CREATE(pkg, interp, "GxPerspectiveCamera", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_creator<GxPerspectiveCamera>(pkg);
  tcl::defFieldContainer<GxPerspectiveCamera>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Gxdisk_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxdisk_Init");

  GVX_PKG_CREATE(pkg, interp, "GxDisk", "4.$Revision$");
  pkg->inherit_pkg("GxNode");
  tcl::def_creator<GxDisk>(pkg);
  tcl::defFieldContainer<GxDisk>(pkg, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_gfx_tclpkg_gx_cc_utc20050628170310[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_TCLPKG_GX_CC_UTC20050628170310_DEFINED
