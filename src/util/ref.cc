///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Wed Aug  8 12:27:56 2001
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
  return ObjDb::theDb().getCheckedPtrBase(id);
}

void Util::RefHelper::insertItem(Util::Object* obj)
{
  ObjDb::theDb().insertPtrBase(obj);
}

void Util::RefHelper::throwError(const char* msg)
{
  throw Util::Error(msg);
}

static const char vcid_ref_cc[] = "$Header$";
#endif // !REF_CC_DEFINED
