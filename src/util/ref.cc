///////////////////////////////////////////////////////////////////////
//
// iditem.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Mon Jun 11 14:41:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "util/iditem.h"

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

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
