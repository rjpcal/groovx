///////////////////////////////////////////////////////////////////////
//
// iditemutils.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Mon Dec 11 17:46:47 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEMUTILS_CC_DEFINED
#define IDITEMUTILS_CC_DEFINED

#include "io/io.h"

#include "io/iditemutils.h"

#include "io/iodb.h"

bool IdItemUtils::isValidId(int id) {
  return IoDb::theDb().isValidId(id);
}

void IdItemUtils::insertItem(IO::IoObject* obj) {
  IoDb::theDb().insertPtrBase(obj);
}

IO::IoObject* IdItemUtils::getCheckedItem(int id) {
  return IoDb::theDb().getCheckedPtrBase(id);
}

static const char vcid_iditemutils_cc[] = "$Header$";
#endif // !IDITEMUTILS_CC_DEFINED
