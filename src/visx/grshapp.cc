///////////////////////////////////////////////////////////////////////
//
// grshapp.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:26:59 1999
// written: Fri Jan 18 16:07:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_CC_DEFINED
#define GRSHAPP_CC_DEFINED

#include "visx/grshapp.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const char* GRSH_LIB_DIR = "GRSH_LIB_DIR";
}

GrshApp::GrshApp(int argc, char** argv, Tcl_Interp* interp) :
  Application(argc, argv, GRSH_LIB_DIR),
  itsInterp(interp),
  itsCanvas(0)
{
DOTRACE("GrshApp::GrshApp");
  Application::installApp(this);
}

GrshApp::~GrshApp()
{
DOTRACE("GrshApp::~GrshApp");
}

Tcl_Interp* GrshApp::getInterp()
{
DOTRACE("GrshApp::getInterp");
  return itsInterp;
}

void GrshApp::installCanvas(Gfx::Canvas& canvas)
{
DOTRACE("GrshApp::installCanvas");
  itsCanvas = &canvas;
  Assert(itsCanvas != 0);
}

Gfx::Canvas& GrshApp::getCanvas()
{
DOTRACE("GrshApp::getCanvas");
  if (itsCanvas == 0)
    {
      throw Util::Error("no canvas has yet been installed");
    }
  return *itsCanvas;
}

static const char vcid_grshapp_cc[] = "$Header$";
#endif // !GRSHAPP_CC_DEFINED
