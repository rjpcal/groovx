///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Wed Jul 18 12:15:51 2001
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

#include "tcl/tclpkg.h"

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

class BitmapTcl::BitmapPkg : public Tcl::Pkg {
public:
  BitmapPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Bitmap", "$Revision$")
  {
    Tcl::defGenericObjCmds<Bitmap>(this);

    defVec( "loadPbm", "item_id(s) filename(s)", &Bitmap::loadPbmFile );
    defVec( "loadPbmGz", "item_id(s) filename(s)(no-gz-suffix)",
            &BitmapTcl::loadPbmGz );
    defVec( "writePbm", "item_id(s) filename(s)", &Bitmap::writePbmFile );
    defVec( "writePbmGz", "item_id(s) filename(s)(no-gz-suffix)",
            &BitmapTcl::writePbmGz );
    defVec( "grabScreenRect", "item_id(s) left top right bottom",
            (void(Bitmap::*)(int,int,int,int)) &Bitmap::grabScreenRect );
    defVec( "grabWorldRect", "item_id(s) left top right bottom",
            (void(Bitmap::*)(double,double,double,double)) &Bitmap::grabWorldRect );
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

class GLBitmapTcl::GLBitmapPkg : public Tcl::Pkg {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "GLBitmap", "$Revision$")
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

class XBitmapTcl::XBitmapPkg : public Tcl::Pkg {
public:
  XBitmapPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "XBitmap", "$Revision$")
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
int Bitmap_Init(Tcl_Interp* interp)
{
DOTRACE("Bitmap_Init");

  Tcl::Pkg* pkg1 = new BitmapTcl::BitmapPkg(interp);
  Tcl::Pkg* pkg2 = new GLBitmapTcl::GLBitmapPkg(interp);
  Tcl::Pkg* pkg3 = new XBitmapTcl::XBitmapPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&GLBitmap::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&XBitmap::make);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
