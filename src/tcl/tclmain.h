///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:32:01 2002
// written: Mon Jul 22 16:34:38 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_H_DEFINED
#define TCLMAIN_H_DEFINED

#include <tcl.h>

namespace Tcl
{
  class Main;
}

class Tcl::Main
{
public:
  Main(int argc, char** argv);

  void run();

  Tcl_Interp* interp() const { return itsInterp; }

private:
  void defaultPrompt(bool partial);
  void prompt(bool partial);

  static void stdinProc(ClientData clientData, int /*mask*/);

  Tcl_Interp* itsInterp;
  const char* itsStartupFileName;
  Tcl_Channel itsInChannel;
};

static const char vcid_tclmain_h[] = "$Header$";
#endif // !TCLMAIN_H_DEFINED
