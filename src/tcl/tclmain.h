///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:32:01 2002
// written: Tue Dec 10 13:25:42 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_H_DEFINED
#define TCLMAIN_H_DEFINED

struct Tcl_Interp;

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
