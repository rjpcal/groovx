///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Mon Oct 30 11:12:30 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"

#include "io/iofactory.h"

#include "util/strings.h"

#include "tcl/listitempkg.h"
#include "tcl/tclcmd.h"
#include "tcl/tclobjlock.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace BitmapTcl {
  class LoadPbmCmd;
  class LoadPbmGzCmd;
  class WritePbmCmd;
  class WritePbmGzCmd;
  class GrabScreenRectCmd;
  class GrabWorldRectCmd;
  class BitmapPkg;
}

//---------------------------------------------------------------------
//
// BitmapTcl::LoadPbmCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::LoadPbmCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  LoadPbmCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id filename", 3, 3) {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 bm->loadPbmFile(filename);
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::LoadPbmGzCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::LoadPbmGzCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  LoadPbmGzCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name,
							  "bitmap_id filename(without .gz extension)",
							  3, 3)
	 {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);

	 dynamic_string gzname(filename); gzname += ".gz";

	 bm->loadPbmFile(gzname.c_str());
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::WritePbmCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::WritePbmCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  WritePbmCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id filename", 3, 3) {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 bm->writePbmFile(filename);
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::WritePbmGzCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::WritePbmGzCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  WritePbmGzCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name,
							  "bitmap_id filename(without .gz extension)", 3, 3)
	 {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);

	 dynamic_string gzname(filename); gzname += ".gz";

	 bm->writePbmFile(gzname.c_str());
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::GrabScreenRectCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::GrabScreenRectCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  GrabScreenRectCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id left top right bottom", 6, 6) {}
protected:
  virtual void invoke() {
	 int l = getIntFromArg(2);
	 int t = getIntFromArg(3);
	 int r = getIntFromArg(4);
	 int b = getIntFromArg(5);

	 Bitmap* bm = getItem();
	 bm->grabScreenRect(l, t, r, b);
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::GrabWorldRectCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::GrabWorldRectCmd : public Tcl::TclItemCmd<Bitmap> {
public:
  GrabWorldRectCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id left top right bottom", 6, 6) {}
protected:
  virtual void invoke() {
	 double l = getDoubleFromArg(2);
	 double t = getDoubleFromArg(3);
	 double r = getDoubleFromArg(4);
	 double b = getDoubleFromArg(5);

	 Bitmap* bm = getItem();
	 bm->grabWorldRect(l, t, r, b);
  }
};

//---------------------------------------------------------------------
//
// BitmapTcl::BitmapPkg class definition
//
//---------------------------------------------------------------------

class BitmapTcl::BitmapPkg : public Tcl::ItemPkg<Bitmap> {
public:
  BitmapPkg(Tcl_Interp* interp) :
	 Tcl::ItemPkg<Bitmap>(interp, "Bitmap", "$Revision$")
  {
	 addCommand( new LoadPbmCmd(this, "Bitmap::loadPbm") );
	 addCommand( new LoadPbmGzCmd(this, "Bitmap::loadPbmGz") );
	 addCommand( new WritePbmCmd(this, "Bitmap::writePbm") );
	 addCommand( new WritePbmGzCmd(this, "Bitmap::writePbmGz") );
	 addCommand( new GrabScreenRectCmd(this, "Bitmap::grabScreenRect") );
	 addCommand( new GrabWorldRectCmd(this, "Bitmap::grabWorldRect") );
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

class GLBitmapTcl::GLBitmapPkg : public Tcl::ItemPkg<GLBitmap> {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
	 Tcl::ItemPkg<GLBitmap>(interp, "GLBitmap", "$Revision$")
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

class XBitmapTcl::XBitmapPkg : public Tcl::ItemPkg<XBitmap> {
public:
  XBitmapPkg(Tcl_Interp* interp) :
	 Tcl::ItemPkg<XBitmap>(interp, "XBitmap", "$Revision$")
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

  new BitmapTcl::BitmapPkg(interp);
  new GLBitmapTcl::GLBitmapPkg(interp);
  new XBitmapTcl::XBitmapPkg(interp);

  IO::IoFactory::theOne().registerCreatorFunc(&GLBitmap::make);
  IO::IoFactory::theOne().registerCreatorFunc(&XBitmap::make);

  return TCL_OK;
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
