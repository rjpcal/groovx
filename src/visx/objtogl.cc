///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Mon Aug 13 15:24:01 2001
// $Id$
//
// This package provides functionality that controlling the display,
// reshaping, etc. of a Togl widget.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "grshapp.h"
#include "toglet.h"
#include "trialbase.h"

#include "gfx/gxnode.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"
#include "util/ref.h"

// This hack causes this file to be compiled into the main executable
// (through the rules in place in the Makefile), rather than a shared
// library, so that this file can find the typeinfo for Toglet.
#ifdef GCC_COMPILER
#  include <GL/gl.h>
#endif

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace ObjTogl
{
  WeakRef<Toglet> theWidget;

  void ObjTogl::setCurrentTogl(WeakRef<Toglet> toglet)
  {
    DOTRACE("ObjTogl::setCurrentTogl");

    theWidget = toglet;

    // Install the experiment into the application
    Application& app = Application::theApp();
    GrshApp* grshapp = dynamic_cast<GrshApp*>(&app);

    if (grshapp != 0)
      {
        grshapp->installCanvas(theWidget->getCanvas());
      }
    toglet->makeCurrent();
  }

  WeakRef<Toglet> getCurrentTogl()
  {
    return theWidget;
  }

  Tcl::List dumpCmap(WeakRef<Toglet> config, unsigned int start, unsigned end)
  {
    if (start > 255 || end > 255)
      {
        throw Tcl::TclError("colormap index out of range");
      }

    Toglet::Color color;

    Tcl::List result;

    for (unsigned int i = start; i <= end; ++i)
      {
        config->queryColor(i, color);
        Tcl::List color_list;
        color_list.append(i);
        color_list.append(color.red);
        color_list.append(color.green);
        color_list.append(color.blue);
        result.append(color_list);
      }

    return result;
  }

  Tcl::List dumpCmapAll(WeakRef<Toglet> toglet)
  {
    return dumpCmap(toglet, 0, 255);
  }

  bool inited()
  {
    return ObjTogl::theWidget.isValid();
  }

  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  void see(WeakRef<GWT::Widget> widg, Ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->display();
  }

  void setColor(WeakRef<Toglet> toglet, unsigned int i,
                double r, double g, double b)
  {
    toglet->setColor(Toglet::Color(i, r, g, b));
  }

  // Change the widget's current trial to a specified trial id. The
  // current trial is the one that will be displayed by a subsequent
  // call to "redraw", or by remap events sent to the screen
  // window. Returns an error if the specified trial id is not valid.
  void setCurTrial(WeakRef<Toglet> toglet, Ref<TrialBase> trial)
  {
    trial->installSelf(toglet);
  }

  void setMinRect(WeakRef<Toglet> toglet,
                  double l, double t, double r, double b)
  {
    // Test for valid rect: right > left && top > bottom. In
    // particular, we must not have right == left or top == bottom
    // since this collapses the space onto one dimension.
    if (r <= l || t <= b)
      {
        throw Tcl::TclError("invalid rect");
      }

    toglet->setMinRectLTRB(l,t,r,b);
  }

  // Make a specified trial the widget's current trial, and draw it in
  // the OpenGL window. The widget's visibility is set to true.
  void show(WeakRef<Toglet> toglet, Ref<TrialBase> trial)
  {
    trial->installSelf(toglet);
    toglet->setVisibility(true);
    toglet->display();
  }

  class TogletPkg;
}

//---------------------------------------------------------------------
//
// TogletPkg class definition
//
//---------------------------------------------------------------------

class ObjTogl::TogletPkg : public Tcl::Pkg {
public:
  TogletPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Toglet", "$Revision$")
  {
    Tcl::defGenericObjCmds<Toglet>(this);

    def( "bind", "event_sequence binding_script", &Toglet::bind );
    def( "currentToglet", "toglet_id", &ObjTogl::setCurrentTogl );
    def( "currentToglet", 0, &ObjTogl::getCurrentTogl );
    def( "defaultParent", "parent", &Toglet::defaultParent );
    def( "dumpCmap", "toglet_id start_index end_index", &ObjTogl::dumpCmap );
    def( "dumpCmap", "toglet_id", &ObjTogl::dumpCmapAll );
    def( "Togl::inited", 0, &ObjTogl::inited );
    def( "see", "gxnode_id", &ObjTogl::see );
    def( "setColor", "index r g b", &ObjTogl::setColor );
    def( "setCurTrial", "toglet_id widget_id", &ObjTogl::setCurTrial );
    def( "setMinRect", "left top right bottom", &ObjTogl::setMinRect );
    def( "show", "toglet_id trial_id", &ObjTogl::show );

    defAction("clearscreen", &Toglet::clearscreen);
    defAction("destroy", &Toglet::destroyWidget);
    defSetter("dumpEps", "item_id(s) filename", &Toglet::writeEpsFile);
    defAttrib("height", &Toglet::getHeight, &Toglet::setHeight);
    defSetter("hold", "item_id(s) hold_on?", &Toglet::setHold);
    defAction("loadDefaultFont", &Toglet::loadDefaultFont);
    defSetter("loadFont", &Toglet::loadFont);
    defSetter("loadFonti", &Toglet::loadFonti);
    defGetter("pathname", &Toglet::pathname);
    defAction("refresh", &Toglet::refresh);
    defSetter("scaleRect", "item_id(s) scale", &Toglet::scaleRect);
    defSetter("setFixedScale", "item_id(s) scale", &Toglet::setFixedScale);
    defSetter("setUnitAngle", "item_id(s) angle_in_degrees",
              &Toglet::setUnitAngle);
    defSetter("setViewingDistance", "item_id(s) distance_in_inches",
              &Toglet::setViewingDistIn);
    defSetter("setVisible", "item_id(s) visibility", &Toglet::setVisibility);
    defAction("swapBuffers", &Toglet::swapBuffers);
    defAction("takeFocus", &Toglet::takeFocus);
    defAction("undraw", &Toglet::undraw);
    defGetter("usingFixedScale", &Toglet::usingFixedScale);
    defAttrib("width", &Toglet::getWidth, &Toglet::setWidth);

    defAction("hook", &Tcl::TkWidget::hook);

    setCurrentTogl(WeakRef<Toglet>(Toglet::make(interp)));

    Pkg::eval("namespace eval Togl { proc init {} {} }\n"
              "proc clearscreen {} { Togl::clearscreen }\n"
              "proc see {id} { Togl::see $id }\n"
              "proc show {id} { Togl::show $id }\n"
              "proc undraw {} { Togl::undraw }\n"
              "proc redraw {} { Togl::refresh }\n");

    Pkg::eval("foreach cmd [info commands ::Toglet::*] {"
              "  proc ::Togl::[namespace tail $cmd] {args} \" eval $cmd \\[Toglet::currentToglet\\] \\$args \" }\n"
              "namespace eval Togl { namespace export * }"
              );

    Pkg::eval("Expt::widget [Toglet::currentToglet]");
  }

  virtual ~TogletPkg()
  {
    if (ObjTogl::theWidget.isValid())
      {
        try
          {
            ObjTogl::theWidget->setVisibility(false);
          }
        catch (...) { /* nothing */ }
      }
  }
};

//---------------------------------------------------------------------
//
// Objtogl_Init
//
//---------------------------------------------------------------------

namespace
{
  Tcl_Interp* toglCreateInterp = 0;

  Toglet* makeToglet()
  {
    if (toglCreateInterp == 0)
      FactoryError::throwForType("Toglet");

    return Toglet::make(toglCreateInterp);
  }
}

extern "C"
int Objtogl_Init(Tcl_Interp* interp)
{
DOTRACE("Objtogl_Init");

  Tcl::Pkg* pkg = new ObjTogl::TogletPkg(interp);

  toglCreateInterp = interp;

  Util::ObjFactory::theOne().registerCreatorFunc( makeToglet );

  return pkg->initStatus();
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
