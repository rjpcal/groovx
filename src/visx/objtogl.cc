///////////////////////////////////////////////////////////////////////
//
// objtogl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  2 08:00:00 1998
// written: Tue Sep 17 16:36:50 2002
// $Id$
//
// This package provides functionality that controlling the display,
// reshaping, etc. of a Togl widget.
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJTOGL_CC_DEFINED
#define OBJTOGL_CC_DEFINED

#include "visx/toglet.h"
#include "visx/trialbase.h"

#include "gfx/gxnode.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"
#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// ObjTogl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace ObjTogl
{
  SoftRef<Toglet> theWidget;

  void setCurrentTogl(SoftRef<Toglet> toglet)
  {
    DOTRACE("ObjTogl::setCurrentTogl");

    theWidget = toglet;

    toglet->makeCurrent();
  }

  SoftRef<Toglet> getCurrentTogl()
  {
    return theWidget;
  }

  Tcl::List dumpCmap(SoftRef<Toglet> toglet, unsigned int start, unsigned end)
  {
    if (start > 255 || end > 255)
      {
        throw Tcl::TclError("colormap index out of range");
      }

    Tcl::List result;

    for (unsigned int i = start; i <= end; ++i)
      {
        Togl::Color color = toglet->queryColor(i);
        Tcl::List color_list;
        color_list.append(i);
        color_list.append(color.red);
        color_list.append(color.green);
        color_list.append(color.blue);
        result.append(color_list);
      }

    return result;
  }

  Tcl::List dumpCmapAll(SoftRef<Toglet> toglet)
  {
    return dumpCmap(toglet, 0, 255);
  }

  bool inited()
  {
    return ObjTogl::theWidget.isValid();
  }

  // Make a specified GxNode the widget's current drawable, and draw
  // it in the OpenGL window. The widget's visibility is set to true.
  Util::UID see(SoftRef<GWT::Widget> widg, Ref<GxNode> item)
  {
    widg->setDrawable(item);
    widg->setVisibility(true);
    widg->fullRender();
    return item->id();
  }

  // Change the widget's current trial to a specified trial id. The
  // current trial is the one that will be displayed by a subsequent
  // call to "redraw", or by remap events sent to the screen
  // window. Returns an error if the specified trial id is not valid.
  void setCurTrial(SoftRef<Toglet> toglet, Ref<TrialBase> trial)
  {
    trial->installSelf(toglet);
  }

  // Make a specified trial the widget's current trial, and draw it in
  // the OpenGL window. The widget's visibility is set to true.
  void show(SoftRef<Toglet> toglet, Ref<TrialBase> trial)
  {
    trial->installSelf(toglet);
    toglet->setVisibility(true);
    toglet->fullRender();
  }

  Tcl_Obj* cget(SoftRef<Toglet> toglet)
  {
    return toglet->cget(0);
  }

  Tcl_Obj* cget2(SoftRef<Toglet> toglet, Tcl_Obj* param)
  {
    return toglet->cget(param);
  }

  void configure(SoftRef<Toglet> toglet, Tcl::List args)
  {
    toglet->configure(args.length(), args.elements());
  }

  class TogletPkg;
}

//---------------------------------------------------------------------
//
// TogletPkg class definition
//
//---------------------------------------------------------------------

class ObjTogl::TogletPkg : public Tcl::Pkg
{
public:
  TogletPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Toglet", "$Revision$")
  {
    Tcl::defGenericObjCmds<Toglet>(this);

    def( "bind", "event_sequence binding_script", &Toglet::bind );
    def( "cget", "toglet_id", &ObjTogl::cget );
    def( "cget", "toglet_id param_name", &ObjTogl::cget2 );
    def( "configure", "toglet_id {param1 value1 ?...?}", &ObjTogl::configure );
    def( "currentToglet", "toglet_id", &ObjTogl::setCurrentTogl );
    def( "currentToglet", 0, &ObjTogl::getCurrentTogl );
    def( "defaultParent", "parent", &Toglet::defaultParent );
    def( "dumpCmap", "toglet_id start_index end_index", &ObjTogl::dumpCmap );
    def( "dumpCmap", "toglet_id", &ObjTogl::dumpCmapAll );
    def( "Togl::inited", 0, &ObjTogl::inited );
    def( "perspective", "toglet_id fovy zNear zFar", &Toglet::setPerspective );
    def( "see", "gxnode_id", &ObjTogl::see );
    def( "setCurTrial", "toglet_id widget_id", &ObjTogl::setCurTrial );
    def( "show", "toglet_id trial_id", &ObjTogl::show );

    defSetter("allowRefresh", &Toglet::allowRefresh);
    defSetter("animate", "item_id(s) frames_per_second", &Toglet::animate);
    defAction("clearscreen", &Toglet::fullClearscreen);
    defAction("destroy", &Tcl::TkWidget::destroyWidget);
    defSetter("dumpEps", "item_id(s) filename", &Toglet::writeEpsFile);
    def("fixedRect", "uid left top right bottom", &Toglet::setFixedRectLTRB);
    defAttrib("height", &Tcl::TkWidget::height, &Tcl::TkWidget::setHeight);
    defSetter("hold", "item_id(s) hold_on?", &Toglet::setHold);
    defAction("loadDefaultFont", &Togl::loadDefaultFont);
    defSetter("loadFont", &Togl::loadBitmapFont);
    defSetter("loadFonti", &Togl::loadBitmapFonti);
    defGetter("pathname", &Tcl::TkWidget::pathname);
    defGetter("pixelsPerInch", &Tcl::TkWidget::pixelsPerInch);
    defSetter("scaleRect", "item_id(s) scale", &Toglet::scaleRect);
    defSetter("pixelsPerUnit", "item_id(s) scale", &Toglet::setPixelsPerUnit);
    def("setMinRect", "uid left top right bottom", &Toglet::setMinRectLTRB);
    defSetter("setUnitAngle", "item_id(s) angle_in_degrees",
              &Toglet::setUnitAngle);
    defSetter("setViewingDistance", "item_id(s) distance_in_inches",
              &Toglet::setViewingDistIn);
    defSetter("setVisible", "item_id(s) visibility", &Toglet::setVisibility);
    defAction("swapBuffers", &Togl::swapBuffers);
    defAction("takeFocus", &Togl::takeFocus);
    defAction("undraw", &Toglet::undraw);
    defGetter("usingFixedScale", &Toglet::usingFixedScale);
    defAttrib("width", &Tcl::TkWidget::width, &Tcl::TkWidget::setWidth);

    defAction("hook", &Tcl::TkWidget::hook);

    setCurrentTogl(SoftRef<Toglet>(Toglet::make()));

    Pkg::eval("namespace eval Togl { proc init {} {} }\n"
              "proc clearscreen {} { Togl::clearscreen }\n"
              "proc see {id} { Togl::see $id }\n"
              "proc undraw {} { Togl::undraw }\n");

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

extern "C"
int Objtogl_Init(Tcl_Interp* interp)
{
DOTRACE("Objtogl_Init");

  Tcl::Pkg* pkg = new ObjTogl::TogletPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::make );

  return pkg->initStatus();
}

static const char vcid_objtogl_cc[] = "$Header$";
#endif // !OBJTOGL_CC_DEFINED
