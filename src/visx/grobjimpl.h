///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Mar 23 16:27:54 2000
// written: Sat Sep 23 14:20:04 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "bitmaprep.h"
#include "grobj.h"
#include "glbmaprenderer.h"
#include "rect.h"
#include "xbmaprenderer.h"

#include "util/strings.h"
#include "util/pointers.h"

namespace {
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

  static const unsigned long GROBJ_SERIAL_VERSION_ID = 1;

public:
  //////////////
  // creators //
  //////////////

  Impl(GrObj* obj);
  virtual ~Impl();

  void serialize(ostream &os, IO::IOFlag flag) const;
  void deserialize(istream &is, IO::IOFlag flag);
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  int charCount() const;

  unsigned long serialVersionId() const { return GROBJ_SERIAL_VERSION_ID; }
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
		itsHasBB(false),
		itsRawBBIsCurrent(false),
		itsCachedRawBB(),
		itsCachedPixelBorder(0),
		itsFinalBBIsCurrent(false),
		itsCachedFinalBB()
		{}
		
	 bool bbExists() const
		{ return itsOwner->grHasBoundingBox(); }

	 const Rect<double>& getRaw() const
		{
		  updateRaw();
		  return itsCachedRawBB;
		}

	 const Rect<double>& getFinal(const GWT::Canvas& canvas) const
		{
		  updateFinal(canvas);
		  return itsCachedFinalBB;
		}

	 void invalidate() 
		{
		  itsRawBBIsCurrent = false;
		  itsFinalBBIsCurrent = false;
		}

  private:
	 void updateRaw() const;

	 void updateFinal(const GWT::Canvas& canvas) const;

	 int pixelBorder() const
		{
		  updateRaw();
		  return itsCachedPixelBorder;
		}

	 // data
  public:
	 bool itsIsVisible;

  private:
	 const GrObj::Impl* const itsOwner;

	 mutable bool itsHasBB;

	 mutable bool itsRawBBIsCurrent;
	 mutable Rect<double> itsCachedRawBB;
	 mutable int itsCachedPixelBorder;

	 mutable bool itsFinalBBIsCurrent;
	 mutable Rect<double> itsCachedFinalBB;
  };

  double nativeWidth() const   { return itsBB.getRaw().width(); }
  double nativeHeight() const  { return itsBB.getRaw().height(); }

  double nativeCenterX() const { return itsBB.getRaw().centerX(); }
  double nativeCenterY() const { return itsBB.getRaw().centerY(); }

public:

  const Rect<double>& getRawBB() const { return itsBB.getRaw(); }

  bool hasBB() const { return itsBB.bbExists(); }

  bool getBoundingBox(const GWT::Canvas& canvas, Rect<double>& bbox) const;

  bool getBBVisibility() const { return itsBB.itsIsVisible; }

  void setBBVisibility(bool visibility) { itsBB.itsIsVisible = visibility; }


  /////////////
  // scaling //
  /////////////
private:
  class Scaler {
  public:
	 Scaler() :
		itsMode(GrObj::NATIVE_SCALING),
		itsWidthFactor(1.0),
		itsHeightFactor(1.0)
		{}

	 void doScaling() const;
	 GrObj::ScalingMode getMode() const { return itsMode; }
	 void setMode(GrObj::ScalingMode new_mode, GrObj::Impl* obj);

  private:
	 GrObj::ScalingMode itsMode;
  public:
	 double itsWidthFactor;
	 double itsHeightFactor;
  };

public:
  void doScaling() const
	 { itsScaler.doScaling(); }

  GrObj::ScalingMode getScalingMode() const
	 { return itsScaler.getMode(); }

  void setScalingMode(GrObj::ScalingMode new_mode)
	 { itsScaler.setMode(new_mode, this); }

  void setWidth(double new_width);
  void setHeight(double new_height);
  void setAspectRatio(double new_aspect_ratio);
  void setMaxDimension(double new_max_dimension);

  double aspectRatio() const
  {
	 if ( !hasBB() ) return 1.0;
	 return (itsScaler.itsHeightFactor != 0.0 ?
				itsScaler.itsWidthFactor/itsScaler.itsHeightFactor : 0.0);
  }

  ///////////////
  // alignment //
  ///////////////
private:
  class Aligner {
  public:
	 Aligner() :
		itsMode(GrObj::NATIVE_ALIGNMENT),
		itsCenterX(0.0),
		itsCenterY(0.0)
		{}
		
	 GrObj::AlignmentMode itsMode;
	 double itsCenterX;
	 double itsCenterY;
  };

public:
  void doAlignment() const;

  void setAlignmentMode(GrObj::AlignmentMode new_mode);
  GrObj::AlignmentMode getAlignmentMode() const { return itsAligner.itsMode; }

  double getCenterX() const { return itsAligner.itsCenterX; }
  double getCenterY() const { return itsAligner.itsCenterY; }

  void setCenterX(double val) { itsAligner.itsCenterX = val; }
  void setCenterY(double val) { itsAligner.itsCenterY = val; }


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

	 void setMode(GrObj::GrObjRenderMode new_mode, GrObj::Impl* obj);

	 GrObj::GrObjRenderMode getMode() const { return itsMode; }

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
		  if ( GrObj::GROBJ_X11_BITMAP_CACHE == itsMode ||
				 GrObj::GROBJ_GL_BITMAP_CACHE == itsMode )
			 postUpdated();
		}

  private:
	 GrObj::GrObjRenderMode itsMode;

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

  GrObj::GrObjRenderMode getRenderMode() const
	 { return itsRenderer.getMode(); }
  void setRenderMode(GrObj::GrObjRenderMode new_mode)
	 { itsRenderer.setMode(new_mode, this); }


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
		itsMode(GrObj::GROBJ_SWAP_FORE_BACK)
		{}

	 GrObj::GrObjRenderMode itsMode;
  };

  void undrawDirectRender(GWT::Canvas& canvas) const;
  void undrawSwapForeBack(GWT::Canvas& canvas) const;
  void undrawClearBoundingBox(GWT::Canvas& canvas) const;

  void undrawBoundingBox(GWT::Canvas& canvas) const;

public:

  GrObj::GrObjRenderMode getUnRenderMode() const
	 { return itsUnRenderer.itsMode; }

  void setUnRenderMode(GrObj::GrObjRenderMode new_mode);

  void undraw(GWT::Canvas& canvas) const;

  ///////////////
  // ? other ? //
  ///////////////
public:
  double finalWidth() const
  { return nativeWidth()*itsScaler.itsWidthFactor; }

  double finalHeight() const
  { return nativeHeight()*itsScaler.itsHeightFactor; }

  double getMaxDimension() const
	 { return max(finalWidth(), finalHeight()); }

  int getCategory() const { return itsCategory; }
  void setCategory(int val) { itsCategory = val; }

  void invalidateCaches();

  ///////////////////////////////////////////
  // Forwards to GrObj's protected members //
  ///////////////////////////////////////////

  void grRender(GWT::Canvas& canvas) const { self->grRender(canvas); }
  void grUnRender(GWT::Canvas& canvas) const { self->grUnRender(canvas); }

  void grGetBoundingBox(Rect<double>& bbox, int& pixel_border) const
	 { self->grGetBoundingBox(bbox, pixel_border); }

  bool grHasBoundingBox() const
	 { return self->grHasBoundingBox(); }

  //////////////////
  // Data members //
  //////////////////
private:
  GrObj* const self;

  int itsCategory;
  BoundingBox itsBB;
  Scaler itsScaler;
  Aligner itsAligner;
  Renderer itsRenderer;
  UnRenderer itsUnRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
