///////////////////////////////////////////////////////////////////////
//
// toglettcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  2 08:00:00 1998 (as objtogl.cc)
// written: Thu Nov 21 15:06:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOGLETTCL_CC_DEFINED
#define TOGLETTCL_CC_DEFINED

#include "tcl/toglet.h"

#include "gfx/gxnode.h"

#include "tcl/tclerror.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"
#include "util/ref.h"

#include "visx/trialbase.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TogletTcl namespace declarations
//
///////////////////////////////////////////////////////////////////////

namespace TogletTcl
{
  SoftRef<Toglet> theWidget;

  void setCurrentTogl(SoftRef<Toglet> toglet)
  {
    DOTRACE("TogletTcl::setCurrentTogl");

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
        Toglet::Color color = toglet->queryColor(i);
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
    return TogletTcl::theWidget.isValid();
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

  class TogletPkg;
}

//---------------------------------------------------------------------
//
// TogletPkg class definition
//
//---------------------------------------------------------------------

class TogletTcl::TogletPkg : public Tcl::Pkg
{
public:
  TogletPkg(Tcl_Interp* interp) :
    Tcl::Pkg(interp, "Toglet", "$Revision$")
  {
    Tcl::defGenericObjCmds<Toglet>(this);

    def( "bind", "event_sequence binding_script", &Toglet::bind );
    def( "currentToglet", "toglet_id", &TogletTcl::setCurrentTogl );
    def( "currentToglet", 0, &TogletTcl::getCurrentTogl );
    def( "defaultParent", "parent", &Toglet::defaultParent );
    def( "dumpCmap", "toglet_id start_index end_index", &TogletTcl::dumpCmap );
    def( "dumpCmap", "toglet_id", &TogletTcl::dumpCmapAll );
    def( "inited", 0, &TogletTcl::inited );
    def( "perspective", "toglet_id fovy zNear zFar", &Toglet::setPerspective );
    def( "see", "gxnode_id", &TogletTcl::see );
    def( "setCurTrial", "toglet_id widget_id", &TogletTcl::setCurTrial );
    def( "show", "toglet_id trial_id", &TogletTcl::show );

    defSetter("allowRefresh", &Toglet::allowRefresh);
    defSetter("animate", "item_id(s) frames_per_second", &Toglet::animate);
    defAction("clearscreen", &Toglet::fullClearscreen);
    defAction("destroy", &Tcl::TkWidget::destroyWidget);
    def("fixedRect", "uid left top right bottom", &Toglet::setFixedRectLTRB);
    defAttrib("height", &Tcl::TkWidget::height, &Tcl::TkWidget::setHeight);
    defSetter("hold", "item_id(s) hold_on?", &Toglet::setHold);
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
    defAction("swapBuffers", &Toglet::swapBuffers);
    defAction("takeFocus", &Toglet::takeFocus);
    defAttrib("timeOut", &Tcl::TkWidget::timeOut, &Tcl::TkWidget::setTimeOut);
    defAction("undraw", &Toglet::undraw);
    defGetter("usingFixedScale", &Toglet::usingFixedScale);
    defAttrib("width", &Tcl::TkWidget::width, &Tcl::TkWidget::setWidth);

    defAction("hook", &Tcl::TkWidget::hook);

    setCurrentTogl(SoftRef<Toglet>(Toglet::make()));

    Pkg::eval("proc clearscreen {} { Togl::clearscreen }\n"
              "proc see {id} { Togl::see $id }\n"
              "proc undraw {} { Togl::undraw }\n");

    Pkg::eval("foreach cmd [info commands ::Toglet::*] {\n"
              "  namespace eval ::Togl {\n"
              "    proc [namespace tail $cmd] {args} \" eval $cmd \\[Toglet::currentToglet\\] \\$args \""
              "  }\n"
              "}\n"
              "namespace eval ::Togl { namespace export * }"
              );

    Pkg::eval("Expt::widget [Toglet::currentToglet]");
  }

  virtual ~TogletPkg()
  {
    if (TogletTcl::theWidget.isValid())
      {
        try
          {
            TogletTcl::theWidget->setVisibility(false);
          }
        catch (...) { /* nothing */ }
      }
  }
};

//---------------------------------------------------------------------
//
// Toglet_Init
//
//---------------------------------------------------------------------

extern "C"
int Toglet_Init(Tcl_Interp* interp)
{
DOTRACE("Toglet_Init");

  Tcl::Pkg* pkg = new TogletTcl::TogletPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc( &Toglet::make );

  return pkg->initStatus();
}

static const char vcid_toglettcl_cc[] = "$Header$";
#endif // !TOGLETTCL_CC_DEFINED
