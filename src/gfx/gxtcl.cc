///////////////////////////////////////////////////////////////////////
//
// gxtcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov  2 14:39:14 2000
// written: Thu Nov 21 12:28:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXTCL_CC_DEFINED
#define GXTCL_CC_DEFINED

#include "gfx/gxaligner.h"
#include "gfx/gxcache.h"
#include "gfx/gxcolor.h"
#include "gfx/gxcylinder.h"
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
#include "gfx/gxtransform.h"
#include "gfx/pscanvas.h"
#include "gfx/recttcl.h"

#include "io/ioproxy.h"
#include "io/reader.h"

#include "tcl/fieldpkg.h"
#include "tcl/itertcl.h"
#include "tcl/tclpkg.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

// FIXME move this file to gfx/
#include "visx/pointtcl.h"

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
  void saveBitmap(Util::Ref<GxShapeKit> obj, const char* filename)
  {
    obj->saveBitmapCache(Gfx::Canvas::current(), filename);
  }
#endif
}

namespace
{
  const IO::VersionId XBITMAP_SVID = 2;
  const IO::VersionId GLBITMAP_SVID = 3;
}

class XBitmap : public GxPixmap
{
public:
  XBitmap() : GxPixmap() {}

  static XBitmap* make() { return new XBitmap; }

  virtual ~XBitmap() {}

  virtual fstring ioTypename() const { return fstring("GxPixmap"); }

  virtual IO::VersionId serialVersionId() const { return XBITMAP_SVID; }

  virtual void readFrom(IO::Reader* reader)
  {
    reader->ensureReadVersionId("XBitmap", 2, "Try grsh0.8a4");

    reader->readBaseClass("Bitmap", IO::makeProxy<GxPixmap>(this));
  }

  // no writeTo() override since we just want GxPixmap's writeTo()
};

class GLBitmap : public GxPixmap
{
public:
  GLBitmap() : GxPixmap() {}

  static GLBitmap* make() { return new GLBitmap; }

  virtual ~GLBitmap() {}

  virtual fstring ioTypename() const { return fstring("GxPixmap"); }

  virtual IO::VersionId serialVersionId() const { return GLBITMAP_SVID; }

  virtual void readFrom(IO::Reader* reader)
  {
    int svid = reader->ensureReadVersionId("GLBitmap", 2, "Try grsh0.8a4");

    if (svid <= 2)
      {
        bool val;
        reader->readValue("usingGlBitmap", val);
      }

    reader->readBaseClass("Bitmap", IO::makeProxy<GxPixmap>(this));
  }

  // no writeTo() override since we just want GxPixmap's writeTo()
};

extern "C"
int Gx_Init(Tcl_Interp* interp)
{
DOTRACE("Gx_Init");

  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "GxNode", "$Revision$");
  Tcl::defGenericObjCmds<GxNode>(pkg1);

  pkg1->def( "contains", "item_id other_id", &GxTcl::contains );
  pkg1->defVec("deepChildren", "item_id(s)", &GxNode::deepChildren);
  pkg1->defVec("boundingBox", "item_id(s)", &GxTcl::boundingBox );
  pkg1->def( "savePS", "item_id filename", &GxTcl::savePS );

  int status = pkg1->initStatus();

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "GxSeparator", "$Revision$");
  pkg2->inherit("GxNode");
  Tcl::defGenericObjCmds<GxSeparator>(pkg2);

  pkg2->def( "fields", "", &GxTcl::gxsepFields );
  pkg2->def( "allFields", "", &GxTcl::gxsepFields );

  pkg2->def( "addChild", "item_id child_item_id", &GxSeparator::addChild );
  pkg2->def( "addChildren", "item_id children_id(s)", &GxTcl::addChildren );
  pkg2->defGetter("children", &GxSeparator::children);
  pkg2->defAttrib("debugMode",
                  &GxSeparator::getDebugMode,
                  &GxSeparator::setDebugMode);
  pkg2->defGetter("numChildren", &GxSeparator::numChildren);
  pkg2->def("removeChildAt", "sep_id(s) child_indices", &GxSeparator::removeChildAt);
  pkg2->def("removeChild","sep_id(s) child_id(s)", &GxSeparator::removeChild);
  Util::ObjFactory::theOne().registerCreatorFunc(&GxSeparator::make);

  status = pkg2->combineStatus(status);

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "GxColor", "$Revision$");
  pkg3->inherit("GxNode");
  Tcl::defFieldContainer<GxColor>(pkg3);
  Tcl::defCreator<GxColor>(pkg3);

  status = pkg3->combineStatus(status);

  Tcl::Pkg* pkg4 = new Tcl::Pkg(interp, "GxDrawStyle", "$Revision$");
  pkg4->inherit("GxNode");
  Tcl::defFieldContainer<GxDrawStyle>(pkg4);
  Tcl::defCreator<GxDrawStyle>(pkg4);

  status = pkg4->combineStatus(status);

  Tcl::Pkg* pkg5 = new Tcl::Pkg(interp, "GxLine", "$Revision$");
  pkg5->inherit("GxNode");
  Tcl::defFieldContainer<GxLine>(pkg5);
  Tcl::defCreator<GxLine>(pkg5);

  status = pkg5->combineStatus(status);

  Tcl::Pkg* pkg6 = new Tcl::Pkg(interp, "GxCylinder", "$Revision$");
  pkg6->inherit("GxNode");
  Tcl::defFieldContainer<GxCylinder>(pkg6);
  Tcl::defCreator<GxCylinder>(pkg6);

  status = pkg6->combineStatus(status);

  Tcl::Pkg* pkg7 = new Tcl::Pkg(interp, "GxSphere", "$Revision$");
  pkg7->inherit("GxNode");
  Tcl::defFieldContainer<GxSphere>(pkg7);
  Tcl::defCreator<GxSphere>(pkg7);

  status = pkg7->combineStatus(status);

  Tcl::Pkg* pkg8 = new Tcl::Pkg(interp, "GxLighting", "$Revision$");
  pkg8->inherit("GxNode");
  Tcl::defFieldContainer<GxLighting>(pkg8);
  Tcl::defCreator<GxLighting>(pkg8);

  status = pkg8->combineStatus(status);

  Tcl::Pkg* pkg9 = new Tcl::Pkg(interp, "GxMaterial", "$Revision$");
  pkg9->inherit("GxNode");
  Tcl::defFieldContainer<GxMaterial>(pkg9);
  Tcl::defCreator<GxMaterial>(pkg9);

  status = pkg9->combineStatus(status);

  Tcl::Pkg* pkg10 = new Tcl::Pkg(interp, "GxPointSet", "$Revision$");
  pkg10->inherit("GxNode");
  Tcl::defFieldContainer<GxPointSet>(pkg10);
  Tcl::defCreator<GxPointSet>(pkg10);

  status = pkg10->combineStatus(status);

  Tcl::Pkg* pkg11 = new Tcl::Pkg(interp, "GxScaler", "$Revision$");
  pkg11->inherit("GxNode");
  Tcl::defCreator<GxScaler>(pkg11);
  pkg11->defAttrib("child", &GxScaler::child, &GxScaler::setChild);
  pkg11->defAttrib("mode", &GxScaler::getMode, &GxScaler::setMode);
  pkg11->defAttrib("width", &GxScaler::scaledWidth, &GxScaler::setWidth);
  pkg11->defAttrib("height", &GxScaler::scaledHeight, &GxScaler::setHeight);
  pkg11->defAttrib("maxDim", &GxScaler::scaledMaxDim, &GxScaler::setMaxDim);
  pkg11->defAttrib("aspect", &GxScaler::aspectRatio, &GxScaler::setAspectRatio);
  pkg11->defAttrib("widthFactor", &GxScaler::widthFactor, &GxScaler::setWidthFactor);
  pkg11->defAttrib("heightFactor", &GxScaler::heightFactor, &GxScaler::setHeightFactor);

  status = pkg11->combineStatus(status);

  Tcl::Pkg* pkg12 = new Tcl::Pkg(interp, "GxEmptyNode", "$Revision$");
  pkg12->inherit("GxNode");
  Tcl::defCreator<GxEmptyNode>(pkg12);

  status = pkg12->combineStatus(status);

  Tcl::Pkg* pkg13 = new Tcl::Pkg(interp, "GxTransform", "$Revision$");
  Tcl::defCreator<GxTransform>(pkg13, "Position");
  Tcl::defFieldContainer<GxTransform>(pkg13);

  status = pkg13->combineStatus(status);

  Tcl::Pkg* pkg14 = new Tcl::Pkg(interp, "GxShapeKit", "$Revision$");
  pkg14->inherit("GxNode");

  Tcl::defTracing(pkg14, GxShapeKit::tracer);

  Tcl::defFieldContainer<GxShapeKit>(pkg14);

#if 0
  pkg14->defVec( "saveBitmap", "item_id(s) filename(s)", &GxTcl::saveBitmap );
#endif

  pkg14->defAttrib("category", &GxShapeKit::category, &GxShapeKit::setCategory);

  pkg14->linkVarCopy("GxShapeKit::DIRECT", GxCache::DIRECT);
  pkg14->linkVarCopy("GxShapeKit::GLCOMPILE", GxCache::GLCOMPILE);

  pkg14->linkVarCopy("GxShapeKit::NATIVE_SCALING", GxScaler::NATIVE_SCALING);
  pkg14->linkVarCopy("GxShapeKit::MAINTAIN_ASPECT_SCALING", GxScaler::MAINTAIN_ASPECT_SCALING);
  pkg14->linkVarCopy("GxShapeKit::FREE_SCALING", GxScaler::FREE_SCALING);

  pkg14->linkVarCopy("GxShapeKit::NATIVE_ALIGNMENT", GxAligner::NATIVE_ALIGNMENT);
  pkg14->linkVarCopy("GxShapeKit::CENTER_ON_CENTER", GxAligner::CENTER_ON_CENTER);
  pkg14->linkVarCopy("GxShapeKit::NW_ON_CENTER", GxAligner::NW_ON_CENTER);
  pkg14->linkVarCopy("GxShapeKit::NE_ON_CENTER", GxAligner::NE_ON_CENTER);
  pkg14->linkVarCopy("GxShapeKit::SW_ON_CENTER", GxAligner::SW_ON_CENTER);
  pkg14->linkVarCopy("GxShapeKit::SE_ON_CENTER", GxAligner::SE_ON_CENTER);
  pkg14->linkVarCopy("GxShapeKit::ARBITRARY_ON_CENTER", GxAligner::ARBITRARY_ON_CENTER);

  status = pkg14->combineStatus(status);

  // GxPixmap
  Tcl::Pkg* pkg15 = new Tcl::Pkg(interp, "GxPixmap", "$Revision$");
  pkg15->inherit("GxShapeKit");
  Tcl::defCreator<GxPixmap>(pkg15, "Bitmap");
  Tcl::defGenericObjCmds<GxPixmap>(pkg15);

  pkg15->defGetter( "filename", &GxPixmap::filename );
  pkg15->defVec( "loadImage", "item_id(s) filename(s)", &GxPixmap::loadImage );
  pkg15->defVec( "queueImage", "item_id(s) filename(s)", &GxPixmap::queueImage );
  pkg15->defVec( "saveImage", "item_id(s) filename(s)", &GxPixmap::saveImage );
  pkg15->defVec( "grabScreenRect", "item_id(s) {left top right bottom}",
                 &GxPixmap::grabScreenRect );
  pkg15->defVec( "grabWorldRect", "item_id(s) {left top right bottom}",
                 &GxPixmap::grabWorldRect );
  pkg15->defAction("flipContrast", &GxPixmap::flipContrast);
  pkg15->defAction("flipVertical", &GxPixmap::flipVertical);
  pkg15->defGetter("size", &GxPixmap::size);
  pkg15->defAttrib("zoom", &GxPixmap::getZoom, &GxPixmap::setZoom);
  pkg15->defAttrib("usingZoom", &GxPixmap::getUsingZoom, &GxPixmap::setUsingZoom);
  pkg15->defAttrib("purgeable", &GxPixmap::isPurgeable, &GxPixmap::setPurgeable);
  pkg15->defAttrib("asBitmap", &GxPixmap::getAsBitmap, &GxPixmap::setAsBitmap);

  // GLBitmap for backward-compatibility
  Tcl::defCreator<GLBitmap>(pkg15);

  // XBitmap for backward-compatability
  Tcl::defCreator<XBitmap>(pkg15);

  status = pkg15->combineStatus(status);

  return status;
}

static const char vcid_gxtcl_cc[] = "$Header$";
#endif // !GXTCL_CC_DEFINED
