///////////////////////////////////////////////////////////////////////
//
// iditem.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Wed Jun  6 15:51:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "util/iditem.h"

#include "util/iodb.h"

bool IdItemUtils::isValidId(Util::UID id) {
  return IoDb::theDb().isValidId(id);
}

Util::Object* IdItemUtils::getCheckedItem(Util::UID id) {
  return IoDb::theDb().getCheckedPtrBase(id);
}

void IdItemUtils::insertItem(Util::Object* obj) {
  IoDb::theDb().insertPtrBase(obj);
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
