///////////////////////////////////////////////////////////////////////
//
// bitmapcachenode.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:19:59 2001
// written: Wed Nov 13 10:13:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPCACHENODE_H_DEFINED
#define BITMAPCACHENODE_H_DEFINED

#include "visx/gmodes.h"
#include "visx/gnode.h"

#include "util/strings.h"
#include "util/pointers.h"

class BmapRenderer;

namespace Gfx { class BmapData; }

class BitmapCacheNode : public Gnode
{
public:
  BitmapCacheNode(Util::SoftRef<Gnode> child);
  virtual ~BitmapCacheNode();

  static fstring BITMAP_CACHE_DIR;

  Gmodes::RenderMode getMode() const { return itsMode; }

  void setMode(Gmodes::RenderMode new_mode);

  const fstring& getCacheFilename() const { return itsCacheFilename; }

  void setCacheFilename(const fstring& name);

  void invalidate();

  void saveBitmapCache(Gfx::Canvas& canvas, const char* filename) const;

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& canvas) const;

private:
  Gmodes::RenderMode itsMode;

  mutable fstring itsCacheFilename;

  mutable shared_ptr<BmapRenderer> itsBmapRenderer;
  mutable shared_ptr<Gfx::BmapData> itsBmapData;

  fstring fullCacheFilename() const
    {
      fstring result = BITMAP_CACHE_DIR;
      result.append( "/" );
      result.append( itsCacheFilename );
      return result;
    }

  void recacheBitmap(Gfx::Canvas& canvas) const;
};

static const char vcid_bitmapcachenode_h[] = "$Header$";
#endif // !BITMAPCACHENODE_H_DEFINED
