///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
// Rob Peters
// created: Nov-98
// written: Mon Jul 12 13:12:40 1999
// $Id$
//
// This package provides functionality that allows a Togl widget to
// work with a Tlist, controlling its display, reshaping, etc.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "objtogl.h"

#include <strstream.h>
#include <iomanip.h>
#include <GL/gl.h>
#include <togl.h>
#include <tk.h>
#include <string>

#include "gfxattribs.h"
#include "tlist.h"
#include "tclcmd.h"
#include "tclevalcmd.h"
#include "tclitempkg.h"
#include "toglconfig.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace ObjTogl {
  Togl* widget = 0;
  ToglConfig* theConfig = 0;

  bool toglCreated = false;
  
  class TlistToglConfig;

  class InitCmd;
  class DestroyCmd;

  class DumpCmapCmd;
  class DumpEpsCmd;
  class InitedCmd;
  class LoadFontCmd;
  class LoadFontiCmd;
  class SetColorCmd;
  class SetMinRectCmd;

  class ObjToglPkg;

  void toglCreateCallback(struct Togl* togl);
}

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace definitions
//
///////////////////////////////////////////////////////////////////////

class ObjTogl::TlistToglConfig : public ToglConfig {
public:
  TlistToglConfig(Togl* togl, double dist, double unit_angle) :
	 ToglConfig(togl, dist, unit_angle) {}

  virtual void display() {
	 DebugPrintNL("clearing back buffer...");
	 glClear(GL_COLOR_BUFFER_BIT);
	 
	 DebugPrintNL("drawing the trial...");
	 Tlist::theTlist().drawCurTrial();
	 
	 DebugPrintNL("swapping the buffers...");
	 swapBuffers();
	 
	 DebugPrintNL("clearing back buffer...");
	 glClear(GL_COLOR_BUFFER_BIT);
  }
};

ToglConfig* ObjTogl::theToglConfig() {
DOTRACE("ObjTogl::theToglConfig");
  if (!toglCreated) { throw TclError("Togl not yet initialized"); }
  Assert(theConfig != 0);
  return theConfig;
}

void ObjTogl::toglCreateCallback(struct Togl* togl) {
DOTRACE("ObjTogl::toglCreateCallback");

  if (toglCreated) { return; }

  /* else */ ObjTogl::widget = togl;
}

//---------------------------------------------------------------------
//
// ObjTogl::DestroyCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DestroyCmd : public TclCmd {
public:
  DestroyCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 delete theConfig;
	 theConfig = 0;
	 widget = 0;
	 toglCreated = false;
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::DumpCmapCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DumpCmapCmd : public TclItemCmd<ToglConfig> {
public:
  DumpCmapCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name,
									"?start_index=0? ?end_index=255?", 1, 3, false) {}
protected:
  virtual void invoke() {
	 int start = (objc() < 2) ? 0   : getIntFromArg(1);
	 int end   = (objc() < 3) ? 255 : getIntFromArg(2);

	 if (start < 0 || end < 0 || start > 255 || end > 255) {
		static const char* const bad_index_msg = "colormap index out of range";
		throw TclError(bad_index_msg);
	 }

 	 const int BUF_SIZE = 64;
 	 char buf[BUF_SIZE];

	 ToglConfig* config = getItem();
	 ToglConfig::Color color;

	 for (int i = start; i <= end; ++i) {
		buf[0] = '\0';
		ostrstream ost(buf, BUF_SIZE);
		config->queryColor(i, color);
		ost.setf(ios::fixed);
		ost << i << '\t' 
			 << setprecision(3) << color.red << '\t' 
			 << setprecision(3) << color.green << '\t' 
			 << setprecision(3) << color.blue << '\n' << '\0';
		lappendVal(buf);
	 }
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::DumpEpsCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DumpEpsCmd : public TclItemCmd<ToglConfig> {
public:
  DumpEpsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
	 const char* filename = getCstringFromArg(1);

	 getItem()->writeEpsFile(filename);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::InitCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitCmd : public TclCmd {
public:
  InitCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name,
			  "init_args ?viewing_dist=30? ?gl_unit_angle=2.05?", 2, 4),
	 itsInterp(interp) {}
protected:
  virtual void invoke() {
	 if (toglCreated) { throw TclError("Togl widget already initialized"); }

	 const char* init_args     = getCstringFromArg(1);
	 int         viewing_dist  = (objc() < 3) ? 30 : getIntFromArg(2);
	 double      gl_unit_angle = (objc() < 4) ? 2.05 : getDoubleFromArg(3);

	 const char* pathname = ".togl_private";

	 string create_cmd_str = string("togl ") + pathname + " " + init_args;
	 TclEvalCmd create_cmd(create_cmd_str.c_str());
	 if ( create_cmd.invoke(itsInterp) != TCL_OK ) { throw TclError(); }

	 // Make sure that widget creation and the create callback
	 // successfully set ObjTogl::widget to point to a struct Togl
	 if (ObjTogl::widget == 0) { throw TclError(); }

	 // Create a new ToglConfig object using the following default
	 // settings: viewing distance = 30 inches, one GL unit == 2.05
	 // degrees visual angle
	 ObjTogl::theConfig = new TlistToglConfig(ObjTogl::widget, 30, 2.05);
	 
	 string pack_cmd_str = string("pack ") + pathname + " -expand 1 -fill both";
	 TclEvalCmd pack_cmd(pack_cmd_str.c_str());
	 if ( pack_cmd.invoke(itsInterp) != TCL_OK ) { throw TclError(); }

	 toglCreated = true;

	 returnVoid();
  }
private:
  Tcl_Interp* itsInterp;
};

//---------------------------------------------------------------------
//
// ObjTogl::InitedCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitedCmd : public TclCmd {
public:
  InitedCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 returnBool(ObjTogl::toglCreated);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::LoadFontCmd --
//
//---------------------------------------------------------------------

class ObjTogl::LoadFontCmd : public TclItemCmd<ToglConfig> {
public:
  LoadFontCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "?fontname?", 1, 2) {}
protected:
  virtual void invoke() {
	 const char* fontname = (objc() < 2) ? 0 : getCstringFromArg(1);

	 getItem()->loadFont(fontname);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::LoadFontiCmd --
//
//---------------------------------------------------------------------

class ObjTogl::LoadFontiCmd : public TclItemCmd<ToglConfig> {
public:
  LoadFontiCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "?fontnumber?", 1, 2) {}
protected:
  virtual void invoke() {
	 int fontnumber = (objc() < 2) ? 0 : getIntFromArg(1);

	 getItem()->loadFonti(fontnumber);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetColorCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetColorCmd : public TclItemCmd<ToglConfig> {
public:
  SetColorCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "index r g b", 5, 5) {}
protected:
  virtual void invoke() {
	 int i = getIntFromArg(1);
	 double r = getDoubleFromArg(2);
	 double g = getDoubleFromArg(3);
	 double b = getDoubleFromArg(4);

	 getItem()->setColor(ToglConfig::Color(i, r, g, b));
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetMinRectCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetMinRectCmd : public TclItemCmd<ToglConfig> {
public:
  SetMinRectCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "left top right bottom", 5, 5) {}
protected:
  virtual void invoke() {
	 double l = getDoubleFromArg(1);
	 double t = getDoubleFromArg(2);
	 double r = getDoubleFromArg(3);
	 double b = getDoubleFromArg(4);

	 // Test for valid rect: right > left && top > bottom. In
	 // particular, we must not have right == left or top == bottom
	 // since this collapses the space onto one dimension.
	 if (r <= l || t <= b) { throw TclError("invalid rect"); }
	 
	 getItem()->setMinRectLTRB(l,t,r,b);
  }
};

//---------------------------------------------------------------------
//
// ObjToglPkg class definition
//
//---------------------------------------------------------------------

class ObjTogl::ObjToglPkg : public CTclItemPkg<ToglConfig> {
public:
  ObjToglPkg(Tcl_Interp* interp) :
	 CTclItemPkg<ToglConfig>(interp, "Togl", "3.2", 0)
  {
	 Tcl_PkgProvide(interp, "Objtogl", "3.2");

	 Togl_CreateFunc(toglCreateCallback);

	 addCommand( new DestroyCmd    (interp, "Togl::destroy") );
	 addCommand( new DumpCmapCmd   (this, "Togl::dumpCmap") );
	 addCommand( new DumpEpsCmd    (this, "Togl::dumpEps") );
	 addCommand( new InitCmd       (interp, "Togl::init") );
	 addCommand( new InitedCmd     (interp, "Togl::inited") );
	 addCommand( new LoadFontCmd   (this, "Togl::loadFont") );
	 addCommand( new LoadFontiCmd  (this, "Togl::loadFonti") );
	 addCommand( new SetColorCmd   (this, "Togl::setColor") );
	 addCommand( new SetMinRectCmd (this, "Togl::setMinRect") );

	 declareCAttrib("height",
						 &ToglConfig::getHeight, &ToglConfig::setHeight);
	 declareCSetter("scaleRect", &ToglConfig::scaleRect, "scale");
	 declareCSetter("setFixedScale", &ToglConfig::setFixedScale, "scale");
	 declareCSetter("setUnitAngle", &ToglConfig::setUnitAngle,
						 "angle_in_degrees");
	 declareCSetter("setViewingDistance", &ToglConfig::setViewingDistIn,
						 "distance_in_inches");
	 declareCAction("swapBuffers", &ToglConfig::swapBuffers);
	 declareCGetter("usingFixedScale", &ToglConfig::usingFixedScale);
	 declareCAttrib("width", &ToglConfig::getWidth, &ToglConfig::setWidth);
  }

  virtual ~ObjToglPkg() {
	 delete theConfig;
  }

  ToglConfig* getCItemFromId(int) {
	 return ObjTogl::theToglConfig();
  }
};

//---------------------------------------------------------------------
//
// Objtogl_Init
//
//---------------------------------------------------------------------

int Objtogl_Init(Tcl_Interp* interp) {
DOTRACE("Objtogl_Init");

  new ObjTogl::ObjToglPkg(interp); 

  return TCL_OK;
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
