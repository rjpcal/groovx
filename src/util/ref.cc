///////////////////////////////////////////////////////////////////////
//
// iditem.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Sat Jun  2 15:44:27 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_CC_DEFINED
#define IDITEM_CC_DEFINED

#include "io/iditem.h"

#include "io/iodb.h"

bool IdItemUtils::isValidId(IO::UID id) {
  return IoDb::theDb().isValidId(id);
}

IO::IoObject* IdItemUtils::getCheckedItem(IO::UID id) {
  return IoDb::theDb().getCheckedPtrBase(id);
}

void IdItemUtils::insertItem(IO::IoObject* obj) {
  IoDb::theDb().insertPtrBase(obj);
}

static const char vcid_iditem_cc[] = "$Header$";
#endif // !IDITEM_CC_DEFINED
