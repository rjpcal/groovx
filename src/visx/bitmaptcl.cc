///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Mon Jul 16 11:37:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"

#include "util/objfactory.h"
#include "util/ref.h"
#include "util/strings.h"

#include "tcl/tclitempkg.h"
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

class BitmapTcl::BitmapPkg : public Tcl::TclItemPkg {
public:
  BitmapPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "Bitmap", "$Revision$")
  {
    Tcl::defGenericObjCmds<Bitmap>(this);

    defVec(&Bitmap::loadPbmFile, "loadPbm", "item_id(s) filename(s)");
    defVec(&BitmapTcl::loadPbmGz,
           "loadPbmGz", "item_id(s) filename(s)(no-gz-suffix)");
    defVec(&Bitmap::writePbmFile, "writePbm", "item_id(s) filename(s)");
    defVec(&BitmapTcl::writePbmGz,
           "writePbmGz", "item_id(s) filename(s)(no-gz-suffix)");
    defVec( (void(Bitmap::*)(int,int,int,int)) &Bitmap::grabScreenRect,
            "grabScreenRect", "item_id(s) left top right bottom" );
    defVec( (void(Bitmap::*)(double,double,double,double)) &Bitmap::grabWorldRect,
            "grabWorldRect", "item_id(s) left top right bottom" );
    defAction("flipContrast", &Bitmap::flipContrast);
    defAction("flipVertical", &Bitmap::flipVertical);
    defAction("center", &Bitmap::center);
    defGetter("width", &Bitmap::width);
    defGetter("height", &Bitmap::height);
    defAttrib("rasterX", &Bitmap::getRasterX, &Bitmap::setRasterX);
    defAttrib("rasterY", &Bitmap::getRasterY, &Bitmap::setRasterY);
    defAttrib("zoomX", &Bitmap::getZoomX, &Bitmap::setZoomX);
    defAttrib("zoomY", &Bitmap::getZoomY, &Bitmap::setZoomY);
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

class GLBitmapTcl::GLBitmapPkg : public Tcl::TclItemPkg {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "GLBitmap", "$Revision$")
  {
    Tcl::defGenericObjCmds<GLBitmap>(this);

    defAttrib("usingGlBitmap",
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

class XBitmapTcl::XBitmapPkg : public Tcl::TclItemPkg {
public:
  XBitmapPkg(Tcl_Interp* interp) :
    Tcl::TclItemPkg(interp, "XBitmap", "$Revision$")
  {
    Tcl::defGenericObjCmds<XBitmap>(this);
  }
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
