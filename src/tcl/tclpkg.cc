///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Mon Jul 16 09:32:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#ifdef ACC_COMPILER // aCC needs this to be happy with the header
#include "util/ref.h"
#endif

#include "tcl/tclitempkg.h"

#include "tcl/stringifycmd.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkg::TclItemPkg(Tcl_Interp* interp,
                            const char* name, const char* version) :
  TclPkg(interp, name, version)
{}

Tcl::TclItemPkg::~TclItemPkg() {}

const char* Tcl::TclItemPkg::actionUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::TclItemPkg::getterUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::TclItemPkg::setterUsage(const char* usage)
{
  return usage ? usage : "item_id(s) new_value(s)";
}


///////////////////////////////////////////////////////////////////////
//
// IO command definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::TclItemPkg::addIoCommands()
{
DOTRACE("Tcl::TclItemPkg::addIoCommands");
  addCommand( new StringifyCmd(interp(), makePkgCmdName("stringify")) );
  addCommand( new DestringifyCmd(interp(), makePkgCmdName("destringify")) );

  addCommand( new WriteCmd(interp(), makePkgCmdName("write")) );
  addCommand( new ReadCmd(interp(), makePkgCmdName("read")) );

  addCommand( new ASWSaveCmd(interp(), makePkgCmdName("save")) );
  addCommand( new ASRLoadCmd(interp(), makePkgCmdName("load")) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
