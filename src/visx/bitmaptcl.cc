///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:43:45 1999
// written: Wed Nov 20 17:10:48 2002
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
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Bitmap", "$Revision$");
  pkg->inherit("GxShapeKit");
  Tcl::defCreator<Bitmap>(pkg);
  Tcl::defGenericObjCmds<Bitmap>(pkg);

  pkg->defGetter( "filename", &Bitmap::filename );
  pkg->defVec( "loadImage", "item_id(s) filename(s)", &Bitmap::loadImage );
  pkg->defVec( "queueImage", "item_id(s) filename(s)", &Bitmap::queueImage );
  pkg->defVec( "saveImage", "item_id(s) filename(s)", &Bitmap::saveImage );
  pkg->defVec( "grabScreenRect", "item_id(s) {left top right bottom}",
               &Bitmap::grabScreenRect );
  pkg->defVec( "grabWorldRect", "item_id(s) {left top right bottom}",
               &Bitmap::grabWorldRect );
  pkg->defAction("flipContrast", &Bitmap::flipContrast);
  pkg->defAction("flipVertical", &Bitmap::flipVertical);
  pkg->defGetter("size", &Bitmap::size);
  pkg->defAttrib("zoom", &Bitmap::getZoom, &Bitmap::setZoom);
  pkg->defAttrib("usingZoom", &Bitmap::getUsingZoom, &Bitmap::setUsingZoom);
  pkg->defAttrib("purgeable", &Bitmap::isPurgeable, &Bitmap::setPurgeable);
  pkg->defAttrib("asBitmap", &Bitmap::getAsBitmap, &Bitmap::setAsBitmap);

  // GLBitmap
  Tcl::defCreator<GLBitmap>(pkg);

  // XBitmap
  Tcl::defCreator<XBitmap>(pkg);

  return pkg->initStatus();
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
