///////////////////////////////////////////////////////////////////////
//
// grobjrenderer.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:19:59 2001
// written: Fri Aug 10 14:41:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJRENDERER_H_DEFINED
#define GROBJRENDERER_H_DEFINED

#include "gmodes.h"
#include "gnode.h"

#include "util/strings.h"
#include "util/pointers.h"

class BmapRenderer;
class BitmapRep;

class Gnode;

class GrObjRenderer {
public:
  GrObjRenderer();
  virtual ~GrObjRenderer();

  static fstring BITMAP_CACHE_DIR;

  void callList() const;

  void setMode(Gmodes::RenderMode new_mode);

  Gmodes::RenderMode getMode() const { return itsMode; }

  const fstring& getCacheFilename() const { return itsCacheFilename; }

  void invalidate();

  void setCacheFilename(const fstring& name);

  // Returns true if the object was rendered to the screen as part
  // of the update, false otherwise
  bool update(const GrObjImpl* obj, Gfx::Canvas& canvas) const;

  void render(const Gnode* node, Gfx::Canvas& canvas) const;

  void unrender(const Gnode* obj, Gfx::Canvas& canvas) const;

  void saveBitmapCache(const GrObjImpl* obj, Gfx::Canvas& canvas,
                       const char* filename) const;

  Gmodes::RenderMode getUnMode() const { return itsUnMode; }
  void setUnMode(Gmodes::RenderMode new_val) { itsUnMode = new_val; }

private:
  Gmodes::RenderMode itsMode;
  Gmodes::RenderMode itsUnMode;

  mutable fstring itsCacheFilename;

  mutable int itsDisplayList;   // OpenGL display list that draws the object
  mutable shared_ptr<BitmapRep> itsBitmapCache;

  fstring fullCacheFilename() const
    {
      fstring result = BITMAP_CACHE_DIR;
      result += "/";
      result += itsCacheFilename;
      return result;
    }

  // This function updates the cached OpenGL display list.
  void recompileIfNeeded(const Gnode* node, Gfx::Canvas& canvas) const;

  // This function updates the cached bitmap, and returns a true if
  // the bitmap was actually recached, and false if nothing was done.
  bool recacheBitmapIfNeeded(const GrObjImpl* obj, Gfx::Canvas& canvas) const;
};

static const char vcid_grobjrenderer_h[] = "$Header$";
#endif // !GROBJRENDERER_H_DEFINED
