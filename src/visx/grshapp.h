///////////////////////////////////////////////////////////////////////
//
// grshapp.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:26:58 1999
// written: Fri Nov 10 17:03:53 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_H_DEFINED
#define GRSHAPP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(APPLICATION_H_DEFINED)
#include "application.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
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
  /// Default constructor.
  NoExptError();
  /// Construct with an appropriate message.
  NoExptError(const char* msg);
  /// Virtual destructor.
  virtual ~NoExptError();
};


class GrshApp : public Application {
public:
  /// Construct with the applicaton's Tcl interpreter.
  GrshApp(int argc, char** argv, Tcl_Interp* interp);

  /// Virtual destructor.
  virtual ~GrshApp();

  /// Returns the application's Tcl interpreter.
  Tcl_Interp* getInterp();

  /// Installs \a expt as the application's \c Experiment.
  void installExperiment(Experiment* expt);

  /// Returns the application's \c Experiment.
  virtual Experiment* getExperiment();

private:
  GrshApp(const GrshApp&);
  GrshApp& operator=(const GrshApp&);

  Tcl_Interp* itsInterp;
  Experiment* itsExpt;
};

static const char vcid_grshapp_h[] = "$Header$";
#endif // !GRSHAPP_H_DEFINED
