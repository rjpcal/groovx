///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  2 14:39:14 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

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
#include "io/ioproxy.h"
#include "io/reader.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace GxTcl
{
  bool contains(Util::Ref<GxNode> item, Util::Ref<GxNode> other)
  {
    return item->contains(other.get());
  }

  void savePS(Util::Ref<GxNode> item, const char* filename)
  {
    Gfx::PSCanvas canvas(filename);

    item->draw(canvas);
  }

  void addChildren(Util::Ref<GxSeparator> sep, Tcl::List objs)
  {
    Tcl::List::Iterator<Util::Ref<GxNode> >
      itr = objs.begin<Util::Ref<GxNode> >(),
      end = objs.end<Util::Ref<GxNode> >();

    while (itr != end)
      {
        sep->addChild(*itr);
        ++itr;
      }
  }

  Gfx::Rect<double> boundingBox(Util::Ref<GxNode> obj)
  {
    return obj->getBoundingBox(Gfx::Canvas::current());
  }

  fstring gxsepFields()
  {
    static fstring result =
      "{addChildren 0 10 1 {NEW_GROUP STRING NO_GET}} "
      "{removeChild 0 10 1 {STRING NO_GET}} "
      "{children 0 20 1 {STRING NO_SET}} "
      "{debugMode 0 1 1 {BOOLEAN TRANSIENT}} ";
    return result;
  }

  // This is gone for now because the bitmap cache node is gone from
  // GxShapeKit, but we can resurrect a cleaner saveBitmap() function
  // pretty easily, just by captuing the screen bounds into a BmapData
  // object and then saving that.
#if 0
  void saveBitmap(Util::Ref<GxNode> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif
}

// This class is for backward-compatibility, to allow us to deserialize
// XBitmap and GLBitmap objects from old expt files.
class CompatBitmap : public GxPixmap
{
public:
  bool isGL;

  CompatBitmap(bool gl) : GxPixmap(), isGL(gl) {}

  static CompatBitmap* makeGL() { return new CompatBitmap(true); }
  static CompatBitmap* makeX() { return new CompatBitmap(false); }

  // no writeTo() or serialVersionId() overrides since we just want
  // GxPixmap's versions

  virtual fstring objTypename() const { return fstring("GxPixmap"); }

  virtual void readFrom(IO::Reader& reader)
  {
    int svid = reader.ensureReadVersionId("CompatBitmap", 2, "Try groovx0.8a4");

    if (isGL && svid <= 2)
      {
        bool dummy;
        reader.readValue("usingGlBitmap", dummy);
      }

    reader.readBaseClass("Bitmap", IO::makeProxy<GxPixmap>(this));
  }
};

extern "C"
int Gxnode_Init(Tcl_Interp* interp)
{
DOTRACE("Gxnode_Init");

  PKG_CREATE(interp, "GxNode", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<GxNode>(pkg);

  pkg->def( "contains", "item_id other_id", &GxTcl::contains );
  pkg->defVec("deepChildren", "item_id(s)", &GxNode::deepChildren);
  pkg->defVec("boundingBox", "item_id(s)", &GxTcl::boundingBox );
  pkg->def( "savePS", "item_id filename", &GxTcl::savePS );

  PKG_RETURN;
}

extern "C"
int Gxseparator_Init(Tcl_Interp* interp)
{
DOTRACE("Gxseparator_Init");

  PKG_CREATE(interp, "GxSeparator", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defGenericObjCmds<GxSeparator>(pkg);

  pkg->def( "fields", "", &GxTcl::gxsepFields );
  pkg->def( "allFields", "", &GxTcl::gxsepFields );

  pkg->def( "addChild", "item_id child_item_id", &GxSeparator::addChild );
  pkg->def( "addChildren", "item_id children_id(s)", &GxTcl::addChildren );
  pkg->defGetter("children", &GxSeparator::children);
  pkg->defAttrib("debugMode",
                 &GxSeparator::getDebugMode,
                 &GxSeparator::setDebugMode);
  pkg->defGetter("numChildren", &GxSeparator::numChildren);
  pkg->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt);
  pkg->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild);
  Util::ObjFactory::theOne().registerCreatorFunc(&GxSeparator::make);

  PKG_RETURN;
}

extern "C"
int Gxcolor_Init(Tcl_Interp* interp)
{
DOTRACE("Gxcolor_Init");

  PKG_CREATE(interp, "GxColor", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxColor>(pkg);
  Tcl::defCreator<GxColor>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxdrawstyle_Init(Tcl_Interp* interp)
{
DOTRACE("Gxdrawstyle_Init");

  PKG_CREATE(interp, "GxDrawStyle", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxDrawStyle>(pkg);
  Tcl::defCreator<GxDrawStyle>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxline_Init(Tcl_Interp* interp)
{
DOTRACE("Gxline_Init");

  PKG_CREATE(interp, "GxLine", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxLine>(pkg);
  Tcl::defCreator<GxLine>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxcylinder_Init(Tcl_Interp* interp)
{
DOTRACE("Gxcylinder_Init");

  PKG_CREATE(interp, "GxCylinder", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxCylinder>(pkg);
  Tcl::defCreator<GxCylinder>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxsphere_Init(Tcl_Interp* interp)
{
DOTRACE("Gxsphere_Init");

  PKG_CREATE(interp, "GxSphere", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxSphere>(pkg);
  Tcl::defCreator<GxSphere>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxlighting_Init(Tcl_Interp* interp)
{
DOTRACE("Gxlighting_Init");

  PKG_CREATE(interp, "GxLighting", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxLighting>(pkg);
  Tcl::defCreator<GxLighting>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxmaterial_Init(Tcl_Interp* interp)
{
DOTRACE("Gxmaterial_Init");

  PKG_CREATE(interp, "GxMaterial", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxMaterial>(pkg);
  Tcl::defCreator<GxMaterial>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxpointset_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpointset_Init");

  PKG_CREATE(interp, "GxPointSet", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defFieldContainer<GxPointSet>(pkg);
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
  pkg->defAttrib("child", &GxScaler::child, &GxScaler::setChild);
  pkg->defAttrib("mode", &GxScaler::getMode, &GxScaler::setMode);
  pkg->defAttrib("width", &GxScaler::scaledWidth, &GxScaler::setWidth);
  pkg->defAttrib("height", &GxScaler::scaledHeight, &GxScaler::setHeight);
  pkg->defAttrib("maxDim", &GxScaler::scaledMaxDim, &GxScaler::setMaxDim);
  pkg->defAttrib("aspect", &GxScaler::aspectRatio, &GxScaler::setAspectRatio);
  pkg->defAttrib("widthFactor", &GxScaler::widthFactor, &GxScaler::setWidthFactor);
  pkg->defAttrib("heightFactor", &GxScaler::heightFactor, &GxScaler::setHeightFactor);

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
  Tcl::defCreator<GxTransform>(pkg, "Position");
  Tcl::defFieldContainer<GxTransform>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxshapekit_Init(Tcl_Interp* interp)
{
DOTRACE("Gxshapekit_Init");

  PKG_CREATE(interp, "GxShapeKit", "$Revision$");
  pkg->inheritPkg("GxNode");

  Tcl::defTracing(pkg, GxShapeKit::tracer);

  Tcl::defFieldContainer<GxShapeKit>(pkg);

#if 0
  pkg->defVec( "saveBitmap", "item_id(s) filename(s)", &GxTcl::saveBitmap );
#endif

  pkg->defAttrib("category", &GxShapeKit::category, &GxShapeKit::setCategory);

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
  void scramble1(Util::SoftRef<GxPixmap> pixmap,
                 int numsubcols, int numsubrows)
  {
    pixmap->scramble(numsubcols, numsubrows, 0);
  }

  void scramble2(Util::SoftRef<GxPixmap> pixmap,
                 int numsubcols, int numsubrows, int seed)
  {
    pixmap->scramble(numsubcols, numsubrows, seed);
  }
}

extern "C"
int Gxpixmap_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpixmap_Init");

  PKG_CREATE(interp, "GxPixmap", "$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defCreator<GxPixmap>(pkg, "Bitmap");
  Tcl::defGenericObjCmds<GxPixmap>(pkg);


  pkg->defAttrib("asBitmap", &GxPixmap::getAsBitmap, &GxPixmap::setAsBitmap);
  pkg->defGetter("checkSum", &GxPixmap::checkSum);
  pkg->defGetter("filename", &GxPixmap::filename );
  pkg->defAction("flipContrast", &GxPixmap::flipContrast);
  pkg->defAction("flipVertical", &GxPixmap::flipVertical);
  pkg->defVec("grabScreenRect", "item_id(s) {left top right bottom}",
              &GxPixmap::grabScreenRect );
  pkg->defVec("grabWorldRect", "item_id(s) {left top right bottom}",
              &GxPixmap::grabWorldRect );
  pkg->defVec("loadImage", "item_id(s) filename(s)", &GxPixmap::loadImage );
  pkg->defAttrib("purgeable", &GxPixmap::isPurgeable, &GxPixmap::setPurgeable);
  pkg->defVec("queueImage", "item_id(s) filename(s)", &GxPixmap::queueImage );
  pkg->defAction("reload", &GxPixmap::reload);
  pkg->defVec("saveImage", "item_id(s) filename(s)", &GxPixmap::saveImage );
  pkg->defVec("scramble", "numcols numrows", &scramble1);
  pkg->defVec("scramble", "numcols numrows ?seed?", &scramble2);
  pkg->defVec("scramble", "numcols numrows ?seed moveParts flipLR flipTB?",
              &GxPixmap::scramble);
  pkg->defGetter("size", &GxPixmap::size);
  pkg->defAttrib("usingZoom", &GxPixmap::getUsingZoom, &GxPixmap::setUsingZoom);
  pkg->defAttrib("zoom", &GxPixmap::getZoom, &GxPixmap::setZoom);
  pkg->defSetter("zoomTo", &GxPixmap::zoomTo);

  // For GLBitmap/XBitmap backward-compatibility
  Util::ObjFactory::theOne().registerCreatorFunc(&CompatBitmap::makeGL, "GLBitmap");
  Util::ObjFactory::theOne().registerCreatorFunc(&CompatBitmap::makeX, "XBitmap");

  PKG_RETURN;
}

extern "C"
int Gxtext_Init(Tcl_Interp* interp)
{
DOTRACE("Gxtext_Init");

  PKG_CREATE(interp, "GxText", "$Revision$");
  pkg->inheritPkg("GxShapeKit");
  Tcl::defCreator<GxText>(pkg, "Gtext");
  Tcl::defFieldContainer<GxText>(pkg);

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
  Tcl::defFieldContainer<GxFixedScaleCamera>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxpsyphycamera_Init(Tcl_Interp* interp)
{
DOTRACE("Gxpsyphycamera_Init");

  PKG_CREATE(interp, "GxPsyphyCamera", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxPsyphyCamera>(pkg);
  Tcl::defFieldContainer<GxPsyphyCamera>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxperspectivecamera_Init(Tcl_Interp* interp)
{
DOTRACE("Gxperspectivecamera_Init");

  PKG_CREATE(interp, "GxPerspectiveCamera", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxPerspectiveCamera>(pkg);
  Tcl::defFieldContainer<GxPerspectiveCamera>(pkg);

  PKG_RETURN;
}

extern "C"
int Gxdisk_Init(Tcl_Interp* interp)
{
DOTRACE("Gxdisk_Init");

  PKG_CREATE(interp, "GxDisk", "$Revision$");
  pkg->inheritPkg("GxNode");
  Tcl::defCreator<GxDisk>(pkg);
  Tcl::defFieldContainer<GxDisk>(pkg);

  PKG_RETURN;
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
