///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov  2 14:39:14 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

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
#include "gfx/pointtcl.h"
#include "gfx/recttcl.h"

#include "io/fieldpkg.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclchannelbuf.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"
#include "tcl/tracertcl.h"

#include "util/error.h"

#include "util/trace.h"

namespace GxTcl
{
  bool contains(Nub::Ref<GxNode> item, Nub::Ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  void savePS(Nub::Ref<GxNode> item, const char* filename)
  {
    Gfx::PSCanvas canvas(filename);

    item->draw(canvas);
  }

  void addChildren(Nub::Ref<GxSeparator> sep, Tcl::List objs)
  {
    Tcl::List::Iterator<Nub::Ref<GxNode> >
      itr = objs.begin<Nub::Ref<GxNode> >(),
      end = objs.end<Nub::Ref<GxNode> >();

    while (itr != end)
      {
        sep->addChild(*itr);
        ++itr;
      }
  }

  void removeAllChildren(Nub::Ref<GxSeparator> sep)
  {
    while (sep->numChildren() > 0)
      sep->removeChildAt(0);
  }

  geom::rect<double> boundingBox(Nub::Ref<GxNode> obj,
                                 Nub::SoftRef<Gfx::Canvas> canvas)
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
  void saveBitmap(Nub::Ref<GxNode> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif

  Tcl::List bezier4(double p1, double p2, double p3, double p4,
                    unsigned int N)
  {
    DOTRACE("<gxtcl.cc>::bezier4");

    if (N < 4)
      {
        throw rutz::error("N must be at least 4", SRC_POS);
      }

    Tcl::List result;

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
DOTRACE("Gx_Init");

  PKG_CREATE(interp, "Gx", "$Revision$");

  pkg->def( "bezier4", "p1 p2 p3 p4 N", &GxTcl::bezier4, SRC_POS );

  PKG_RETURN;
}

extern "C"
int Gxnode_Init(Tcl_Interp* interp)
{
DOTRACE("Gxnode_Init");

  PKG_CREATE(interp, "GxNode", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<GxNode>(pkg, SRC_POS);

  pkg->def( "contains", "objref other_id", &GxTcl::contains, SRC_POS );
  pkg->defVec("deepChildren", "objref(s)", &GxNode::deepChildren, 1, SRC_POS);
  pkg->defVec("boundingBox", "objref(s) canvas", &GxTcl::boundingBox, 1, SRC_POS);
  pkg->def( "savePS", "objref filename", &GxTcl::savePS, SRC_POS );

  PKG_RETURN;
}

extern "C"
int Gxseparator_Init(Tcl_Interp* interp)
{
DOTRACE("Gxseparator_Init");

  PKG_CREATE(interp, "GxSeparator", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defGenericObjCmds<GxSeparator>(pkg, SRC_POS);

  pkg->def( "fields", "", &GxTcl::gxsepFields, SRC_POS );
  pkg->def( "allFields", "", &GxTcl::gxsepFields, SRC_POS );

  pkg->def( "addChild", "objref child_objref", &GxSeparator::addChild, SRC_POS );
  pkg->def( "addChildren", "objref children_objref(s)", &GxTcl::addChildren, SRC_POS );
  pkg->defGetter("children", &GxSeparator::children, SRC_POS);
  pkg->defAttrib("debugMode",
                 &GxSeparator::getDebugMode,
                 &GxSeparator::setDebugMode,
                 SRC_POS);
  pkg->defGetter("numChildren", &GxSeparator::numChildren, SRC_POS);
  pkg->def("removeAllChildren", "sep_id(s)", &GxTcl::removeAllChildren, SRC_POS);
  pkg->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt, SRC_POS);
  pkg->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild, SRC_POS);
  Nub::ObjFactory::theOne().register_creator(&GxSeparator::make);

  PKG_RETURN;
}

extern "C"
int Gxcolor_Init(Tcl_Interp* interp)
{
DOTRACE("Gxcolor_Init");

  PKG_CREATE(interp, "GxColor", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxColor>(pkg, SRC_POS);
  Tcl::defCreator<GxColor>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxdrawstyle_Init(Tcl_Interp* interp)
{
DOTRACE("Gxdrawstyle_Init");

  PKG_CREATE(interp, "GxDrawStyle", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxDrawStyle>(pkg, SRC_POS);
  Tcl::defCreator<GxDrawStyle>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxline_Init(Tcl_Interp* interp)
{
DOTRACE("Gxline_Init");

  PKG_CREATE(interp, "GxLine", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxLine>(pkg, SRC_POS);
  Tcl::defCreator<GxLine>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxcylinder_Init(Tcl_Interp* interp)
{
DOTRACE("Gxcylinder_Init");

  PKG_CREATE(interp, "GxCylinder", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxCylinder>(pkg, SRC_POS);
  Tcl::defCreator<GxCylinder>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxsphere_Init(Tcl_Interp* interp)
{
DOTRACE("Gxsphere_Init");

  PKG_CREATE(interp, "GxSphere", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxSphere>(pkg, SRC_POS);
  Tcl::defCreator<GxSphere>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxlighting_Init(Tcl_Interp* interp)
{
DOTRACE("Gxlighting_Init");

  PKG_CREATE(interp, "GxLighting", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxLighting>(pkg, SRC_POS);
  Tcl::defCreator<GxLighting>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxmaterial_Init(Tcl_Interp* interp)
{
DOTRACE("Gxmaterial_Init");

  PKG_CREATE(interp, "GxMaterial", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxMaterial>(pkg, SRC_POS);
  Tcl::defCreator<GxMaterial>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxpointset_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpointset_Init");

  PKG_CREATE(interp, "GxPointSet", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxPointSet>(pkg, SRC_POS);
  Tcl::defCreator<GxPointSet>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxscaler_Init(Tcl_Interp* interp)
{
DOTRACE("Gxscaler_Init");

  PKG_CREATE(interp, "GxScaler", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxScaler>(pkg);
  pkg->defAttrib("child", &GxScaler::child, &GxScaler::setChild, SRC_POS);
  pkg->defAttrib("mode", &GxScaler::getMode, &GxScaler::setMode, SRC_POS);
  pkg->defAttrib("width", &GxScaler::scaledWidth, &GxScaler::setWidth, SRC_POS);
  pkg->defAttrib("height", &GxScaler::scaledHeight, &GxScaler::setHeight, SRC_POS);
  pkg->defAttrib("maxDim", &GxScaler::scaledMaxDim, &GxScaler::setMaxDim, SRC_POS);
  pkg->defAttrib("aspect", &GxScaler::aspectRatio, &GxScaler::setAspectRatio, SRC_POS);
  pkg->defAttrib("widthFactor", &GxScaler::widthFactor, &GxScaler::setWidthFactor, SRC_POS);
  pkg->defAttrib("heightFactor", &GxScaler::heightFactor, &GxScaler::setHeightFactor, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxemptynode_Init(Tcl_Interp* interp)
{
DOTRACE("Gxemptynode_Init");

  PKG_CREATE(interp, "GxEmptyNode", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxEmptyNode>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxtransform_Init(Tcl_Interp* interp)
{
DOTRACE("Gxtransform_Init");

  PKG_CREATE(interp, "GxTransform", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxTransform>(pkg);
  Tcl::defFieldContainer<GxTransform>(pkg, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxshapekit_Init(Tcl_Interp* interp)
{
DOTRACE("Gxshapekit_Init");

  PKG_CREATE(interp, "GxShapeKit", "$Revision$");
  pkg->inheritPkg("GxNode");

  Tcl::defTracing(pkg, GxShapeKit::tracer);

  Tcl::defFieldContainer<GxShapeKit>(pkg, SRC_POS);

#if 0
  pkg->defVec( "saveBitmap", "objref(s) filename(s)", &GxTcl::saveBitmap, 1, SRC_POS );
#endif

  pkg->defAttrib("category", &GxShapeKit::category, &GxShapeKit::setCategory, SRC_POS);

  pkg->linkVarCopy("GxShapeKit::DIRECT", GxCache::DIRECT);
  pkg->linkVarCopy("GxShapeKit::GLCOMPILE", GxCache::GLCOMPILE);

  pkg->linkVarCopy("GxShapeKit::NATIVE_SCALING", GxScaler::NATIVE_SCALING);
  pkg->linkVarCopy("GxShapeKit::MAINTAIN_ASPECT_SCALING", GxScaler::MAINTAIN_ASPECT_SCALING);
  pkg->linkVarCopy("GxShapeKit::FREE_SCALING", GxScaler::FREE_SCALING);

  pkg->linkVarCopy("GxShapeKit::NATIVE_ALIGNMENT", GxAligner::NATIVE_ALIGNMENT);
  pkg->linkVarCopy("GxShapeKit::CENTER_ON_CENTER", GxAligner::CENTER_ON_CENTER);
  pkg->linkVarCopy("GxShapeKit::NW_ON_CENTER", GxAligner::NW_ON_CENTER);
  pkg->linkVarCopy("GxShapeKit::NE_ON_CENTER", GxAligner::NE_ON_CENTER);
  pkg->linkVarCopy("GxShapeKit::SW_ON_CENTER", GxAligner::SW_ON_CENTER);
  pkg->linkVarCopy("GxShapeKit::SE_ON_CENTER", GxAligner::SE_ON_CENTER);
  pkg->linkVarCopy("GxShapeKit::ARBITRARY_ON_CENTER", GxAligner::ARBITRARY_ON_CENTER);

  PKG_RETURN;
}

namespace
{
  void scramble1(Nub::Ref<GxPixmap> pixmap,
                 int numsubcols, int numsubrows)
  {
    pixmap->scramble(numsubcols, numsubrows, 0);
  }

  void scramble2(Nub::Ref<GxPixmap> pixmap,
                 int numsubcols, int numsubrows, int seed)
  {
    pixmap->scramble(numsubcols, numsubrows, seed);
  }

  void loadImageStream(Tcl::Context& ctx)
  {
    using rutz::shared_ptr;
    Nub::Ref<GxPixmap> pixmap = ctx.getValFromArg<Nub::Ref<GxPixmap> >(1);
    const char* channame = ctx.getValFromArg<const char*>(2);
    Tcl_Interp* interp = ctx.interp().intp();
    shared_ptr<std::istream> ist(Tcl::ichanopen(interp, channame));
    pixmap->loadImageStream(*ist);
  }
}

extern "C"
int Gxpixmap_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpixmap_Init");

  PKG_CREATE(interp, "GxPixmap", "$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defCreator<GxPixmap>(pkg, "Bitmap");
  Tcl::defGenericObjCmds<GxPixmap>(pkg, SRC_POS);


  pkg->defAttrib("asBitmap", &GxPixmap::getAsBitmap, &GxPixmap::setAsBitmap, SRC_POS);
  pkg->defGetter("checkSum", &GxPixmap::checkSum, SRC_POS);
  pkg->defGetter("filename", &GxPixmap::filename, SRC_POS );
  pkg->defAction("flipContrast", &GxPixmap::flipContrast, SRC_POS);
  pkg->defAction("flipVertical", &GxPixmap::flipVertical, SRC_POS);
  pkg->defVec("grabScreenRect", "objref(s) canvas {left top right bottom}",
              &GxPixmap::grabScreenRect, 1,
              SRC_POS );
  pkg->defVec("grabScreen", "objref(s) canvas", &GxPixmap::grabScreen, 1, SRC_POS );
  pkg->defVec("grabWorldRect", "objref(s) canvas {left top right bottom}",
              &GxPixmap::grabWorldRect, 1,
              SRC_POS );
  pkg->defRaw("loadImageStream", 2, "objref channame", &loadImageStream, SRC_POS);
  pkg->defVec("loadImage", "objref(s) filename(s)", &GxPixmap::loadImage, 1, SRC_POS );
  pkg->defAttrib("purgeable", &GxPixmap::isPurgeable, &GxPixmap::setPurgeable, SRC_POS);
  pkg->defVec("queueImage", "objref(s) filename(s)", &GxPixmap::queueImage, 1, SRC_POS );
  pkg->defAction("reload", &GxPixmap::reload, SRC_POS);
  pkg->defVec("saveImage", "objref(s) filename(s)", &GxPixmap::saveImage, 1, SRC_POS );
  pkg->defVec("scramble", "numcols numrows", &scramble1, 1, SRC_POS);
  pkg->defVec("scramble", "numcols numrows ?seed?", &scramble2, 1, SRC_POS);
  pkg->defVec("scramble", "numcols numrows ?seed moveParts flipLR flipTB?",
              &GxPixmap::scramble, 1,
              SRC_POS);
  pkg->defGetter("size", &GxPixmap::size, SRC_POS);
  pkg->defAttrib("usingZoom", &GxPixmap::getUsingZoom, &GxPixmap::setUsingZoom, SRC_POS);
  pkg->defAttrib("zoom", &GxPixmap::getZoom, &GxPixmap::setZoom, SRC_POS);
  pkg->defSetter("zoomTo", &GxPixmap::zoomTo, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxtext_Init(Tcl_Interp* interp)
{
DOTRACE("Gxtext_Init");

  PKG_CREATE(interp, "GxText", "$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defCreator<GxText>(pkg, "Gtext");
  Tcl::defFieldContainer<GxText>(pkg, SRC_POS);

  PKG_RETURN;
}


//
// Cameras
//

extern "C"
int Gxfixedscalecamera_Init(Tcl_Interp* interp)
{
DOTRACE("Gxfixedscalecamera_Init");

  PKG_CREATE(interp, "GxFixedScaleCamera", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxFixedScaleCamera>(pkg);
  Tcl::defFieldContainer<GxFixedScaleCamera>(pkg, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxpsyphycamera_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpsyphycamera_Init");

  PKG_CREATE(interp, "GxPsyphyCamera", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxPsyphyCamera>(pkg);
  Tcl::defFieldContainer<GxPsyphyCamera>(pkg, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxperspectivecamera_Init(Tcl_Interp* interp)
{
DOTRACE("Gxperspectivecamera_Init");

  PKG_CREATE(interp, "GxPerspectiveCamera", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxPerspectiveCamera>(pkg);
  Tcl::defFieldContainer<GxPerspectiveCamera>(pkg, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Gxdisk_Init(Tcl_Interp* interp)
{
DOTRACE("Gxdisk_Init");

  PKG_CREATE(interp, "GxDisk", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxDisk>(pkg);
  Tcl::defFieldContainer<GxDisk>(pkg, SRC_POS);

  PKG_RETURN;
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
