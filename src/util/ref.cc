///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Wed Mar 19 12:45:37 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_CC_DEFINED
#define REF_CC_DEFINED

#include "util/ref.h"

#include "util/error.h"
#include "util/objdb.h"

bool Util::RefHelper::isValidId(Util::UID id)
{
  return ObjDb::theDb().isValidId(id);
}

Util::Object* Util::RefHelper::getCheckedItem(Util::UID id)
{
  return ObjDb::theDb().getCheckedObj(id);
}

void Util::RefHelper::insertItem(Util::Object* obj)
{
  ObjDb::theDb().insertObj(obj);
}

void Util::RefHelper::insertItemWeak(Util::Object* obj)
{
  ObjDb::theDb().insertObjWeak(obj);
}

void Util::RefHelper::throwError(const char* msg)
{
  throw Util::Error(msg);
}

static const char vcid_ref_cc[] = "$Header$";
#endif // !REF_CC_DEFINED
