///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Fri Nov 22 17:10:04 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Pkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Pkg::Pkg(Tcl_Interp* interp, const char* name, const char* version) :
  Tcl::PkgBase(interp, name, version)
{}

Tcl::Pkg::~Pkg() {}

const char* Tcl::Pkg::actionUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s)";
}

const char* Tcl::Pkg::getterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s)";
}

const char* Tcl::Pkg::setterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s) new_value(s)";
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
