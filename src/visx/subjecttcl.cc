///////////////////////////////////////////////////////////////////////
//
// subjecttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Mon Jul 16 09:43:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SUBJECTTCL_CC_DEFINED
#define SUBJECTTCL_CC_DEFINED

#include "subject.h"

#include "tcl/genericobjpkg.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace SubjectTcl {
  class SubjectPkg;
}

class SubjectTcl::SubjectPkg : public Tcl::GenericObjPkg<Subject> {
public:
  SubjectPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<Subject>(interp, "Subject", "$Revision$")
  {
    defAttrib("name", &Subject::getName, &Subject::setName);
    defAttrib("directory", &Subject::getDirectory,
              &Subject::setDirectory);
  }
};

extern "C"
int Subject_Init(Tcl_Interp* interp) {
DOTRACE("Subject_Init");

  Tcl::TclPkg* pkg = new SubjectTcl::SubjectPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Subject::make);

  return pkg->initStatus();
}

static const char vcid_subjecttcl_cc[] = "$Header$";
#endif // !SUBJECTTCL_CC_DEFINED
