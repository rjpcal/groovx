///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Fri Jul 13 15:06:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"

#include "util/objfactory.h"
#include "util/strings.h"

#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace BitmapTcl
{
  void loadPbmGz(Util::Ref<Bitmap> bmap, const char* filename)
  {
    dynamic_string gzname(filename); gzname += ".gz";

    bmap->loadPbmFile(gzname.c_str());
  }

  void writePbmGz(Util::Ref<Bitmap> bmap, const char* filename)
  {
    dynamic_string gzname(filename); gzname += ".gz";

    bmap->writePbmFile(gzname.c_str());
  }

  class BitmapPkg;
}

//---------------------------------------------------------------------
//
// BitmapTcl::BitmapPkg class definition
//
//---------------------------------------------------------------------

class BitmapTcl::BitmapPkg : public Tcl::GenericObjPkg<Bitmap> {
public:
  BitmapPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<Bitmap>(interp, "Bitmap", "$Revision$")
  {
    Tcl::defVec(this, &Bitmap::loadPbmFile,
                "Bitmap::loadPbm", "item_id(s) filename(s)");
    Tcl::defVec(this, &BitmapTcl::loadPbmGz,
                "Bitmap::loadPbmGz", "item_id(s) filename(s)(no-gz-suffix)");
    Tcl::defVec(this, &Bitmap::writePbmFile,
                "Bitmap::writePbm", "item_id(s) filename(s)");
    Tcl::defVec(this, &BitmapTcl::writePbmGz,
                "Bitmap::writePbmGz", "item_id(s) filename(s)(no-gz-suffix)");
    Tcl::defVec(
      this,
      (void(Bitmap::*)(int,int,int,int)) &Bitmap::grabScreenRect,
      "Bitmap::grabScreenRect", "item_id(s) left top right bottom");
    Tcl::defVec(
      this,
      (void(Bitmap::*)(double,double,double,double)) &Bitmap::grabWorldRect,
      "Bitmap::grabWorldRect", "item_id(s) left top right bottom");
    declareCAction("flipContrast", &Bitmap::flipContrast);
    declareCAction("flipVertical", &Bitmap::flipVertical);
    declareCAction("center", &Bitmap::center);
    declareCGetter("width", &Bitmap::width);
    declareCGetter("height", &Bitmap::height);
    declareCAttrib("rasterX", &Bitmap::getRasterX, &Bitmap::setRasterX);
    declareCAttrib("rasterY", &Bitmap::getRasterY, &Bitmap::setRasterY);
    declareCAttrib("zoomX", &Bitmap::getZoomX, &Bitmap::setZoomX);
    declareCAttrib("zoomY", &Bitmap::getZoomY, &Bitmap::setZoomY);
  }
};

///////////////////////////////////////////////////////////////////////
//
// GLBitmapTcl --
//
///////////////////////////////////////////////////////////////////////

namespace GLBitmapTcl {
  class GLBitmapPkg;
}

class GLBitmapTcl::GLBitmapPkg : public Tcl::GenericObjPkg<GLBitmap> {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<GLBitmap>(interp, "GLBitmap", "$Revision$")
  {
    declareCAttrib("usingGlBitmap",
                   &GLBitmap::getUsingGlBitmap, &GLBitmap::setUsingGlBitmap);
  }
};

///////////////////////////////////////////////////////////////////////
//
// XBitmapTcl --
//
///////////////////////////////////////////////////////////////////////

namespace XBitmapTcl {
  class XBitmapPkg;
}

class XBitmapTcl::XBitmapPkg : public Tcl::GenericObjPkg<XBitmap> {
public:
  XBitmapPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<XBitmap>(interp, "XBitmap", "$Revision$")
    {}
};

//---------------------------------------------------------------------
//
// Bitmap_Init --
//
//---------------------------------------------------------------------

extern "C"
int Bitmap_Init(Tcl_Interp* interp) {
DOTRACE("Bitmap_Init");

  Tcl::TclPkg* pkg1 = new BitmapTcl::BitmapPkg(interp);
  Tcl::TclPkg* pkg2 = new GLBitmapTcl::GLBitmapPkg(interp);
  Tcl::TclPkg* pkg3 = new XBitmapTcl::XBitmapPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&GLBitmap::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&XBitmap::make);

  return pkg1->combineStatus(pkg2->combineStatus(pkg3->initStatus()));;
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
