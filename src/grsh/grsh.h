///////////////////////////////////////////////////////////////////////
//
// grsh.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 11 14:44:39 2002
// written: Wed Sep 11 14:50:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRSH_H_DEFINED
#define GRSH_H_DEFINED

namespace Gfx
{
  class Canvas;
}

/// Top-level (i.e. global) info about the application.
namespace Grsh
{
  /// Get the application's number of command-line arguments.
  int argc();

  /// Get the application's command-line arguments.
  char** argv();

  /// Get the library directory specified by the relevant environment variable.
  const char* libraryDirectory();

  /// Set the current Canvas.
  void installCanvas(Gfx::Canvas& canvas);

  /// Get the current Canvas.
  Gfx::Canvas& canvas();
}

static const char vcid_grsh_h[] = "$Header$";
#endif // !GRSH_H_DEFINED
