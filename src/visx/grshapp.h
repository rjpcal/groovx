///////////////////////////////////////////////////////////////////////
//
// grshapp.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:26:58 1999
// written: Wed Aug  8 12:50:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_H_DEFINED
#define GRSHAPP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(APPLICATION_H_DEFINED)
#include "application.h"
#endif

struct Tcl_Interp;


class GrshApp : public Application {
public:
  /// Construct with the applicaton's Tcl interpreter.
  GrshApp(int argc, char** argv, Tcl_Interp* interp);

  /// Virtual destructor.
  virtual ~GrshApp();

  /// Returns the application's Tcl interpreter.
  Tcl_Interp* getInterp();

  /// Installs \a canavs as the application's \c Canvas.
  void installCanvas(GWT::Canvas& canvas);

  /// Returns the application's \c Canvas.
  virtual GWT::Canvas& getCanvas();

private:
  GrshApp(const GrshApp&);
  GrshApp& operator=(const GrshApp&);

  Tcl_Interp* itsInterp;
  GWT::Canvas* itsCanvas;
};

static const char vcid_grshapp_h[] = "$Header$";
#endif // !GRSHAPP_H_DEFINED
