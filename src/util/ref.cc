///////////////////////////////////////////////////////////////////////
//
// iditemutils.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Oct 27 17:07:31 2000
// written: Sat May 19 11:14:16 2001
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

IO::IoObject* IdItemUtils::getCheckedItem(int id) {
  return IoDb::theDb().getCheckedPtrBase(id);
}

static const char vcid_iditemutils_cc[] = "$Header$";
#endif // !IDITEMUTILS_CC_DEFINED
