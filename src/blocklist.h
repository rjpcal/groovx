///////////////////////////////////////////////////////////////////////
//
// blocklist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:16 1999
// written: Wed Feb 16 07:45:30 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_H_DEFINED
#define BLOCKLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

class Block;

///////////////////////////////////////////////////////////////////////
/**
 *
 * BlockList is a singleton wrapper for \c PtrList<Block>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BlockList : public PtrList<Block> {
private:
  typedef PtrList<Block> Base;

protected:
  /// Construct and reserve space for \a size objects.
  BlockList(int size);

public:
  /// Returns a reference to the singleton instance.
  static BlockList& theBlockList();
};

static const char vcid_blocklist_h[] = "$Header$";
#endif // !BLOCKLIST_H_DEFINED
