///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
// Rob Peters
// created: Nov-98
// written: Tue Nov  9 13:18:11 1999
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
#define LOCAL_ASSERT
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

  class BindCmd;
  class DumpCmapCmd;
  class DumpEpsCmd;
  class InitedCmd;
  class LoadFontCmd;
  class LoadFontiCmd;
  class SetColorCmd;
  class SetMinRectCmd;

  class ObjToglPkg;
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
	 try {
		Tlist::theTlist().drawCurTrial();
	 }
	 catch (InvalidIdError& err) {
		DebugEvalNL(err.msg());
		Tlist::theTlist().setVisible(false);
	 }
    
    DebugPrintNL("swapping the buffers...");
    swapBuffers();
    
    DebugPrintNL("clearing back buffer...");
    glClear(GL_COLOR_BUFFER_BIT);
  }
};

bool ObjTogl::toglHasBeenCreated() {
DOTRACE("ObjTogl::toglHasBeenCreated");
  return toglCreated;
}

ToglConfig* ObjTogl::theToglConfig() {
DOTRACE("ObjTogl::theToglConfig");
  if (!toglCreated) { throw TclError("Togl not yet initialized"); }
  Assert(theConfig != 0);
  return theConfig;
}

//---------------------------------------------------------------------
//
// ObjTogl::DestroyCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DestroyCmd : public TclCmd {
public:
  DestroyCmd(Tcl_Interp* interp, const char* cmd_name) :
    TclCmd(interp, cmd_name, NULL, 1, 1)
  {
    Togl_DestroyFunc(destroyCallback);  
  }

  ~DestroyCmd() { try {invoke();} catch (...) {} }

  static void destroyCallback(struct Togl* togl) {
  DOTRACE("ObjTogl::DestroyCmd::destroyCallback");
    if ( (togl != 0) && (togl == ObjTogl::widget) ) {
      ToglConfig* config = static_cast<ToglConfig*>(Togl_GetClientData(togl));
      DebugEvalNL((void*)config);
      delete config;
      ObjTogl::theConfig = 0;
      ObjTogl::widget = 0;
      ObjTogl::toglCreated = false;
    }
  }

protected:
  virtual void invoke() {
  DOTRACE("ObjTogl::DestroyCmd::invoke");

    if (toglCreated) {
      // This tells the ToglConfig to destroy the widget, which in
      // turn generates a call to the destroyCallback, which in turn
      // delete's the toglConfig.
      ObjTogl::theToglConfig()->destroyWidget();
    }
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::BindCmd --
//
//---------------------------------------------------------------------

class ObjTogl::BindCmd : public TclItemCmd<ToglConfig> {
public:
  BindCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<ToglConfig>(pkg, cmd_name, "event_sequence binding_script",
									3, 3) {}
protected:
  virtual void invoke() {
	 const char* event_sequence = getCstringFromArg(1);
	 const char* binding_script = getCstringFromArg(2);
	 
	 getItem()->bind(event_sequence, binding_script);
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
           "init_args ?viewing_dist=30?"
			  "?gl_unit_angle=2.05? ?pack=yes?", 2, 5),
    itsInterp(interp)
  {
    Togl_CreateFunc(createCallback);
  }
protected:
  static void createCallback(struct Togl* togl) {
    if (ObjTogl::toglCreated) { return; }
    /* else */ ObjTogl::widget = togl;
  }

  virtual void invoke() {
    if (toglCreated) { throw TclError("Togl widget already initialized"); }

    const char* init_args     = getCstringFromArg(1);
    int         viewing_dist  = (objc() < 3) ? 30 : getIntFromArg(2);
    double      gl_unit_angle = (objc() < 4) ? 2.05 : getDoubleFromArg(3);
	 bool        pack          = (objc() < 5) ? true : getBoolFromArg(4);

    const char* pathname = ".togl_private";

    // Eval a command to create the widget. This will cause in turn
    // call the createCallback as part of Togl's internal creation
    // procedures.
    string create_cmd_str = string("togl ") + pathname + " " + init_args;
    TclEvalCmd create_cmd(create_cmd_str.c_str(), TclEvalCmd::THROW_EXCEPTION);
    create_cmd.invoke(itsInterp);

    // Make sure that widget creation and the create callback
    // successfully set ObjTogl::widget to point to a struct Togl
    if (ObjTogl::widget == 0) { throw TclError(); }

    // Create a new ToglConfig object using the following default
    // settings: viewing distance = 30 inches, one GL unit == 2.05
    // degrees visual angle
    ObjTogl::theConfig = new TlistToglConfig(ObjTogl::widget, 30, 2.05);
    
	 if (pack) {
		string pack_cmd_str =
		  string("pack ") + pathname + " -expand 1 -fill both";
		TclEvalCmd pack_cmd(pack_cmd_str.c_str(), TclEvalCmd::THROW_EXCEPTION);
		pack_cmd.invoke(itsInterp);
	 }

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

	 addCommand( new BindCmd       (this, "Togl::bind") );
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
