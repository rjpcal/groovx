///////////////////////////////////////////////////////////////////////
//
// iditemutils.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Oct 27 17:07:31 2000
// written: Fri Oct 27 18:45:17 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEMUTILS_CC_DEFINED
#define IDITEMUTILS_CC_DEFINED

#include "ioptrlist.h"

namespace IdItemUtils {
  bool isValidId(int id);
  void insertItem(IO::IoObject* obj);
  IO::IoObject* getCheckedItem(int id);
}

bool IdItemUtils::isValidId(int id) {
  return IoPtrList::theList().isValidId(id);
}

void IdItemUtils::insertItem(IO::IoObject* obj) {
  IoPtrList::theList().insertPtrBase(obj);
}

IO::IoObject* IdItemUtils::getCheckedItem(int id) {
  return IoPtrList::theList().getCheckedPtrBase(id);
}

static const char vcid_iditemutils_cc[] = "$Header$";
#endif // !IDITEMUTILS_CC_DEFINED
