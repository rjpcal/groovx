///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Thu Jun 17 18:06:17 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include <tcl.h>

#include "bitmap.h"
#include "objlist.h"
#include "tclitempkg.h"
#include "tclcmd.h"
#include "id.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

Bitmap* getBitmapFromId(int id) {
  if ( !ObjList::theObjList().isValidObjid(id) ) {
	 throw TclError("invalid object id");
  }
  GrObj* g = ObjList::theObjList().getObj(id);
  if ( g == NULL ) {
	 throw TclError("invalid object id");
  }
  Bitmap* bm = dynamic_cast<Bitmap*>(g);
  if ( bm == NULL ) {
	 throw TclError("object not of correct type");
  }
  return bm;
}

class BitmapCmd : public TclCmd {
public:
  BitmapCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 Bitmap* bm = new Bitmap;
	 ObjId objid = ObjList::theObjList().addObj(bm);
	 returnInt(objid.toInt());
  }
};

class LoadPbmCmd : public TclCmd {
public:
  LoadPbmCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "bitmap_id filename", 3, 3) {}
protected:
  virtual void invoke() {
	 int id = getIntFromArg(1);
	 Bitmap* bm = getBitmapFromId(id);
	 const char* filename = getCstringFromArg(2);
	 bm->loadPbmFile(filename);
  }
};

class FlipContrastCmd : public TclCmd {
public:
  FlipContrastCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "bitmap_id", 2, 2) {}
protected:
  virtual void invoke() {
	 int id = getIntFromArg(1);
	 Bitmap* bm = getBitmapFromId(id);
	 bm->flipContrast();
  }
};

class BitmapPkg : public TclItemPkg {
public:
  BitmapPkg(Tcl_Interp* interp) :
	 TclItemPkg(interp, "Bitmap", "1.1")
  {
	 addCommand( new BitmapCmd(interp, "Bitmap::bitmap") );
	 addCommand( new LoadPbmCmd(interp, "Bitmap::loadPbm") );
	 addCommand( new FlipContrastCmd(interp, "Bitmap::flipContrast") );
	 declareDoubleAttrib("rasterX", 
								new CAttrib<Bitmap, double>(&Bitmap::getRasterX,
																	 &Bitmap::setRasterX));
	 declareDoubleAttrib("rasterY",
								new CAttrib<Bitmap, double>(&Bitmap::getRasterY,
																	 &Bitmap::setRasterY));
	 declareDoubleAttrib("zoomX", 
								new CAttrib<Bitmap, double>(&Bitmap::getZoomX,
																	 &Bitmap::setZoomX));
	 declareDoubleAttrib("zoomY",
								new CAttrib<Bitmap, double>(&Bitmap::getZoomY,
																	 &Bitmap::setZoomY));
  }

  virtual void* getItemFromId(int id) {
	 return static_cast<void*>(getBitmapFromId(id));
  }
};

namespace BitmapTcl {
  Tcl_PackageInitProc Bitmap_Init;
}

int BitmapTcl::Bitmap_Init(Tcl_Interp* interp) {
DOTRACE("BitmapTcl::Bitmap_Init");

  new BitmapPkg(interp);

  return TCL_OK;
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
