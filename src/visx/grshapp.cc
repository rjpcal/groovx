///////////////////////////////////////////////////////////////////////
//
// grshapp.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:26:59 1999
// written: Thu Jun  1 13:51:18 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_CC_DEFINED
#define GRSHAPP_CC_DEFINED

#include "grshapp.h"

#include "util/trace.h"

NoExptError::NoExptError() : ErrorWithMsg() {}

NoExptError::NoExptError(const char* msg) : ErrorWithMsg(msg) {}

NoExptError::~NoExptError() {}

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
