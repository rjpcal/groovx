///////////////////////////////////////////////////////////////////////
//
// iditem.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Sat Jun  9 14:23:59 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "util/iditem.h"

#include "util/objdb.h"

bool IdItemUtils::isValidId(Util::UID id) {
  return ObjDb::theDb().isValidId(id);
}

Util::Object* IdItemUtils::getCheckedItem(Util::UID id) {
  return ObjDb::theDb().getCheckedPtrBase(id);
}

void IdItemUtils::insertItem(Util::Object* obj) {
  ObjDb::theDb().insertPtrBase(obj);
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
