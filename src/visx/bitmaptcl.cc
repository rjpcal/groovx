///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Wed Mar 15 10:20:36 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"
#include "iofactory.h"
#include "objlist.h"
#include "listitempkg.h"
#include "util/strings.h"
#include "system.h"
#include "tclcmd.h"
#include "tclobjlock.h"

#include <tcl.h>

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
  LoadPbmGzCmd(Tcl::TclItemPkg* pkg, const char* cmd_name, int method=0) :
	 Tcl::TclItemCmd<Bitmap>(pkg, cmd_name,
							  "bitmap_id filename(without .gz extension)"
							  "?temp_filename=.temp.pbm?",
							  3, 4),
	 itsMethod(method)
	 {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 const char* tempfilename =
		(objc() >= 4) ? getCstringFromArg(3) : ".temp.pbm";
  
	 dynamic_string gzname(filename);
	 gzname += ".gz";

	 if (itsMethod == 0) {
		bm->loadPbmGzFile(gzname.c_str());
	 }
	 else {
		fixed_string gzname_copy(gzname.c_str());

		invokeUsingTempFile(bm, gzname_copy.data(), tempfilename);
	 }
  }

private:
  void invokeUsingTempFile(Bitmap* bm,
									const char* gzname, const char* tempfilename);
  
  int itsMethod;
};

void BitmapTcl::LoadPbmGzCmd::invokeUsingTempFile(
  Bitmap* bm, const char* gzname, const char* tempfilename
) {
DOTRACE("BitmapTcl::LoadPbmGzCmd::invokeUsingTempFile");
  // Form a command to gunzip the file into a temp file
  const char* argv[] = { "gunzip", "-c", gzname, ">", tempfilename, 0 };
  int argc = 5;

  // Open a channel to execute the command
  Tcl_Channel chan = Tcl_OpenCommandChannel(interp(), argc, 
														  const_cast<char**>(argv),
														  0 /* no flags */);
  if (chan == 0) { throw Tcl::TclError("error opening command channel"); }

  int result = Tcl_Close(interp(), chan);
  if (result != TCL_OK) { throw Tcl::TclError("error closing command channel"); }
  
  // Read the temp file
  bm->loadPbmFile(tempfilename);

  // Remove the temp file
  result = System::theSystem().remove(tempfilename);
  if (result != 0) { throw Tcl::TclError("error removing temp file"); }
}

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

	 // write the file
	 bm->writePbmFile(filename);

	 // gzip the file
	 dynamic_string cmd = "exec gzip ";
	 cmd += filename;

	 fixed_string cmd_copy(cmd.c_str());
	 int result = Tcl_Eval(interp(), cmd_copy.data());

	 if (result != TCL_OK) {
		throw Tcl::TclError("error gzip-ing file");
	 }
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

class BitmapTcl::BitmapPkg : public Tcl::AbstractListItemPkg<Bitmap, ObjList> {
public:
  BitmapPkg(Tcl_Interp* interp) :
	 Tcl::AbstractListItemPkg<Bitmap, ObjList>(interp, ObjList::theObjList(),
															 "Bitmap", "1.1")
  {
	 addCommand( new LoadPbmCmd(this, "Bitmap::loadPbm") );
	 addCommand( new LoadPbmGzCmd(this, "Bitmap::loadPbmGz", 0) );
	 addCommand( new LoadPbmGzCmd(this, "Bitmap::loadPbmGzAlt", 1) );
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

class GLBitmapTcl::GLBitmapPkg : public Tcl::ListItemPkg<GLBitmap, ObjList> {
public:
  GLBitmapPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<GLBitmap, ObjList>(interp, ObjList::theObjList(),
													 "GLBitmap", "1.1")
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

class XBitmapTcl::XBitmapPkg : public Tcl::ListItemPkg<XBitmap, ObjList> {
public:
  XBitmapPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<XBitmap, ObjList>(interp, ObjList::theObjList(),
													"XBitmap", "$Revision$")
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

  FactoryRegistrar<IO, GLBitmap> :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, XBitmap>  :: registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_bitmaptcl_cc[] = "$Header$";
#endif // !BITMAPTCL_CC_DEFINED
