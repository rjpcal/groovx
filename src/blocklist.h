///////////////////////////////////////////////////////////////////////
//
// blocklist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:22:16 1999
// written: Mon May 15 19:33:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BLOCKLIST_H_DEFINED
#define BLOCKLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
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

  static BlockList theInstance;

protected:
  /// Construct and reserve space for \a size objects.
  BlockList(int size);

public:
  /// Returns a reference to the singleton instance.
  static BlockList& theBlockList();
};

static const char vcid_blocklist_h[] = "$Header$";
#endif // !BLOCKLIST_H_DEFINED
