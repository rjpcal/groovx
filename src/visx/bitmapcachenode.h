///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:19:59 2001
// written: Tue Aug 14 12:59:25 2001
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

class BitmapCacheNode : public Gnode {
public:
  BitmapCacheNode(shared_ptr<Gnode> child);
  virtual ~BitmapCacheNode();

  static fstring BITMAP_CACHE_DIR;

  Gmodes::RenderMode getMode() const { return itsMode; }

  void setMode(Gmodes::RenderMode new_mode);

  const fstring& getCacheFilename() const { return itsCacheFilename; }

  void setCacheFilename(const fstring& name);

  void invalidate();

  void saveBitmapCache(Gfx::Canvas& canvas, const char* filename) const;

  virtual void gnodeDraw(Gfx::Canvas& canvas) const;

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

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

  void recacheBitmap(Gfx::Canvas& canvas) const;
};

static const char vcid_bitmapcachenode_h[] = "$Header$";
#endif // !BITMAPCACHENODE_H_DEFINED
