///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Oct 30 10:00:39 2000
// written: Wed Mar 19 12:45:46 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "tcl/iotcl.h"

#include "io/io.h"
#include "io/ioutil.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/strings.h"

#include "util/trace.h"

void Tcl::defIoCommands(Tcl::Pkg* pkg)
{
DOTRACE("Tcl::defIoCommands");
  pkg->defVec( "stringify", "item_id(s)", IO::stringify );
  pkg->defVec( "destringify", "item_id(s) string(s)", IO::destringify );

  pkg->defVec( "write", "item_id(s)", IO::write );
  pkg->defVec( "read", "item_id(s) string(s)", IO::read );

  pkg->def( "save", "item_id filename", IO::saveASW );
  pkg->def( "load", "item_id filename", IO::loadASR );
  pkg->def( "retrieve", "filename", IO::retrieveASR );
}

extern "C"
int Io_Init(Tcl_Interp* interp)
{
DOTRACE("Io_Init");

  //
  // IO package
  //

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "IO", "$Revision$");

  Tcl::defGenericObjCmds<IO::IoObject>(pkg);
  Tcl::defIoCommands(pkg);
  pkg->defGetter("type", &IO::IoObject::ioTypename);

  pkg->eval("proc ::-> {args} {\n"
            "set ids [lindex $args 0]\n"
            "set namesp [IO::type [lindex $ids 0]]\n"
            "set cmd [lreplace $args 0 1 [lindex $args 1] $ids]\n"
            "namespace eval $namesp $cmd\n"
            "}");

  return pkg->initStatus();
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
