///////////////////////////////////////////////////////////////////////
//
// blocklist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:16 1999
// written: Fri Oct 27 17:35:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_H_DEFINED
#define BLOCKLIST_H_DEFINED

class IoPtrList;

namespace BlockList {
  /// Returns a reference to the singleton instance.
  IoPtrList& theBlockList();
}

static const char vcid_blocklist_h[] = "$Header$";
#endif // !BLOCKLIST_H_DEFINED
