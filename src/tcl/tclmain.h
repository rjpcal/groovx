///////////////////////////////////////////////////////////////////////
//
// tclmain.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:32:01 2002
// written: Mon Jul 22 18:33:27 2002
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

  Tcl_Interp* interp() const;

  Tcl::Interp& safeInterp() const;

  void run();

private:
  Main(const Main&);
  Main& operator=(const Main&);

  struct Impl;
  friend struct Impl;
  Impl* const rep;
};

static const char vcid_tclmain_h[] = "$Header$";
#endif // !TCLMAIN_H_DEFINED
