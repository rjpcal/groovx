///////////////////////////////////////////////////////////////////////
//
// objmgr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Apr 23 01:13:16 1999
// written: Tue Aug 21 15:22:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJMGR_CC_DEFINED
#define OBJMGR_CC_DEFINED

#include "util/objmgr.h"

#include "util/objfactory.h"
#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"

SoftRef<Util::Object> Util::ObjMgr::newObj(const char* type)
{
  return newObj(fstring(type));
}

SoftRef<Util::Object> Util::ObjMgr::newObj(const fstring& type)
{
DOTRACE("Util::ObjMgr::newObj(const fstring&)");
  return SoftRef<Util::Object>(Util::ObjFactory::theOne().newCheckedObject(type));
}

static const char vcid_objmgr_cc[] = "$Header$";
#endif // !OBJMGR_CC_DEFINED
