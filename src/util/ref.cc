///////////////////////////////////////////////////////////////////////
//
// ref.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Tue Jun 12 11:13:59 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_CC_DEFINED
#define REF_CC_DEFINED

#include "util/ref.h"

#include "util/objdb.h"

bool Util::RefHelper::isValidId(Util::UID id) {
  return ObjDb::theDb().isValidId(id);
}

Util::Object* Util::RefHelper::getCheckedItem(Util::UID id) {
  return ObjDb::theDb().getCheckedPtrBase(id);
}

void Util::RefHelper::insertItem(Util::Object* obj) {
  ObjDb::theDb().insertPtrBase(obj);
}

static const char vcid_ref_cc[] = "$Header$";
#endif // !REF_CC_DEFINED
