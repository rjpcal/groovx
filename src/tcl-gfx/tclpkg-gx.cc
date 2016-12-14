/** @file gfx/tclpkg-gx.cc tcl interface packages for all of the
    fundamental graphic object classes */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov  2 14:39:14 2000
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

namespace
{
  bool gxtcl_contains(nub::ref<GxNode> item, nub::ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  void gxtcl_savePS(nub::ref<GxNode> item, const char* filename)
  {
    Gfx::PSCanvas canvas(filename);

    item->draw(canvas);
  }

  void gxtcl_addChildren(nub::ref<GxSeparator> sep, const tcl::list& objs)
  {
    for (const auto& noderef: objs.view_of<nub::ref<GxNode>>())
      sep->addChild(noderef);
  }

  void gxtcl_removeAllChildren(nub::ref<GxSeparator> sep)
  {
    while (sep->numChildren() > 0)
      sep->removeChildAt(0);
  }

  geom::rect<double> gxtcl_boundingBox(nub::ref<GxNode> obj,
                                       nub::soft_ref<Gfx::Canvas> canvas)
  {
    return obj->getBoundingBox(*canvas);
  }

  rutz::fstring gxtcl_gxsepFields()
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
  // media::bmap_data object and then saving that.
#if 0
  void gxtcl_saveBitmap(nub::ref<GxNode> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif

  tcl::list gxtcl_bezier4(double p1, double p2, double p3, double p4,
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

  return tcl::pkg::init
    (interp, "Gx", "4.0",
     [](tcl::pkg* pkg) {
      pkg->def( "bezier4", "p1 p2 p3 p4 N", &gxtcl_bezier4, SRC_POS );
    });
}

extern "C"
int Gxnode_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxnode_Init");

  return tcl::pkg::init
    (interp, "GxNode", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<GxNode>(pkg, SRC_POS);

      pkg->def( "contains", "objref other_id", &gxtcl_contains, SRC_POS );
      pkg->def_vec("deepChildren", "objref(s)", &GxNode::deepChildren, 1, SRC_POS);
      pkg->def_vec("boundingBox", "objref(s) canvas", &gxtcl_boundingBox, 1, SRC_POS);
      pkg->def( "savePS", "objref filename", &gxtcl_savePS, SRC_POS );
    });
}

extern "C"
int Gxseparator_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxseparator_Init");

  return tcl::pkg::init
    (interp, "GxSeparator", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_basic_type_cmds<GxSeparator>(pkg, SRC_POS);
      tcl::def_creator<GxSeparator>(pkg);

      pkg->def( "fields", "", &gxtcl_gxsepFields, SRC_POS );
      pkg->def( "allFields", "", &gxtcl_gxsepFields, SRC_POS );

      pkg->def( "addChild", "objref child_objref", &GxSeparator::addChild, SRC_POS );
      pkg->def( "addChildren", "objref children_objref(s)", &gxtcl_addChildren, SRC_POS );
      pkg->def_getter("children", &GxSeparator::children, SRC_POS);
      pkg->def_get_set("debugMode",
                       &GxSeparator::getDebugMode,
                       &GxSeparator::setDebugMode,
                       SRC_POS);
      pkg->def_getter("numChildren", &GxSeparator::numChildren, SRC_POS);
      pkg->def("removeAllChildren", "sep_id(s)", &gxtcl_removeAllChildren, SRC_POS);
      pkg->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt, SRC_POS);
      pkg->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild, SRC_POS);
    });
}

extern "C"
int Gxcolor_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxcolor_Init");

  return tcl::pkg::init
    (interp, "GxColor", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxColor>(pkg, SRC_POS);
      tcl::def_creator<GxColor>(pkg);
    });
}

extern "C"
int Gxdrawstyle_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxdrawstyle_Init");

  return tcl::pkg::init
    (interp, "GxDrawStyle", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxDrawStyle>(pkg, SRC_POS);
      tcl::def_creator<GxDrawStyle>(pkg);
    });
}

extern "C"
int Gxline_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxline_Init");

  return tcl::pkg::init
    (interp, "GxLine", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxLine>(pkg, SRC_POS);
      tcl::def_creator<GxLine>(pkg);
    });
}

extern "C"
int Gxcylinder_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxcylinder_Init");

  return tcl::pkg::init
    (interp, "GxCylinder", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxCylinder>(pkg, SRC_POS);
      tcl::def_creator<GxCylinder>(pkg);
    });
}

extern "C"
int Gxsphere_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxsphere_Init");

  return tcl::pkg::init
    (interp, "GxSphere", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxSphere>(pkg, SRC_POS);
      tcl::def_creator<GxSphere>(pkg);
    });
}

extern "C"
int Gxlighting_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxlighting_Init");

  return tcl::pkg::init
    (interp, "GxLighting", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxLighting>(pkg, SRC_POS);
      tcl::def_creator<GxLighting>(pkg);
    });
}

extern "C"
int Gxmaterial_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxmaterial_Init");

  return tcl::pkg::init
    (interp, "GxMaterial", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxMaterial>(pkg, SRC_POS);
      tcl::def_creator<GxMaterial>(pkg);
    });
}

extern "C"
int Gxpointset_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpointset_Init");

  return tcl::pkg::init
    (interp, "GxPointSet", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::defFieldContainer<GxPointSet>(pkg, SRC_POS);
      tcl::def_creator<GxPointSet>(pkg);
    });
}

extern "C"
int Gxscaler_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxscaler_Init");

  return tcl::pkg::init
    (interp, "GxScaler", "4.0",
     [](tcl::pkg* pkg) {
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
    });
}

extern "C"
int Gxemptynode_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxemptynode_Init");

  return tcl::pkg::init
    (interp, "GxEmptyNode", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_basic_type_cmds<GxEmptyNode>(pkg, SRC_POS);
      tcl::def_creator<GxEmptyNode>(pkg);
    });
}

extern "C"
int Gxtransform_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxtransform_Init");

  return tcl::pkg::init
    (interp, "GxTransform", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_creator<GxTransform>(pkg);
      tcl::defFieldContainer<GxTransform>(pkg, SRC_POS);
    });
}

extern "C"
int Gxshapekit_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxshapekit_Init");

  return tcl::pkg::init
    (interp, "GxShapeKit", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");

      tcl::def_tracing(pkg, GxShapeKit::tracer);

      tcl::defFieldContainer<GxShapeKit>(pkg, SRC_POS);

#if 0
      pkg->def_vec( "saveBitmap", "objref(s) filename(s)", &gxtcl_saveBitmap, 1, SRC_POS );
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
    });
}

namespace
{
  void scramble1(nub::ref<GxPixmap> pixmap,
                 unsigned int numsubcols, unsigned int numsubrows)
  {
    pixmap->scramble(numsubcols, numsubrows, 0);
  }

  void scramble2(nub::ref<GxPixmap> pixmap,
                 unsigned int numsubcols, unsigned int numsubrows, unsigned long seed)
  {
    pixmap->scramble(numsubcols, numsubrows, seed);
  }

  void loadImageStream(tcl::call_context& ctx)
  {
    nub::ref<GxPixmap> pixmap = ctx.get_arg<nub::ref<GxPixmap> >(1);
    const char* channame = ctx.get_arg<const char*>(2);
    Tcl_Interp* interp = ctx.interp().intp();
    std::unique_ptr<std::istream> ist(tcl::ichanopen(interp, channame));
    pixmap->loadImageStream(*ist);
  }

  void saveImageStream(tcl::call_context& ctx)
  {
    nub::ref<GxPixmap> pixmap = ctx.get_arg<nub::ref<GxPixmap> >(1);
    const char* channame = ctx.get_arg<const char*>(2);
    Tcl_Interp* interp = ctx.interp().intp();
    std::unique_ptr<std::ostream> ost(tcl::ochanopen(interp, channame));
    pixmap->saveImageStream(*ost);
    // ost->flush();
  }
}

extern "C"
int Gxpixmap_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpixmap_Init");

  return tcl::pkg::init
    (interp, "GxPixmap", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxShapeKit");
      tcl::def_creator<GxPixmap>(pkg, "Bitmap");
      tcl::def_basic_type_cmds<GxPixmap>(pkg, SRC_POS);

      pkg->def_get_set("asBitmap", &GxPixmap::getAsBitmap, &GxPixmap::setAsBitmap, SRC_POS);
      pkg->def_getter("checkSum", &GxPixmap::checkSum, SRC_POS);
      pkg->def_getter("bkdrHash", &GxPixmap::bkdrHash, SRC_POS);
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
      pkg->def_raw("saveImageStream", tcl::arg_spec(3),
                   "objref channame", &saveImageStream, SRC_POS);
      pkg->def_vec("scramble", "numcols numrows", &scramble1, 1, SRC_POS);
      pkg->def_vec("scramble", "numcols numrows ?seed?", &scramble2, 1, SRC_POS);
      pkg->def_vec("scramble", "numcols numrows ?seed moveParts flipLR flipTB?",
                   &GxPixmap::scramble, 1,
                   SRC_POS);
      pkg->def_getter("size", &GxPixmap::size, SRC_POS);
      pkg->def_get_set("usingZoom", &GxPixmap::getUsingZoom, &GxPixmap::setUsingZoom, SRC_POS);
      pkg->def_get_set("zoom", &GxPixmap::getZoom, &GxPixmap::setZoom, SRC_POS);
      pkg->def_setter("zoomTo", &GxPixmap::zoomTo, SRC_POS);
    });
}

extern "C"
int Gxtext_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxtext_Init");

  return tcl::pkg::init
    (interp, "GxText", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxShapeKit");
      tcl::def_creator<GxText>(pkg, "Gtext");
      tcl::defFieldContainer<GxText>(pkg, SRC_POS);
    });
}


//
// Cameras
//

extern "C"
int Gxfixedscalecamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxfixedscalecamera_Init");

  return tcl::pkg::init
    (interp, "GxFixedScaleCamera", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_creator<GxFixedScaleCamera>(pkg);
      tcl::defFieldContainer<GxFixedScaleCamera>(pkg, SRC_POS);
    });
}

extern "C"
int Gxpsyphycamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxpsyphycamera_Init");

  return tcl::pkg::init
    (interp, "GxPsyphyCamera", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_creator<GxPsyphyCamera>(pkg);
      tcl::defFieldContainer<GxPsyphyCamera>(pkg, SRC_POS);
    });
}

extern "C"
int Gxperspectivecamera_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxperspectivecamera_Init");

  return tcl::pkg::init
    (interp, "GxPerspectiveCamera", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_creator<GxPerspectiveCamera>(pkg);
      tcl::defFieldContainer<GxPerspectiveCamera>(pkg, SRC_POS);
    });
}

extern "C"
int Gxdisk_Init(Tcl_Interp* interp)
{
GVX_TRACE("Gxdisk_Init");

  return tcl::pkg::init
    (interp, "GxDisk", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("GxNode");
      tcl::def_creator<GxDisk>(pkg);
      tcl::defFieldContainer<GxDisk>(pkg, SRC_POS);
    });
}
