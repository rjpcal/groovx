///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Wed Jul 18 18:31:11 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "bitmaprep.h"
#include "grobj.h"
#include "grobjaligner.h"
#include "grobjscaler.h"
#include "glbmaprenderer.h"
#include "point.h"
#include "rect.h"
#include "xbmaprenderer.h"

#include "util/strings.h"
#include "util/pointers.h"

namespace
{
  template <class T>
  inline T max(const T& t1, const T& t2)
    {
      return (t2 > t1) ? t2 : t1;
    }
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl class
//
///////////////////////////////////////////////////////////////////////

class GrObj::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  static const IO::VersionId GROBJ_SERIAL_VERSION_ID = 1;

public:
  //////////////
  // creators //
  //////////////

  Impl(GrObj* obj);
  virtual ~Impl();

  IO::VersionId serialVersionId() const { return GROBJ_SERIAL_VERSION_ID; }
  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;


  //////////////////
  // bounding box //
  //////////////////
private:
  class BoundingBox {
  private:
    BoundingBox(const BoundingBox&);
    BoundingBox& operator=(const BoundingBox&);

  public:
    BoundingBox(const GrObj::Impl* owner) :
      itsIsVisible(false),
      itsOwner(owner),
      itsRawBBIsCurrent(false),
      itsCachedRawBB(),
      itsCachedPixelBorder(0)
    {}

    const Rect<double>& native() const
      {
        updateRaw();
        return itsCachedRawBB;
      }

    Rect<double> withBorder(const GWT::Canvas& canvas) const;

    int pixelBorder() const
      {
        updateRaw();
        return itsCachedPixelBorder;
      }

    void invalidate()
      {
        itsRawBBIsCurrent = false;
      }

  private:
    void updateRaw() const;

    // data
  public:
    bool itsIsVisible;

  private:
    const GrObj::Impl* const itsOwner;

    mutable bool itsRawBBIsCurrent;
    mutable Rect<double> itsCachedRawBB;
    mutable int itsCachedPixelBorder;
  };

  double nativeWidth() const   { return itsBB.native().width(); }
  double nativeHeight() const  { return itsBB.native().height(); }

  Point<double> nativeCenter() const
  {
    return Point<double>(nativeCenterX(), nativeCenterY());
  }

  double nativeCenterX() const { return itsBB.native().centerX(); }
  double nativeCenterY() const { return itsBB.native().centerY(); }

public:

  const Rect<double>& getRawBB() const { return itsBB.native(); }

  void getBoundingBox(const GWT::Canvas& canvas, Rect<double>& bbox) const
  {
	 bbox = itsBB.withBorder(canvas);
  }

  bool getBBVisibility() const { return itsBB.itsIsVisible; }

  void setBBVisibility(bool visibility) { itsBB.itsIsVisible = visibility; }


  /////////////
  // scaling //
  /////////////
private:
  typedef GrObjScaler Scaler;

public:
  void setWidth(double new_width)
  {
    itsScaler.setWidth(new_width, nativeWidth());
  }

  void setHeight(double new_height)
  {
    itsScaler.setHeight(new_height, nativeHeight());
  }

  void setMaxDimension(double new_max_dimension)
  {
    itsScaler.setMaxDimension(new_max_dimension, getMaxDimension());
  }

  double finalWidth() const
  { return nativeWidth()*itsScaler.itsWidthFactor; }

  double finalHeight() const
  { return nativeHeight()*itsScaler.itsHeightFactor; }

  double getMaxDimension() const
    { return max(finalWidth(), finalHeight()); }


  ///////////////
  // rendering //
  ///////////////
private:
  class Renderer {
  public:
    Renderer();
    virtual ~Renderer();

    static dynamic_string BITMAP_CACHE_DIR;

    void callList() const;

    void invalidate() const { itsIsCurrent = false; }

    void setMode(GrObj::RenderMode new_mode);

    GrObj::RenderMode getMode() const { return itsMode; }

    const dynamic_string& getCacheFilename() const { return itsCacheFilename; }

    void setCacheFilename(const dynamic_string& name)
      {
        itsCacheFilename = name;
        queueBitmapLoad();
      }

    // Returns true if the object was rendered to the screen as part
    // of the update, false otherwise
    bool update(const GrObj::Impl* obj, GWT::Canvas& canvas) const;

    void render(const GrObj::Impl* obj, GWT::Canvas& canvas) const;

    void saveBitmapCache(const GrObj::Impl* obj, GWT::Canvas& canvas,
                         const char* filename) const
      {
        recacheBitmapIfNeeded(obj, canvas);
        itsCacheFilename = filename;

        itsBitmapCache->writePbmFile(fullCacheFilename().c_str());
      }

    void restoreBitmapCache() const
      {
        queueBitmapLoad();
        if ( GrObj::X11_BITMAP_CACHE == itsMode ||
             GrObj::GL_BITMAP_CACHE == itsMode )
          postUpdated();
      }

  private:
    GrObj::RenderMode itsMode;

    mutable dynamic_string itsCacheFilename;

    mutable bool itsIsCurrent;    // true if displaylist is current
    mutable int itsDisplayList;   // OpenGL display list that draws the object
    shared_ptr<BmapRenderer> itsBmapRenderer;
    shared_ptr<BitmapRep> itsBitmapCache;

    dynamic_string fullCacheFilename() const
      {
        dynamic_string result = BITMAP_CACHE_DIR;
        result += "/";
        result += itsCacheFilename;
        return result;
      }

    void queueBitmapLoad() const
      {
        if ( !itsCacheFilename.empty() && itsBitmapCache.get() != 0 )
          itsBitmapCache->queuePbmFile(fullCacheFilename().c_str());
      }

    // This function updates the cached OpenGL display list.
    void recompileIfNeeded(const GrObj::Impl* obj, GWT::Canvas& canvas) const;

    // This function updates the cached bitmap, and returns a true if
    // the bitmap was actually recached, and false if nothing was done.
    bool recacheBitmapIfNeeded(const GrObj::Impl* obj, GWT::Canvas& canvas) const;

    void postUpdated() const { itsIsCurrent = true; }

    // This function deletes any previous display list, allocates a new
    // display list, and checks that the allocation actually succeeded.
    void newList() const;
  };

public:

  GrObj::RenderMode getRenderMode() const
    { return itsRenderer.getMode(); }
  void setRenderMode(GrObj::RenderMode new_mode)
    { itsRenderer.setMode(new_mode); }

  static void setBitmapCacheDir(const char* dirname)
    { Impl::Renderer::BITMAP_CACHE_DIR = dirname; }

  void saveBitmapCache(GWT::Canvas& canvas, const char* filename);
  void restoreBitmapCache() const
    { itsRenderer.restoreBitmapCache(); }
  void update(GWT::Canvas& canvas) const
    { itsRenderer.update(this, canvas); }
  void draw(GWT::Canvas& canvas) const;

  void grDrawBoundingBox(const GWT::Canvas& canvas) const;

  /////////////////
  // unrendering //
  /////////////////
private:
  class UnRenderer {
  public:
    UnRenderer() :
      itsMode(GrObj::SWAP_FORE_BACK)
      {}

    GrObj::RenderMode itsMode;
  };

  void undrawDirectRender(GWT::Canvas& canvas) const;
  void undrawSwapForeBack(GWT::Canvas& canvas) const;
  void undrawClearBoundingBox(GWT::Canvas& canvas) const;

  void undrawBoundingBox(GWT::Canvas& canvas) const;

public:

  GrObj::RenderMode getUnRenderMode() const
    { return itsUnRenderer.itsMode; }

  void setUnRenderMode(GrObj::RenderMode new_mode)
  {
	 itsUnRenderer.itsMode = new_mode;
  }

  void undraw(GWT::Canvas& canvas) const;

  ///////////////
  // ? other ? //
  ///////////////
public:

  int category() const { return itsCategory; }
  void setCategory(int val) { itsCategory = val; }

  void invalidateCaches();

  ///////////////////////////////////////////
  // Forwards to GrObj's protected members //
  ///////////////////////////////////////////

  void grRender(GWT::Canvas& canvas, GrObj::DrawMode mode) const
    { self->grRender(canvas, mode); }

  void grGetBoundingBox(Rect<double>& bbox, int& pixel_border) const
    { self->grGetBoundingBox(bbox, pixel_border); }

  //////////////////
  // Data members //
  //////////////////
private:
  GrObj* const self;

public:
  int itsCategory;
  BoundingBox itsBB;
  Scaler itsScaler;
  GrObjAligner itsAligner;
  Renderer itsRenderer;
  UnRenderer itsUnRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
