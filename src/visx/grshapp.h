///////////////////////////////////////////////////////////////////////
//
// grshapp.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:26:58 1999
// written: Tue Dec  7 11:35:45 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_H_DEFINED
#define GRSHAPP_H_DEFINED

#ifndef APPLICATION_H_DEFINED
#include "application.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

struct Tcl_Interp;

class NoExptError : public ErrorWithMsg {
public:
  NoExptError(const string& msg = "") : ErrorWithMsg(msg) {}
};


class GrshApp : public Application {
public:
  GrshApp(Tcl_Interp* interp);
  virtual ~GrshApp();

  Tcl_Interp* getInterp();

  void installExperiment(Experiment* expt);
  virtual Experiment* getExperiment();

private:
  Tcl_Interp* itsInterp;
  Experiment* itsExpt;
};

static const char vcid_grshapp_h[] = "$Header$";
#endif // !GRSHAPP_H_DEFINED
