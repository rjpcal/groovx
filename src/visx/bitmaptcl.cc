///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Sun Aug  5 19:34:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"

#include "util/objfactory.h"

#include "tcl/tclpkg.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

extern "C"
int Bitmap_Init(Tcl_Interp* interp)
{
DOTRACE("Bitmap_Init");

  // Bitmap
  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "Bitmap", "$Revision$");

  Tcl::defGenericObjCmds<Bitmap>(pkg1);

  pkg1->defVec( "loadPbm", "item_id(s) filename(s)", &Bitmap::loadPbmFile );
  pkg1->defVec( "writePbm", "item_id(s) filename(s)", &Bitmap::writePbmFile );
  pkg1->defVec( "grabScreenRect", "item_id(s) left top right bottom",
                (void(Bitmap::*)(int,int,int,int)) &Bitmap::grabScreenRect );
  pkg1->defVec( "grabWorldRect", "item_id(s) left top right bottom",
                (void(Bitmap::*)(double,double,double,double)) &Bitmap::grabWorldRect );
  pkg1->defAction("flipContrast", &Bitmap::flipContrast);
  pkg1->defAction("flipVertical", &Bitmap::flipVertical);
  pkg1->defAction("center", &Bitmap::center);
  pkg1->defGetter("width", &Bitmap::width);
  pkg1->defGetter("height", &Bitmap::height);
  pkg1->defAttrib("rasterX", &Bitmap::getRasterX, &Bitmap::setRasterX);
  pkg1->defAttrib("rasterY", &Bitmap::getRasterY, &Bitmap::setRasterY);
  pkg1->defAttrib("zoomX", &Bitmap::getZoomX, &Bitmap::setZoomX);
  pkg1->defAttrib("zoomY", &Bitmap::getZoomY, &Bitmap::setZoomY);

  // GLBitmap
  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "GLBitmap", "$Revision$");

  Tcl::defGenericObjCmds<GLBitmap>(pkg2);
  pkg2->defAttrib("usingGlBitmap",
                  &GLBitmap::getUsingGlBitmap, &GLBitmap::setUsingGlBitmap);

  Util::ObjFactory::theOne().registerCreatorFunc(&GLBitmap::make);

  // XBitmap
  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "XBitmap", "$Revision$");

  Tcl::defGenericObjCmds<XBitmap>(pkg3);

  Util::ObjFactory::theOne().registerCreatorFunc(&XBitmap::make);


  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
