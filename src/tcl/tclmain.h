///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:32:01 2002
// written: Mon Jan 13 11:01:38 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_H_DEFINED
#define TCLMAIN_H_DEFINED

namespace Tcl
{
  class Interp;
  class Main;
}

class Tcl::Main
{
public:
  Main(int argc, char** argv);
  ~Main();

  static bool isInteractive();

  static Tcl::Interp& interp();

  static void run();

private:
  Main(const Main&);
  Main& operator=(const Main&);
};

static const char vcid_tclmain_h[] = "$Header$";
#endif // !TCLMAIN_H_DEFINED
