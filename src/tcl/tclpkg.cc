///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Wed Mar 19 12:45:43 2003
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
