///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Wed Sep  8 13:19:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include <tcl.h>

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"
#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace BitmapTcl {
  class LoadPbmCmd;
  class BitmapPkg;
}

namespace GLBitmapTcl {
  class GLBitmapPkg;
}

namespace XBitmapTcl {
  class XBitmapPkg;
}

//---------------------------------------------------------------------
//
// BitmapTcl::LoadPbmCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::LoadPbmCmd : public TclItemCmd<Bitmap> {
public:
  LoadPbmCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id filename", 3, 3) {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 bm->loadPbmFile(filename);
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::BitmapPkg class definition
//
//---------------------------------------------------------------------

class BitmapTcl::BitmapPkg : public AbstractListItemPkg<Bitmap, ObjList> {
public:
  BitmapPkg(Tcl_Interp* interp) :
	 AbstractListItemPkg<Bitmap, ObjList>(interp, ObjList::theObjList(),
													  "Bitmap", "1.1")
  {
	 addCommand( new LoadPbmCmd(this, "Bitmap::loadPbm") );
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

class GLBitmapTcl::GLBitmapPkg : public ListItemPkg<GLBitmap, ObjList> {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
	 ListItemPkg<GLBitmap, ObjList>(interp, ObjList::theObjList(),
											  "GLBitmap", "1.1")
  {
	 declareCAttrib("usingGlBitmap",
						 &GLBitmap::getUsingGlBitmap, &GLBitmap::setUsingGlBitmap);
  }
};

class XBitmapTcl::XBitmapPkg : public ListItemPkg<XBitmap, ObjList> {
public:
  XBitmapPkg(Tcl_Interp* interp) :
	 ListItemPkg<XBitmap, ObjList>(interp, ObjList::theObjList(),
											  "XBitmap", "1.1")
  {}
};

//---------------------------------------------------------------------
//
// Bitmap_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Bitmap_Init;

int Bitmap_Init(Tcl_Interp* interp) {
DOTRACE("Bitmap_Init");

  new BitmapTcl::BitmapPkg(interp);
  new GLBitmapTcl::GLBitmapPkg(interp);
  new XBitmapTcl::XBitmapPkg(interp);

  FactoryRegistrar<IO, GLBitmap> registrar1(IoFactory::theOne());
  FactoryRegistrar<IO, XBitmap>  registrar2(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
