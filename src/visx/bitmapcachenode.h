///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:19:59 2001
// written: Fri Aug 10 14:41:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPCACHENODE_H_DEFINED
#define BITMAPCACHENODE_H_DEFINED

#include "gmodes.h"
#include "gnode.h"

#include "util/strings.h"
#include "util/pointers.h"

class BitmapRep;

class Gnode;

class BitmapCacheNode {
public:
  BitmapCacheNode();
  virtual ~BitmapCacheNode();

  static fstring BITMAP_CACHE_DIR;

  void setMode(Gmodes::RenderMode new_mode);

  Gmodes::RenderMode getMode() const { return itsMode; }

  const fstring& getCacheFilename() const { return itsCacheFilename; }

  void invalidate();

  void setCacheFilename(const fstring& name);

  void render(const Gnode* node, Gfx::Canvas& canvas) const;

  void unrender(const Gnode* node, Gfx::Canvas& canvas) const;

  void saveBitmapCache(const Gnode* node, Gfx::Canvas& canvas,
                       const char* filename) const;

private:
  Gmodes::RenderMode itsMode;

  mutable fstring itsCacheFilename;

  mutable shared_ptr<BitmapRep> itsBitmapRep;

  fstring fullCacheFilename() const
    {
      fstring result = BITMAP_CACHE_DIR;
      result += "/";
      result += itsCacheFilename;
      return result;
    }

  // This function updates the cached bitmap, and returns a true if
  // the bitmap was actually recached, and false if nothing was done.
  bool recacheBitmap(const Gnode* node, Gfx::Canvas& canvas) const;
};

static const char vcid_bitmapcachenode_h[] = "$Header$";
#endif // !BITMAPCACHENODE_H_DEFINED
