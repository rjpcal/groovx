///////////////////////////////////////////////////////////////////////
//
// iomgr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Apr 23 01:13:16 1999
// written: Thu May 17 16:40:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOMGR_CC_DEFINED
#define IOMGR_CC_DEFINED

#include "io/iomgr.h"

#include "io/iofactory.h"

#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"

IdItem<IO::IoObject> IO::IoMgr::newIO(const char* type) {
  return newIO(fixed_string(type));
}

IdItem<IO::IoObject> IO::IoMgr::newIO(const fixed_string& type) {
DOTRACE("IO::IoMgr::newIO(const fixed_string&)");
  return IdItem<IO::IoObject>(IO::IoFactory::theOne().newCheckedObject(type));
}

static const char vcid_iomgr_cc[] = "$Header$";
#endif // !IOMGR_CC_DEFINED
