///////////////////////////////////////////////////////////////////////
//
// grshapp.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  7 11:26:58 1999
// written: Wed Sep 11 14:21:18 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSHAPP_H_DEFINED
#define GRSHAPP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(APPLICATION_H_DEFINED)
#include "visx/application.h"
#endif


/// Application class for grsh; holds a reference to the current Gfx::Canvas.
class GrshApp : public Application
{
public:
  /// Construct with the applicaton's Tcl interpreter.
  GrshApp(int argc, char** argv);

  /// Virtual destructor.
  virtual ~GrshApp();

  /// Installs \a canavs as the application's \c Canvas.
  void installCanvas(Gfx::Canvas& canvas);

  /// Returns the application's \c Canvas.
  virtual Gfx::Canvas& getCanvas();

private:
  GrshApp(const GrshApp&);
  GrshApp& operator=(const GrshApp&);

  Gfx::Canvas* itsCanvas;
};

static const char vcid_grshapp_h[] = "$Header$";
#endif // !GRSHAPP_H_DEFINED
