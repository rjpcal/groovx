///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Tue Nov 23 15:45:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include <tcl.h>
#include <string>
#include <cstring>

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
  class LoadPbmGzCmd;
  class WritePbmCmd;
  class WritePbmGzCmd;
  class GrabScreenRectCmd;
  class GrabWorldRectCmd;
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
// BitmapTcl::LoadPbmGzCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::LoadPbmGzCmd : public TclItemCmd<Bitmap> {
public:
  LoadPbmGzCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name,
							  "bitmap_id filename(without .gz extension)"
							  "?temp_filename=.temp.pbm?",
							  3, 4),
	 itsInterp(pkg->interp()) {}
protected:
  virtual void invoke();

private:
  Tcl_Interp* itsInterp;
};

void BitmapTcl::LoadPbmGzCmd::invoke() {
DOTRACE("BitmapTcl::LoadPbmGzCmd::invoke");
  Bitmap* bm = getItem();
  const char* filename = getCstringFromArg(2);
  const char* tempfilename =
	 (objc() >= 4) ? getCstringFromArg(3) : ".temp.pbm";
  
  // zcat the file to a temp file
  string cmd = "exec gunzip -c ";
  cmd += filename;
  cmd += ".gz > ";
  cmd += tempfilename;
  vector<char> cmd_cstr(cmd.length()+1);
  strcpy(&cmd_cstr[0], cmd.c_str());
  int result = Tcl_Eval(itsInterp, &cmd_cstr[0]);
  
  if (result != TCL_OK) {
	 throw TclError("error gunzip-ing file");
  }
  
  // load the file
  bm->loadPbmFile(tempfilename);

  // remove the temp file
  cmd = "exec rm ";
  cmd += tempfilename;
  cmd_cstr.resize(cmd.length()+1);
  strcpy(&cmd_cstr[0], cmd.c_str());
  result = Tcl_Eval(itsInterp, &cmd_cstr[0]);
  
  if (result != TCL_OK) {
	 throw TclError("error removing temp file");
  }
  
}

//---------------------------------------------------------------------
//
// BitmapTcl::WritePbmCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::WritePbmCmd : public TclItemCmd<Bitmap> {
public:
  WritePbmCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id filename", 3, 3) {}
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

class BitmapTcl::WritePbmGzCmd : public TclItemCmd<Bitmap> {
public:
  WritePbmGzCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name,
							  "bitmap_id filename(without .gz extension)", 3, 3),
	 itsInterp(pkg->interp()) {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);

	 // write the file
	 bm->writePbmFile(filename);

	 // gzip the file
	 string cmd = "exec gzip ";
	 cmd += filename;
	 vector<char> cmd_cstr(cmd.length()+1);
	 strcpy(&cmd_cstr[0], cmd.c_str());
	 int result = Tcl_Eval(itsInterp, &cmd_cstr[0]);

	 if (result != TCL_OK) {
		throw TclError("error gzip-ing file");
	 }
  }

private:
  Tcl_Interp* itsInterp;
};

//---------------------------------------------------------------------
//
// BitmapTcl::GrabScreenRectCmd --
//
//---------------------------------------------------------------------

class BitmapTcl::GrabScreenRectCmd : public TclItemCmd<Bitmap> {
public:
  GrabScreenRectCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id left top right bottom", 6, 6) {}
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

class BitmapTcl::GrabWorldRectCmd : public TclItemCmd<Bitmap> {
public:
  GrabWorldRectCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Bitmap>(pkg, cmd_name, "bitmap_id left top right bottom", 6, 6) {}
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

class BitmapTcl::BitmapPkg : public AbstractListItemPkg<Bitmap, ObjList> {
public:
  BitmapPkg(Tcl_Interp* interp) :
	 AbstractListItemPkg<Bitmap, ObjList>(interp, ObjList::theObjList(),
													  "Bitmap", "1.1")
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
