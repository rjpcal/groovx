///////////////////////////////////////////////////////////////////////
//
// subjecttcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Fri Jan 18 16:06:59 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECTTCL_CC_DEFINED
#define SUBJECTTCL_CC_DEFINED

#include "visx/subject.h"

#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Subject_Init(Tcl_Interp* interp)
{
DOTRACE("Subject_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Subject", "$Revision$");
  Tcl::defGenericObjCmds<Subject>(pkg);

  pkg->defAttrib("name", &Subject::getName, &Subject::setName);
  pkg->defAttrib("directory", &Subject::getDirectory, &Subject::setDirectory);

  Util::ObjFactory::theOne().registerCreatorFunc(&Subject::make);

  return pkg->initStatus();
}

static const char vcid_subjecttcl_cc[] = "$Header$";
#endif // !SUBJECTTCL_CC_DEFINED
