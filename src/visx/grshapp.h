///////////////////////////////////////////////////////////////////////
//
// grshapp.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Dec  7 11:26:58 1999
// written: Fri Feb 18 22:52:08 2000
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

/**
 *
 * This exception class will be thrown by \c GrshApp::getExperiment()
 * if no one has installed an \c Experiment.
 *
 **/
class NoExptError : public ErrorWithMsg {
public:
  /// Construct with an appropriate message.
  NoExptError(const string& msg = "") : ErrorWithMsg(msg) {}
};


class GrshApp : public Application {
public:
  /// Construct with the applicaton's Tcl interpreter.
  GrshApp(Tcl_Interp* interp);

  /// Virtual destructor.
  virtual ~GrshApp();

  /// Returns the application's Tcl interpreter.
  Tcl_Interp* getInterp();

  /// Installs \a expt as the application's \c Experiment.
  void installExperiment(Experiment* expt);

  /// Returns the application's \c Experiment.
  virtual Experiment* getExperiment();

private:
  Tcl_Interp* itsInterp;
  Experiment* itsExpt;
};

static const char vcid_grshapp_h[] = "$Header$";
#endif // !GRSHAPP_H_DEFINED
