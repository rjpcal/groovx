///////////////////////////////////////////////////////////////////////
//
// bitmaptcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:43:45 1999
// written: Wed Dec 15 12:56:00 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPTCL_CC_DEFINED
#define BITMAPTCL_CC_DEFINED

#include <tcl.h>
#include <tk.h>
#include <string>
#include <cstring>
#include <strstream.h>

#include "bitmap.h"
#include "glbitmap.h"
#include "xbitmap.h"
#include "iofactory.h"
#include "objlist.h"
#include "listitempkg.h"
#include "system.h"
#include "tclcmd.h"
#include "tclobjlock.h"

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
	 itsInterp(pkg->interp()),
	 itsMethod(method)
	 {}
protected:
  virtual void invoke() {
	 Bitmap* bm = getItem();
	 const char* filename = getCstringFromArg(2);
	 const char* tempfilename =
		(objc() >= 4) ? getCstringFromArg(3) : ".temp.pbm";
  
	 string gzname = string(filename) + ".gz";
	 vector<char> gzname_cstr(gzname.begin(), gzname.end());
	 gzname_cstr.push_back('\0');

	 if (itsMethod == 0) {
		invokeUsingTempFile(bm, &gzname_cstr[0], tempfilename);
	 }
	 else {
		invokeUsingChannel(bm, &gzname_cstr[0]);
	 }
  }

private:
  void invokeUsingChannel(Bitmap* bm, const char* gzname);
  void invokeUsingTempFile(Bitmap* bm,
									const char* gzname, const char* tempfilename);
  
  Tcl_Interp* itsInterp;
  int itsMethod;
};

void BitmapTcl::LoadPbmGzCmd::invokeUsingChannel(
  Bitmap* bm, const char* gzname
) {
DOTRACE("BitmapTcl::LoadPbmGzCmd::invoke");
  // Form a command to gunzip the file
  const char* argv[] = { "gunzip", "-c", gzname, 0 };
  int argc = 3;

  // Open a channel to get the output of the command
  Tcl_Channel chan = Tcl_OpenCommandChannel(itsInterp, argc, 
														  const_cast<char**>(argv),
														  TCL_STDOUT);
  if (chan == 0) { throw Tcl::TclError("error opening command channel"); }

  // Read the output of the channel into a Tcl_Obj
  Tcl::TclObjPtr contents(Tcl_NewObj());

  while (1) {
	 int chars_read = Tcl_ReadChars(chan, contents, 4096, 1 /* -> do append */);
	 if ( chars_read == -1 ) { throw Tcl::TclError("error reading from channel"); }
	 if ( chars_read == 0 ) /* we are done, so... */ break;
  }

  int result = Tcl_Close(itsInterp, chan);
  if (result != TCL_OK) { throw Tcl::TclError("error closing command channel"); }
  
  // Form a stream from the bytes and load the bitmap from that stream
  int num_bytes;
  unsigned char* byte_array = Tcl_GetByteArrayFromObj(contents, &num_bytes);

  istrstream ist(reinterpret_cast<char*>(byte_array), num_bytes);

  bm->loadPbmFile(ist);
}

void BitmapTcl::LoadPbmGzCmd::invokeUsingTempFile(
  Bitmap* bm, const char* gzname, const char* tempfilename
) {
DOTRACE("BitmapTcl::LoadPbmGzCmd::invokeUsingTempFile");
  // Form a command to gunzip the file into a temp file
  const char* argv[] = { "gunzip", "-c", gzname, ">", tempfilename, 0 };
  int argc = 5;

  // Open a channel to execute the command
  Tcl_Channel chan = Tcl_OpenCommandChannel(itsInterp, argc, 
														  const_cast<char**>(argv),
														  0 /* no flags */);
  if (chan == 0) { throw Tcl::TclError("error opening command channel"); }

  int result = Tcl_Close(itsInterp, chan);
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
		throw Tcl::TclError("error gzip-ing file");
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
