///////////////////////////////////////////////////////////////////////
//
// grshapp.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:26:59 1999
// written: Fri Mar 10 01:29:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_CC_DEFINED
#define GRSHAPP_CC_DEFINED

#include "grshapp.h"

#include "trace.h"

GrshApp::GrshApp(Tcl_Interp* interp) :
  itsInterp(interp),
  itsExpt(0)
{
DOTRACE("GrshApp::GrshApp");
  Application::installApp(this);
}

GrshApp::~GrshApp() {
DOTRACE("GrshApp::~GrshApp");
}

Tcl_Interp* GrshApp::getInterp() {
DOTRACE("GrshApp::getInterp");
  return itsInterp;
}

void GrshApp::installExperiment(Experiment* expt) {
DOTRACE("GrshApp::installExperiment");
  if (expt != 0) {
	 itsExpt = expt;
  }
}

Experiment* GrshApp::getExperiment() {
DOTRACE("GrshApp::getExperiment");
  if (itsExpt == 0) {
	 throw NoExptError("no experiment has yet been installed");
  }
  return itsExpt;
}

static const char vcid_grshapp_cc[] = "$Header$";
#endif // !GRSHAPP_CC_DEFINED
