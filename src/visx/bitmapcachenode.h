///////////////////////////////////////////////////////////////////////
//
// grobjrenderer.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 11:19:59 2001
// written: Wed Aug  8 19:00:53 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJRENDERER_H_DEFINED
#define GROBJRENDERER_H_DEFINED

#include "grobj.h"

#include "util/strings.h"
#include "util/pointers.h"

class BmapRenderer;
class BitmapRep;

class GrObjRenderer {
public:
  GrObjRenderer();
  virtual ~GrObjRenderer();

  static fixed_string BITMAP_CACHE_DIR;

  void callList() const;

  void invalidate() const { itsIsCurrent = false; }

  void setMode(GrObj::RenderMode new_mode);

  GrObj::RenderMode getMode() const { return itsMode; }

  const fixed_string& getCacheFilename() const { return itsCacheFilename; }

  void setCacheFilename(const fixed_string& name)
    {
      itsCacheFilename = name;
      queueBitmapLoad();
    }

  // Returns true if the object was rendered to the screen as part
  // of the update, false otherwise
  bool update(const GrObjImpl* obj, GWT::Canvas& canvas) const;

  void render(const GrObjImpl* obj, GWT::Canvas& canvas) const;

  void unrender(const GrObjImpl* obj, GWT::Canvas& canvas) const;

  void saveBitmapCache(const GrObjImpl* obj, GWT::Canvas& canvas,
                       const char* filename);

  void restoreBitmapCache() const
    {
      queueBitmapLoad();
      if ( GrObj::X11_BITMAP_CACHE == itsMode ||
           GrObj::GL_BITMAP_CACHE == itsMode )
        postUpdated();
    }

  GrObj::RenderMode getUnMode() const { return itsUnMode; }
  void setUnMode(GrObj::RenderMode new_val) { itsUnMode = new_val; }

private:
  GrObj::RenderMode itsMode;
  GrObj::RenderMode itsUnMode;

  mutable fixed_string itsCacheFilename;

  mutable bool itsIsCurrent;    // true if displaylist is current
  mutable int itsDisplayList;   // OpenGL display list that draws the object
  shared_ptr<BmapRenderer> itsBmapRenderer;
  shared_ptr<BitmapRep> itsBitmapCache;

  fixed_string fullCacheFilename() const
    {
      fixed_string result = BITMAP_CACHE_DIR;
      result += "/";
      result += itsCacheFilename;
      return result;
    }

  void queueBitmapLoad() const;

  // This function updates the cached OpenGL display list.
  void recompileIfNeeded(const GrObjImpl* obj, GWT::Canvas& canvas) const;

  // This function updates the cached bitmap, and returns a true if
  // the bitmap was actually recached, and false if nothing was done.
  bool recacheBitmapIfNeeded(const GrObjImpl* obj, GWT::Canvas& canvas) const;

  void postUpdated() const { itsIsCurrent = true; }

  // This function deletes any previous display list, allocates a new
  // display list, and checks that the allocation actually succeeded.
  void newList() const;
};

static const char vcid_grobjrenderer_h[] = "$Header$";
#endif // !GROBJRENDERER_H_DEFINED
