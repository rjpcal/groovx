///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Wed Jun 13 13:15:55 2001
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

#include "tcl/ioitempkg.h"
#include "tcl/tclcmd.h"
#include "tcl/tclevalcmd.h"
#include "tcl/tclitempkg.h"

#include "util/objfactory.h"
#include "util/ref.h"

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

  WeakRef<Toglet> theWidget;

  class BindCmd;
  class CurrentTogletCmd;
  class DumpCmapCmd;
  class InitedCmd;
  class SeeCmd;
  class SetColorCmd;
  class SetCurTrialCmd;
  class SetMinRectCmd;
  class ShowCmd;

  class ObjToglPkg;
  class TogletPkg;
}

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace definitions
//
///////////////////////////////////////////////////////////////////////

class WidgetDestroyCallback : public Toglet::DestroyCallback {
public:
  virtual void onDestroy(Toglet* config)
  {
    if (ObjTogl::theWidget.isValid() && ObjTogl::theWidget.get() == config)
      {
        ObjTogl::theWidget = WeakRef<Toglet>();
      }
  }
};

void ObjTogl::setCurrentTogl(Toglet* togl) {
DOTRACE("ObjTogl::setCurrentTogl");

  theWidget = WeakRef<Toglet>(togl);
  theWidget->setDestroyCallback(new WidgetDestroyCallback);
}

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
// ObjTogl::CurrentToglet --
//
//---------------------------------------------------------------------

class ObjTogl::CurrentTogletCmd : public Tcl::TclCmd {
public:
  CurrentTogletCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, "?item_id?", 1, 2) {}

protected:
  virtual void invoke() {
    if (objc() < 2)
      {
        returnInt(theWidget.id());
      }
    else
      {
        theWidget = Ref<Toglet>( getIntFromArg(1) );
        theWidget->makeCurrent();
      }
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
// ObjTogl::InitedCmd --
//
//---------------------------------------------------------------------

class ObjTogl::InitedCmd : public Tcl::TclCmd {
public:
  InitedCmd(Tcl_Interp* interp, const char* cmd_name) :
    Tcl::TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
    returnBool(ObjTogl::theWidget.isValid());
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

    widg->setDrawable(Ref<GxNode>(id));
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

    Ref<TrialBase>(trial)->installSelf(*getItem());
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

    Ref<TrialBase>(id)->installSelf(*widg);
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
    Tcl_PkgProvide(interp, "Objtogl", "$Revision$");

    addCommand( new BindCmd       (this, "Togl::bind") );
    addCommand( new DumpCmapCmd   (this, "Togl::dumpCmap") );
    addCommand( new InitedCmd     (interp, "Togl::inited") );
    addCommand( new SeeCmd        (this, "Togl::see") );
    addCommand( new SetColorCmd   (this, "Togl::setColor") );
    addCommand( new SetCurTrialCmd(this, "Togl::setCurTrial") );
    addCommand( new SetMinRectCmd (this, "Togl::setMinRect") );
    addCommand( new ShowCmd       (this, "Togl::show") );

    declareCAction("clearscreen", &Toglet::clearscreen);
    declareCAction("destroy", &Toglet::destroyWidget);
    declareCSetter("dumpEps", &Toglet::writeEpsFile, "filename");
    declareCAttrib("height", &Toglet::getHeight, &Toglet::setHeight);
    declareCSetter("hold", &Toglet::setHold, "hold_on?");
    declareCAction("loadDefaultFont", &Toglet::loadDefaultFont);
    declareCSetter("loadFont", &Toglet::loadFont);
    declareCSetter("loadFonti", &Toglet::loadFonti);
    declareCAction("refresh", &Toglet::refresh);
    declareCSetter("scaleRect", &Toglet::scaleRect, "scale");
    declareCSetter("setFixedScale", &Toglet::setFixedScale, "scale");
    declareCSetter("setUnitAngle", &Toglet::setUnitAngle, "angle_in_degrees");
    declareCSetter("setViewingDistance", &Toglet::setViewingDistIn,
                   "distance_in_inches");
    declareCSetter("setVisible", &Toglet::setVisibility, "visibility");
    declareCAction("swapBuffers", &Toglet::swapBuffers);
    declareCAction("takeFocus", &Toglet::takeFocus);
    declareCAction("undraw", &Toglet::undraw);
    declareCGetter("usingFixedScale", &Toglet::usingFixedScale);
    declareCAttrib("width", &Toglet::getWidth, &Toglet::setWidth);

    Tcl_Eval(interp,
             "namespace eval Togl { proc init {} {} }\n"
             "proc clearscreen {} { Togl::clearscreen }\n"
             "proc see {id} { Togl::see $id }\n"
             "proc show {id} { Togl::show $id }\n"
             "proc undraw {} { Togl::undraw }\n"
             "proc redraw {} { Togl::refresh }\n");
  }

  virtual ~ObjToglPkg() {
    if (ObjTogl::theWidget.isValid()) {
      ObjTogl::theWidget->setVisibility(false);
    }
  }

  Toglet* getCItemFromId(int) {
    if (!theWidget.isValid()) { throw Tcl::TclError("no valid Togl exists"); }

    return ObjTogl::theWidget.get();
  }
};


class ObjTogl::TogletPkg : public Tcl::IoItemPkg<Toglet> {
public:
  TogletPkg(Tcl_Interp* interp) :
    Tcl::IoItemPkg<Toglet>(interp, "Toglet", "$Revision$")
  {
    addCommand( new BindCmd       (this, "Toglet::bind") );
    addCommand( new CurrentTogletCmd (interp, "Toglet::currentToglet") );
    addCommand( new DumpCmapCmd   (this, "Toglet::dumpCmap") );
    addCommand( new SeeCmd        (this, "Toglet::see") );
    addCommand( new SetColorCmd   (this, "Toglet::setColor") );
    addCommand( new SetCurTrialCmd(this, "Toglet::setCurTrial") );
    addCommand( new SetMinRectCmd (this, "Toglet::setMinRect") );
    addCommand( new ShowCmd       (this, "Toglet::show") );

    declareCAction("clearscreen", &Toglet::clearscreen);
    declareCAction("destroy", &Toglet::destroyWidget);
    declareCSetter("dumpEps", &Toglet::writeEpsFile, "item_id(s) filename");
    declareCAttrib("height", &Toglet::getHeight, &Toglet::setHeight);
    declareCSetter("hold", &Toglet::setHold, "item_id(s) hold_on?");
    declareCAction("loadDefaultFont", &Toglet::loadDefaultFont);
    declareCSetter("loadFont", &Toglet::loadFont);
    declareCSetter("loadFonti", &Toglet::loadFonti);
    declareCAction("refresh", &Toglet::refresh);
    declareCSetter("scaleRect", &Toglet::scaleRect, "item_id(s) scale");
    declareCSetter("setFixedScale", &Toglet::setFixedScale, "item_id(s) scale");
    declareCSetter("setUnitAngle", &Toglet::setUnitAngle,
                   "item_id(s) angle_in_degrees");
    declareCSetter("setViewingDistance", &Toglet::setViewingDistIn,
                   "item_id(s) distance_in_inches");
    declareCSetter("setVisible", &Toglet::setVisibility,
                   "item_id(s) visibility");
    declareCAction("swapBuffers", &Toglet::swapBuffers);
    declareCAction("takeFocus", &Toglet::takeFocus);
    declareCAction("undraw", &Toglet::undraw);
    declareCGetter("usingFixedScale", &Toglet::usingFixedScale);
    declareCAttrib("width", &Toglet::getWidth, &Toglet::setWidth);
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
      FactoryError::throwForType("Toglet");

    return Toglet::make(toglCreateInterp);
  }
}

extern "C"
int Objtogl_Init(Tcl_Interp* interp) {
DOTRACE("Objtogl_Init");

  new ObjTogl::ObjToglPkg(interp);

  new ObjTogl::TogletPkg(interp);

  toglCreateInterp = interp;

  Util::ObjFactory::theOne().registerCreatorFunc( makeToglet );

  return TCL_OK;
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
