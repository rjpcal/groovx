///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Jun  6 20:26:14 2001
// $Id$
//
// This package provides functionality that controlling the display,
// reshaping, etc. of a Togl widget.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "objtogl.h"

#include "toglet.h"
#include "trialbase.h"

#include "gx/gxnode.h"

#include "io/io.h"

#include "tcl/ioitempkg.h"
#include "tcl/tclcmd.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclitempkg.h"

#include "util/iditem.h"
#include "util/objfactory.h"
#include "util/strings.h"

// This hack causes this file to be compiled into the main executable
// (through the rules in place in the Makefile), rather than a shared
// library, so that this file can find the typeinfo for Toglet.
#ifdef GCC_COMPILER
#  include <GL/gl.h>
#endif

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

  MaybeIdItem<Toglet> theWidget;

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
}

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace definitions
//
///////////////////////////////////////////////////////////////////////

class WidgetDestroyCallback : public Toglet::DestroyCallback {
public:
  virtual void onDestroy(Toglet* config);
};

void WidgetDestroyCallback::onDestroy(Toglet* config)
  {
	 if (ObjTogl::theWidget.isValid() &&
		  ObjTogl::theWidget.get() == config)
		{
  		  ObjTogl::theWidget = MaybeIdItem<Toglet>();
  		  ObjTogl::toglCreated = false;
		}
  }

void ObjTogl::setCurrentTogl(Toglet* togl) {
DOTRACE("ObjTogl::setCurrentTogl");

  theWidget = MaybeIdItem<Toglet>(togl);
  theWidget->setDestroyCallback(new WidgetDestroyCallback);
  toglCreated = true;
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
      // This tells the Toglet to destroy itsTogl
		theWidget->destroyWidget();
    }
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::BindCmd --
//
//---------------------------------------------------------------------

class ObjTogl::BindCmd : public Tcl::TclItemCmd<Toglet> {
public:
  BindCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name,
									 "event_sequence binding_script",
									 pkg->itemArgn()+3) {}
protected:
  virtual void invoke() {
	 const char* event_sequence = getCstringFromArg(afterItemArg(1));
	 const char* binding_script = getCstringFromArg(afterItemArg(2));
	 
	 getItem()->bind(event_sequence, binding_script);
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::DumpCmapCmd --
//
//---------------------------------------------------------------------

class ObjTogl::DumpCmapCmd : public Tcl::TclItemCmd<Toglet> {
public:
  DumpCmapCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<Toglet>(pkg, cmd_name,
									 "?start_index=0? ?end_index=255?",
									 pkg->itemArgn()+1, pkg->itemArgn()+3, false) {}
protected:
  virtual void invoke() {
    int start = (objc() < 2) ? 0   : getIntFromArg(afterItemArg(1));
    int end   = (objc() < 3) ? 255 : getIntFromArg(afterItemArg(2));

    if (start < 0 || end < 0 || start > 255 || end > 255) {
      static const char* const bad_index_msg = "colormap index out of range";
      throw Tcl::TclError(bad_index_msg);
    }

    const int BUF_SIZE = 64;
    char buf[BUF_SIZE];

    Toglet* config = getItem();
    Toglet::Color color;

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

class ObjTogl::DumpEpsCmd : public Tcl::TclItemCmd<Toglet> {
public:
  DumpEpsCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "filename", pkg->itemArgn()+2) {}
protected:
  virtual void invoke() {
    const char* filename = getCstringFromArg(afterItemArg(1));

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

class ObjTogl::HoldCmd : public Tcl::TclItemCmd<Toglet> {
public:
  HoldCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "hold_on?", pkg->itemArgn()+2) {}
protected:
  virtual void invoke() {
	 bool hold_on = getBoolFromArg(afterItemArg(1));

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
    Tcl::TclCmd(interp, cmd_name, 0, 1, 1) {}

protected:

  virtual void invoke() { /* no-op */; }
};

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

class ObjTogl::SeeCmd : public Tcl::TclItemCmd<Toglet> {
public:
  SeeCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "item_id", pkg->itemArgn()+2) {}
protected:
  virtual void invoke() {
  DOTRACE("SeeCmd::invoke");
	 int id = getIntFromArg(afterItemArg(1));

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

class ObjTogl::SetColorCmd : public Tcl::TclItemCmd<Toglet> {
public:
  SetColorCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "index r g b", pkg->itemArgn()+5) {}

protected:
  virtual void invoke() {
    int i = getIntFromArg(afterItemArg(1));
    double r = getDoubleFromArg(afterItemArg(2));
    double g = getDoubleFromArg(afterItemArg(3));
    double b = getDoubleFromArg(afterItemArg(4));

    getItem()->setColor(Toglet::Color(i, r, g, b));
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

class ObjTogl::SetCurTrialCmd : public Tcl::TclItemCmd<Toglet> {
public:
  SetCurTrialCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "trial_id", pkg->itemArgn()+2) {}
protected:
  virtual void invoke() {
	 int trial = getIntFromArg(afterItemArg(1));

	 IdItem<TrialBase>(trial)->installSelf(*getItem());
  }
};

//---------------------------------------------------------------------
//
// ObjTogl::SetMinRectCmd --
//
//---------------------------------------------------------------------

class ObjTogl::SetMinRectCmd : public Tcl::TclItemCmd<Toglet> {
public:
  SetMinRectCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "left top right bottom",
									 pkg->itemArgn()+5) {}
protected:
  virtual void invoke() {
    double l = getDoubleFromArg(afterItemArg(1));
    double t = getDoubleFromArg(afterItemArg(2));
    double r = getDoubleFromArg(afterItemArg(3));
    double b = getDoubleFromArg(afterItemArg(4));

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

class ObjTogl::SetVisibleCmd : public Tcl::TclItemCmd<Toglet> {
public:
  SetVisibleCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "visibility", pkg->itemArgn()+2) {}

protected:
  virtual void invoke() {
	 bool vis = getBoolFromArg(afterItemArg(1));

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

class ObjTogl::ShowCmd : public Tcl::TclItemCmd<Toglet> {
public:
  ShowCmd(Tcl::CTclItemPkg<Toglet>* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Toglet>(pkg, cmd_name, "trial_id", pkg->itemArgn()+2) {}

protected:
  virtual void invoke() {
  DOTRACE("ShowCmd::invoke");
	 int id = getIntFromArg(afterItemArg(1));

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

class ObjTogl::ObjToglPkg : public Tcl::CTclItemPkg<Toglet> {
public:
  ObjToglPkg(Tcl_Interp* interp) :
    Tcl::CTclItemPkg<Toglet>(interp, "Togl", "$Revision$", 0)
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

	 declareCAction("clearscreen", &Toglet::clearscreen);
    declareCAttrib("height",
                   &Toglet::getHeight, &Toglet::setHeight);
	 declareCAction("loadDefaultFont", &Toglet::loadDefaultFont);
	 declareCSetter("loadFont", &Toglet::loadFont);
	 declareCSetter("loadFonti", &Toglet::loadFonti);
	 declareCAction("refresh", &Toglet::refresh);
    declareCSetter("scaleRect", &Toglet::scaleRect, "scale");
    declareCSetter("setFixedScale", &Toglet::setFixedScale, "scale");
    declareCSetter("setUnitAngle", &Toglet::setUnitAngle,
                   "angle_in_degrees");
    declareCSetter("setViewingDistance", &Toglet::setViewingDistIn,
                   "distance_in_inches");
    declareCAction("swapBuffers", &Toglet::swapBuffers);
	 declareCAction("takeFocus", &Toglet::takeFocus);
	 declareCAction("undraw", &Toglet::undraw);
    declareCGetter("usingFixedScale", &Toglet::usingFixedScale);
    declareCAttrib("width", &Toglet::getWidth, &Toglet::setWidth);

	 Tcl_Eval(interp,
				 "proc clearscreen {} { Togl::clearscreen }\n"
				 "proc see {id} { Togl::see $id }\n"
				 "proc show {id} { Togl::show $id }\n"
				 "proc undraw {} { Togl::undraw }\n"
				 "proc redraw {} { Togl::refresh }\n");
  }

  virtual ~ObjToglPkg() {
	 if (toglCreated) {
  		ObjTogl::theWidget->setVisibility(false);
	 }
  }

  Toglet* getCItemFromId(int) {
	 if (!toglCreated) { throw Tcl::TclError("no valid Togl exists"); }
	 return ObjTogl::theWidget.get();
  }
};

namespace ObjTogl { class ObjToglPkg2; }

class ObjTogl::ObjToglPkg2 : public Tcl::IoItemPkg<Toglet> {
public:
  ObjToglPkg2(Tcl_Interp* interp) :
    Tcl::IoItemPkg<Toglet>(interp, "Togl_", "$Revision$")
  {
    Tcl_PkgProvide(interp, "Objtogl", "3.2");

	 addCommand( new BindCmd       (this, "Togl_::bind") );
    addCommand( new DestroyCmd    (interp, "Togl_::destroy") );
    addCommand( new DumpCmapCmd   (this, "Togl_::dumpCmap") );
    addCommand( new DumpEpsCmd    (this, "Togl_::dumpEps") );
	 addCommand( new HoldCmd       (this, "Togl_::hold") );
    addCommand( new InitCmd       (interp, "Togl_::init") );
    addCommand( new InitedCmd     (interp, "Togl_::inited") );
	 addCommand( new SeeCmd        (this, "Togl_::see") );
    addCommand( new SetColorCmd   (this, "Togl_::setColor") );
	 addCommand( new SetCurTrialCmd(this, "Togl_::setCurTrial") );
    addCommand( new SetMinRectCmd (this, "Togl_::setMinRect") );
    addCommand( new SetVisibleCmd (this, "Togl_::setVisible") );
	 addCommand( new ShowCmd       (this, "Togl_::show") );

	 declareCAction("clearscreen", &Toglet::clearscreen);
    declareCAttrib("height",
                   &Toglet::getHeight, &Toglet::setHeight);
	 declareCAction("loadDefaultFont", &Toglet::loadDefaultFont);
	 declareCSetter("loadFont", &Toglet::loadFont);
	 declareCSetter("loadFonti", &Toglet::loadFonti);
	 declareCAction("refresh", &Toglet::refresh);
    declareCSetter("scaleRect", &Toglet::scaleRect, "scale");
    declareCSetter("setFixedScale", &Toglet::setFixedScale, "scale");
    declareCSetter("setUnitAngle", &Toglet::setUnitAngle,
                   "angle_in_degrees");
    declareCSetter("setViewingDistance", &Toglet::setViewingDistIn,
                   "distance_in_inches");
    declareCAction("swapBuffers", &Toglet::swapBuffers);
	 declareCAction("takeFocus", &Toglet::takeFocus);
	 declareCAction("undraw", &Toglet::undraw);
    declareCGetter("usingFixedScale", &Toglet::usingFixedScale);
    declareCAttrib("width", &Toglet::getWidth, &Toglet::setWidth);

	 Tcl_Eval(interp,
				 "proc clearscreen_ {} { Togl_::clearscreen }\n"
				 "proc see_ {id} { Togl_::see $id }\n"
				 "proc show_ {id} { Togl_::show $id }\n"
				 "proc undraw_ {} { Togl_::undraw }\n"
				 "proc redraw_ {} { Togl_::refresh }\n");
  }

  virtual ~ObjToglPkg2() {
	 if (toglCreated) {
  		ObjTogl::theWidget->setVisibility(false);
	 }
  }
};

//---------------------------------------------------------------------
//
// Objtogl_Init
//
//---------------------------------------------------------------------

namespace {
  Tcl_Interp* toglCreateInterp = 0;

  Toglet* makeToglet()
  {
	 if (toglCreateInterp == 0)
		throw FactoryError("Toglet");

	 return Toglet::make(toglCreateInterp);
  }
}

extern "C"
int Objtogl_Init(Tcl_Interp* interp) {
DOTRACE("Objtogl_Init");

  new ObjTogl::ObjToglPkg(interp); 

  new ObjTogl::ObjToglPkg2(interp); 

  toglCreateInterp = interp;

  Util::ObjFactory::theOne().registerCreatorFunc( makeToglet );

  return TCL_OK;
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
