///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Jun  6 15:54:58 2001
// $Id$
//
// This package provides functionality that controlling the display,
// reshaping, etc. of a Togl widget.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "objtogl.h"

#include "toglconfig.h"
#include "trialbase.h"

#include "gx/gxnode.h"

#include "io/io.h"

#include "tcl/tclcmd.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclitempkg.h"

#include "util/iditem.h"
#include "util/strings.h"

#include <tcl.h>
#include <strstream.h>
#include <iomanip.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace ObjTogl {
  ToglConfig* theWidget = 0;

  bool toglCreated = false;
  
  class InitCmd;
  class DestroyCmd;

  class BindCmd;
  class DumpCmapCmd;
  class DumpEpsCmd;
  class HoldCmd;
  class InitedCmd;
  class SeeCmd;
  class SetColorCmd;
  class SetCurTrialCmd;
  class SetMinRectCmd;
  class SetVisibleCmd;
  class ShowCmd;

  class ObjToglPkg;

  ToglConfig* theToglConfig();
}

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace definitions
//
///////////////////////////////////////////////////////////////////////

bool ObjTogl::toglHasBeenCreated() {
DOTRACE("ObjTogl::toglHasBeenCreated");
  return toglCreated;
}

GWT::Widget* ObjTogl::theGwtWidget() {
DOTRACE("ObjTogl::theGwtWidget");
  return theToglConfig(); 
}

ToglConfig* ObjTogl::theToglConfig() {
DOTRACE("ObjTogl::theToglConfig");
  if (!toglCreated) { throw Tcl::TclError("Togl not yet initialized"); }
  Assert(theWidget != 0);
  return theWidget;
}

//---------------------------------------------------------------------
//
// ObjTogl::DestroyCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DestroyCmd : public Tcl::TclCmd {
public:
  DestroyCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, NULL, 1, 1)
  {}

protected:
  virtual void invoke() {
  DOTRACE("ObjTogl::DestroyCmd::invoke");

    if (toglCreated) {
      // This tells the ToglConfig to destroy itsTogl, which in turn
      // generates a call to the destroyCallback, which in turn
      // delete's the toglConfig.
      theWidget->destroyWidget();
    }
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::BindCmd --
//
//---------------------------------------------------------------------

class ObjTogl::BindCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  BindCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name,
										  "event_sequence binding_script", 3, 3) {}
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

class ObjTogl::DumpCmapCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  DumpCmapCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name,
										  "?start_index=0? ?end_index=255?",
										  1, 3, false) {}
protected:
  virtual void invoke() {
    int start = (objc() < 2) ? 0   : getIntFromArg(1);
    int end   = (objc() < 3) ? 255 : getIntFromArg(2);

    if (start < 0 || end < 0 || start > 255 || end > 255) {
      static const char* const bad_index_msg = "colormap index out of range";
      throw Tcl::TclError(bad_index_msg);
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

class ObjTogl::DumpEpsCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  DumpEpsCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "filename", 2, 2) {}
protected:
  virtual void invoke() {
    const char* filename = getCstringFromArg(1);

    getItem()->writeEpsFile(filename);
  }
};

//--------------------------------------------------------------------
//
// ObjTogl::HoldCmd --
//
// Make a specified trial the widget's current trial, and draw it in
// the OpenGL window. The widget's visibility is set to true.
//
//--------------------------------------------------------------------

class ObjTogl::HoldCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  HoldCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "hold_on?", 2, 2) {}
protected:
  virtual void invoke() {
	 bool hold_on = getBoolFromArg(1);

	 getItem()->setHold(hold_on);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::InitCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitCmd : public Tcl::TclCmd {
public:
  InitCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name,
					 "init_args ?viewing_dist=30?"
					 "?gl_unit_angle=2.05? ?pack=yes?", 1, 5) {}

protected:
  class WidgetDestroyCallback : public ToglConfig::DestroyCallback {
  public:
	 virtual void onDestroy(ToglConfig* config)
		{
		  if (ObjTogl::theWidget == config)
			 {
				ObjTogl::theWidget = 0;
				ObjTogl::toglCreated = false;
			 }
		}
  };

  virtual void invoke();
};

void ObjTogl::InitCmd::invoke() {
DOTRACE("ObjTogl::InitCmd::invoke");
  if (toglCreated) { throw Tcl::TclError("Togl widget already initialized"); }

  const char* dflt = "-stereo false";

  const char* init_args     =      (objc() < 2) ? dflt : getCstringFromArg(1);
  int         viewing_dist  =      (objc() < 3) ? 30   : getIntFromArg(2);
  double      gl_unit_angle =      (objc() < 4) ? 2.05 : getDoubleFromArg(3);
  bool        pack          = bool((objc() < 5) ? true : getBoolFromArg(4));

  const char* pathname = ".togl_private";

  // Generate an argc/argv array of configuration options
  int config_argc = 0;
  char** config_argv = 0;
  Tcl_SplitList(interp(), init_args, &config_argc, &config_argv);

  // Create a new ToglConfig object with the specified conguration
  // options, viewing distance, and visual angle per GL unit
  ObjTogl::theWidget = new ToglConfig(interp(), pathname,
												  config_argc, config_argv,
												  viewing_dist, gl_unit_angle);

  Tcl_Free((char*) config_argv);

  ObjTogl::theWidget->setDestroyCallback(new WidgetDestroyCallback);

  if (pack) {
	 dynamic_string pack_cmd_str = "pack ";
	 pack_cmd_str += pathname;
	 pack_cmd_str += " -expand 1 -fill both";
	 Tcl::TclEvalCmd pack_cmd(pack_cmd_str.c_str(),
									  Tcl::TclEvalCmd::THROW_EXCEPTION);
	 pack_cmd.invoke(interp());
  }

  toglCreated = true;

  returnVoid();
}

//---------------------------------------------------------------------
//
// ObjTogl::InitedCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitedCmd : public Tcl::TclCmd {
public:
  InitedCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
    returnBool(ObjTogl::toglCreated);
  }
};

//--------------------------------------------------------------------
//
// ObjTogl::SeeCmd --
//
// Make a specified GxNode the widget's current drawable, and draw it
// in the OpenGL window. The widget's visibility is set to true.
//
//--------------------------------------------------------------------

class ObjTogl::SeeCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  SeeCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
  DOTRACE("SeeCmd::invoke");
	 int id = getIntFromArg(1);

	 GWT::Widget* widg = getItem();

	 widg->setDrawable(IdItem<GxNode>(id));
	 widg->setVisibility(true);

	 widg->display();
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetColorCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetColorCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  SetColorCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "index r g b", 5, 5) {}
protected:
  virtual void invoke() {
    int i = getIntFromArg(1);
    double r = getDoubleFromArg(2);
    double g = getDoubleFromArg(3);
    double b = getDoubleFromArg(4);

    getItem()->setColor(ToglConfig::Color(i, r, g, b));
  }
};

//--------------------------------------------------------------------
//
// ObjTogl::SetCurTrialCmd --
//
// Change the widget's current trial to a specified trial id. The
// current trial is the one that will be displayed by a subsequent
// call to "redraw", or by remap events sent to the screen
// window. Returns an error if the specified trial id is not valid.
//
//--------------------------------------------------------------------

class ObjTogl::SetCurTrialCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  SetCurTrialCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "trial_id", 2, 2) {}
protected:
  virtual void invoke() {
	 int trial = getIntFromArg(1);

	 IdItem<TrialBase>(trial)->installSelf(*getItem());
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetMinRectCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetMinRectCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  SetMinRectCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "left top right bottom", 5, 5) {}
protected:
  virtual void invoke() {
    double l = getDoubleFromArg(1);
    double t = getDoubleFromArg(2);
    double r = getDoubleFromArg(3);
    double b = getDoubleFromArg(4);

    // Test for valid rect: right > left && top > bottom. In
    // particular, we must not have right == left or top == bottom
    // since this collapses the space onto one dimension.
    if (r <= l || t <= b) { throw Tcl::TclError("invalid rect"); }
    
    getItem()->setMinRectLTRB(l,t,r,b);
  }
};


//--------------------------------------------------------------------
//
// ObjTogl::SetVisibleCmd --
//
//--------------------------------------------------------------------

class ObjTogl::SetVisibleCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  SetVisibleCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "visibility", 2, 2) {}
protected:
  virtual void invoke() {
	 bool vis = getBoolFromArg(1);

	 getItem()->setVisibility(vis);
  }
};

//--------------------------------------------------------------------
//
// ObjTogl::ShowCmd --
//
// Make a specified trial the widget's current trial, and draw it in
// the OpenGL window. The widget's visibility is set to true.
//
//--------------------------------------------------------------------

class ObjTogl::ShowCmd : public Tcl::TclItemCmd<ToglConfig> {
public:
  ShowCmd(Tcl::CTclItemPkg<ToglConfig>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<ToglConfig>(pkg, cmd_name, "trial_id", 2, 2) {}
protected:
  virtual void invoke() {
  DOTRACE("ShowCmd::invoke");
	 int id = getIntFromArg(1);

	 GWT::Widget* widg = getItem();

	 IdItem<TrialBase>(id)->installSelf(*widg);
	 widg->setVisibility(true);

	 widg->display();
  }
};

//---------------------------------------------------------------------
//
// ObjToglPkg class definition
//
//---------------------------------------------------------------------

class ObjTogl::ObjToglPkg : public Tcl::CTclItemPkg<ToglConfig> {
public:
  ObjToglPkg(Tcl_Interp* interp) :
    Tcl::CTclItemPkg<ToglConfig>(interp, "Togl", "$Revision$", 0)
  {
    Tcl_PkgProvide(interp, "Objtogl", "3.2");

	 addCommand( new BindCmd       (this, "Togl::bind") );
    addCommand( new DestroyCmd    (interp, "Togl::destroy") );
    addCommand( new DumpCmapCmd   (this, "Togl::dumpCmap") );
    addCommand( new DumpEpsCmd    (this, "Togl::dumpEps") );
	 addCommand( new HoldCmd       (this, "Togl::hold") );
    addCommand( new InitCmd       (interp, "Togl::init") );
    addCommand( new InitedCmd     (interp, "Togl::inited") );
	 addCommand( new SeeCmd        (this, "Togl::see") );
    addCommand( new SetColorCmd   (this, "Togl::setColor") );
	 addCommand( new SetCurTrialCmd(this, "Togl::setCurTrial") );
    addCommand( new SetMinRectCmd (this, "Togl::setMinRect") );
    addCommand( new SetVisibleCmd (this, "Togl::setVisible") );
	 addCommand( new ShowCmd       (this, "Togl::show") );

	 declareCAction("clearscreen", &ToglConfig::clearscreen);
    declareCAttrib("height",
                   &ToglConfig::getHeight, &ToglConfig::setHeight);
	 declareCAction("loadDefaultFont", &ToglConfig::loadDefaultFont);
	 declareCSetter("loadFont", &ToglConfig::loadFont);
	 declareCSetter("loadFonti", &ToglConfig::loadFonti);
	 declareCAction("refresh", &ToglConfig::refresh);
    declareCSetter("scaleRect", &ToglConfig::scaleRect, "scale");
    declareCSetter("setFixedScale", &ToglConfig::setFixedScale, "scale");
    declareCSetter("setUnitAngle", &ToglConfig::setUnitAngle,
                   "angle_in_degrees");
    declareCSetter("setViewingDistance", &ToglConfig::setViewingDistIn,
                   "distance_in_inches");
    declareCAction("swapBuffers", &ToglConfig::swapBuffers);
	 declareCAction("takeFocus", &ToglConfig::takeFocus);
	 declareCAction("undraw", &ToglConfig::undraw);
    declareCGetter("usingFixedScale", &ToglConfig::usingFixedScale);
    declareCAttrib("width", &ToglConfig::getWidth, &ToglConfig::setWidth);

	 Tcl_Eval(interp,
				 "proc clearscreen {} { Togl::clearscreen }\n"
				 "proc see {id} { Togl::see $id }\n"
				 "proc show {id} { Togl::show $id }\n"
				 "proc undraw {} { Togl::undraw }\n"
				 "proc redraw {} { Togl::refresh }\n");
  }

  virtual ~ObjToglPkg() {
	 if (toglCreated) {
		ObjTogl::theToglConfig()->setVisibility(false);
	 }
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
