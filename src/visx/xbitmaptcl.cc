///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:46:28 1999
// written: Tue Nov 30 19:09:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAPTCL_CC_DEFINED
#define XBITMAPTCL_CC_DEFINED

#include <tcl.h>
#include "xbitmap.h"
#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace XBitmapTcl {
  class LoadPbmCmd;
  class XBitmapPkg;
}

//---------------------------------------------------------------------
//
// XBitmapTcl::LoadPbmCmd --
//
//---------------------------------------------------------------------

class XBitmapTcl::LoadPbmCmd : public TclItemCmd<XBitmap> {
public:
  LoadPbmCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<XBitmap>(pkg, cmd_name, "bitmap_id filename", 3, 3) {}
protected:
  virtual void invoke() {
	 XBitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 bm->loadPbmFile(filename);
  }
};

//---------------------------------------------------------------------
//
// XBitmapTcl::XBitmapPkg class definition
//
//---------------------------------------------------------------------

class XBitmapTcl::XBitmapPkg : public ListItemPkg<XBitmap, ObjList> {
public:
  XBitmapPkg(Tcl_Interp* interp) :
	 ListItemPkg<XBitmap, ObjList>(interp, ObjList::theObjList(),
											 "XBitmap", "1.1")
  {
	 addCommand( new LoadPbmCmd(this, "XBitmap::loadPbm") );
	 declareCAction("flipContrast", &XBitmap::flipContrast);
	 declareCAction("flipVertical", &XBitmap::flipVertical);
	 declareCAction("center", &XBitmap::center);
	 declareCGetter("width", &XBitmap::width);
	 declareCGetter("height", &XBitmap::height);
	 declareCAttrib("rasterX", &XBitmap::getRasterX, &XBitmap::setRasterX);
	 declareCAttrib("rasterY", &XBitmap::getRasterY, &XBitmap::setRasterY);
  }
};

//---------------------------------------------------------------------
//
// Xbitmap_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Xbitmap_Init;

int Xbitmap_Init(Tcl_Interp* interp) {
DOTRACE("Xbitmap_Init");

  new XBitmapTcl::XBitmapPkg(interp);

  FactoryRegistrar<IO, XBitmap>::registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_xbitmaptcl_cc[] = "$Header$";
#endif // !XBITMAPTCL_CC_DEFINED
