///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Wed Nov 20 16:10:45 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "gfx/recttcl.h"

#include "visx/bitmap.h"
#include "visx/glbitmap.h"
#include "visx/pointtcl.h"
#include "visx/xbitmap.h"

#include "util/objfactory.h"

#include "tcl/tclpkg.h"

#include "util/trace.h"
#include "util/debug.h"

extern "C"
int Bitmap_Init(Tcl_Interp* interp)
{
DOTRACE("Bitmap_Init");

  // Bitmap
  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "Bitmap", "$Revision$");
  pkg1->inherit("GxShapeKit");
  Tcl::defGenericObjCmds<Bitmap>(pkg1);

  pkg1->defGetter( "filename", &Bitmap::filename );
  pkg1->defVec( "loadImage", "item_id(s) filename(s)", &Bitmap::loadImage );
  pkg1->defVec( "queueImage", "item_id(s) filename(s)", &Bitmap::queueImage );
  pkg1->defVec( "saveImage", "item_id(s) filename(s)", &Bitmap::saveImage );
  pkg1->defVec( "grabScreenRect", "item_id(s) {left top right bottom}",
                &Bitmap::grabScreenRect );
  pkg1->defVec( "grabWorldRect", "item_id(s) {left top right bottom}",
                &Bitmap::grabWorldRect );
  pkg1->defAction("flipContrast", &Bitmap::flipContrast);
  pkg1->defAction("flipVertical", &Bitmap::flipVertical);
  pkg1->defGetter("size", &Bitmap::size);
  pkg1->defAttrib("zoom", &Bitmap::getZoom, &Bitmap::setZoom);
  pkg1->defAttrib("purgeable", &Bitmap::isPurgeable, &Bitmap::setPurgeable);

  // GLBitmap
  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "GLBitmap", "$Revision$");
  pkg2->inherit("Bitmap");
  Tcl::defGenericObjCmds<GLBitmap>(pkg2);
  pkg2->defAttrib("usingGlBitmap",
                  &GLBitmap::getUsingGlBitmap, &GLBitmap::setUsingGlBitmap);

  Util::ObjFactory::theOne().registerCreatorFunc(&GLBitmap::make);

  // XBitmap
  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "XBitmap", "$Revision$");
  pkg3->inherit("Bitmap");
  Tcl::defGenericObjCmds<XBitmap>(pkg3);

  Util::ObjFactory::theOne().registerCreatorFunc(&XBitmap::make);


  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3);
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
