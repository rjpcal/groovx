///////////////////////////////////////////////////////////////////////
//
// blocklist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:16 1999
// written: Sat Nov 20 20:30:01 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_H_DEFINED
#define BLOCKLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

class Block;

/// A singleton wrapper of PtrList<Block>.
class BlockList : public PtrList<Block> {
private:
  typedef PtrList<Block> Base;
protected:
  BlockList(int size);
public:
  /// Returns the singleton instance.
  static BlockList& theBlockList();
};

static const char vcid_blocklist_h[] = "$Header$";
#endif // !BLOCKLIST_H_DEFINED
