///////////////////////////////////////////////////////////////////////
//
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Tue Feb  8 17:55:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <GL/glu.h>
#include <memory>

#include "bitmaprep.h"
#include "canvas.h"
#include "glbmaprenderer.h"
#include "util/error.h"
#include "rect.h"
#include "reader.h"
#include "writer.h"
#include "xbmaprenderer.h"

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#ifndef NULL
#define NULL 0L
#endif

class GrObjError : public virtual ErrorWithMsg {
public:
  GrObjError(const string& msg = "") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
  GLenum BITMAP_CACHE_BUFFER = GL_FRONT;
#ifdef LOCAL_DEBUG
  inline void checkForGlError(const char* where) throw (GrObjError) {
	 GLenum status = glGetError();
	 if (status != GL_NO_ERROR) {
		string msg = reinterpret_cast<const char*>(gluErrorString(status));
		throw GrObjError(string ("GL error: ") + msg + " " + where);
	 }
  }
#else
#  define checkForGlError(x) {}
#endif

}

Util::Tracer GrObj::tracer;

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl class
//
///////////////////////////////////////////////////////////////////////

class GrObj::Impl {
public:
  //////////////
  // creators //
  //////////////

  Impl(GrObj* obj);
  virtual ~Impl();

  virtual void serialize(ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(istream &is, IO::IOFlag flag);
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;


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

  double aspectRatio() const;

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


  //////////////////
  // bounding box //
  //////////////////
private:
  class BoundingBox {
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

	 const Rect<double>& getRaw() const;

	 const Rect<double>& getFinal(const Canvas& canvas) const
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

	 void updateFinal(const Canvas& canvas) const;

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

  double nativeWidth() const;
  double nativeHeight() const;

  double nativeCenterX() const;
  double nativeCenterY() const;

public:

  const Rect<double>& getRawBB() const { return itsBB.getRaw(); }

  bool hasBB() const { return itsBB.bbExists(); }

  bool getBoundingBox(const Canvas& canvas, Rect<double>& bbox) const;

  bool getBBVisibility() const { return itsBB.itsIsVisible; }

  void setBBVisibility(bool visibility);


  ///////////////
  // rendering //
  ///////////////
private:
  class Renderer {
  public:
	 Renderer();
	 virtual ~Renderer();

	 void callList() const;

	 void invalidate() const { itsIsCurrent = false; }

	 void setMode(GrObj::GrObjRenderMode new_mode, GrObj::Impl* obj);

	 GrObj::GrObjRenderMode getMode() const { return itsMode; }

	 // Returns true if the object was rendered to the screen as part
	 // of the update, false otherwise
	 bool update(const GrObj::Impl* obj, Canvas& canvas) const;

	 void render(const GrObj::Impl* obj, Canvas& canvas) const;

  private:
	 GrObj::GrObjRenderMode itsMode;

	 mutable bool itsIsCurrent;    // true if displaylist is current
	 mutable int itsDisplayList;   // OpenGL display list that draws the object
	 auto_ptr<BmapRenderer> itsBmapRenderer;
	 auto_ptr<BitmapRep> itsBitmapCache;

	 // This function updates the cached OpenGL display list.
	 void recompileIfNeeded(const GrObj::Impl* obj, Canvas& canvas) const;

	 // This function updates the cached bitmap, and returns a true if
	 // the bitmap was actually recached, and false if nothing was done.
	 bool recacheBitmapIfNeeded(const GrObj::Impl* obj, Canvas& canvas) const;

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


  void update(Canvas& canvas) const;
  void draw(Canvas& canvas) const;

  void grDrawBoundingBox(const Canvas& canvas) const;

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

  void undrawDirectRender(Canvas& canvas) const;
  void undrawSwapForeBack(Canvas& canvas) const;
  void undrawClearBoundingBox(Canvas& canvas) const;

  void undrawBoundingBox(Canvas& canvas) const;

public:

  GrObj::GrObjRenderMode getUnRenderMode() const { return itsUnRenderer.itsMode; }

  void setUnRenderMode(GrObj::GrObjRenderMode new_mode);

  void undraw(Canvas& canvas) const;

  ///////////////
  // ? other ? //
  ///////////////
public:
  double finalWidth() const;
  double finalHeight() const;

  double getMaxDimension() const
	 { return max(finalWidth(), finalHeight()); }

  int getCategory() const { return itsCategory; }
  void setCategory(int val) { itsCategory = val; }

  void invalidateCaches();

  ///////////////////////////////////////////
  // Forwards to GrObj's protected members //
  ///////////////////////////////////////////

  void grRender(Canvas& canvas) const { self->grRender(canvas); }
  void grUnRender(Canvas& canvas) const { self->grUnRender(canvas); }

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
  Scaler itsScaler;
  Aligner itsAligner;
  BoundingBox itsBB;
  Renderer itsRenderer;
  UnRenderer itsUnRenderer;
};


///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::Scaler definitions
//
///////////////////////////////////////////////////////////////////////

void GrObj::Impl::Scaler::doScaling() const {
DOTRACE("GrObj::Impl::Scaler::doScaling");
  if (GrObj::NATIVE_SCALING == itsMode) return;
	 
  switch (itsMode) {
  case GrObj::MAINTAIN_ASPECT_SCALING:
  case GrObj::FREE_SCALING:
	 glScaled(itsWidthFactor, itsHeightFactor, 1.0);
	 break;
  }
}

void GrObj::Impl::Scaler::setMode(GrObj::ScalingMode new_mode,
											 GrObj::Impl* obj) {
DOTRACE("GrObj::Impl::Scaler::setMode");
  if (itsMode == new_mode) return;

  switch (new_mode) {
  case GrObj::NATIVE_SCALING:
	 itsMode = new_mode;

	 itsWidthFactor = 1.0;
	 itsHeightFactor = 1.0;
	 break;

  case GrObj::MAINTAIN_ASPECT_SCALING:
  case GrObj::FREE_SCALING:
	 // These modes require a bounding box
	 if ( !obj->hasBB() ) return;

	 itsMode = new_mode;
	 break;

  } // end switch
}



///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::BoundingBox definitions
//
///////////////////////////////////////////////////////////////////////

void GrObj::Impl::BoundingBox::updateRaw() const {
DOTRACE("GrObj::Impl::BoundingBox::updateRaw");
  DebugEval(itsRawBBIsCurrent);
  if (!itsRawBBIsCurrent) {
	 itsHasBB = itsOwner->grHasBoundingBox();

	 if (itsHasBB) {
		itsOwner->grGetBoundingBox(itsCachedRawBB, itsCachedPixelBorder);
	 }
	 else {
		itsCachedRawBB.setRectLTRB(0.0, 0.0, 0.0, 0.0);
		itsCachedPixelBorder = 0;
	 }
	 itsRawBBIsCurrent = true;
  }
  DebugEvalNL(itsHasBB);
}

void GrObj::Impl::BoundingBox::updateFinal(const Canvas& canvas) const {
DOTRACE("GrObj::Impl::BoundingBox::updateFinal");

  if ( !itsFinalBBIsCurrent ) {

	 // Do the object's internal scaling and alignment, and find the
	 // bounding box in screen coordinates
	 
	 Rect<int> screen_pos;

	 {
		glMatrixMode(GL_MODELVIEW);

		Canvas::StateSaver state(canvas);

		itsOwner->doScaling();
		itsOwner->doAlignment();

		screen_pos = canvas.getScreenFromWorld(getRaw());
	 }  
	 
	 // Add a pixel border around the edges of the image...
	 int bp = pixelBorder();
	 
	 screen_pos.widenByStep(bp);
	 screen_pos.heightenByStep(bp);

	 // ... and project back to world coordinates
	 itsCachedFinalBB = canvas.getWorldFromScreen(screen_pos);
	 
	 // This next line is commented out to disable the caching scheme
	 // because I don't think it really works, since changes to the
	 // OpenGL state will screw up a cached copy of the box. What we
	 // need is a way to determine whether the OpenGL state has
	 // changed... but this might be impractical.
//  	 itsFinalBBIsCurrent = true;
  }
}

const Rect<double>& GrObj::Impl::BoundingBox::getRaw() const {
DOTRACE("GrObj::Impl::BoundingBox::getRaw");
  updateRaw();
  return itsCachedRawBB;
}





///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::Renderer definitions
//
///////////////////////////////////////////////////////////////////////

GrObj::Impl::Renderer::Renderer() :
  itsMode(GrObj::GROBJ_GL_COMPILE),
  itsIsCurrent(false),
  itsDisplayList(-1),
  itsBmapRenderer(0),
  itsBitmapCache(0)
{
DOTRACE("GrObj::Impl::Renderer::Renderer");
}

GrObj::Impl::Renderer::~Renderer() {
DOTRACE("GrObj::Impl::Renderer::~Renderer");
  glDeleteLists(itsDisplayList, 1);
}

void GrObj::Impl::Renderer::recompileIfNeeded(const GrObj::Impl* obj,
															 Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recompileIfNeeded");
  if (itsIsCurrent) return;
  
  newList();
  
  glNewList(itsDisplayList, GL_COMPILE);
  obj->grRender(canvas);
  glEndList();
  
  postUpdated();
}

bool GrObj::Impl::Renderer::recacheBitmapIfNeeded(const GrObj::Impl* obj,
																  Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recacheBitmapIfNeeded");
  if (itsIsCurrent) return false;

  Assert(itsBitmapCache.get() != 0);
  
  obj->undraw(canvas);
  
  {
	 glMatrixMode(GL_MODELVIEW);
	 Canvas::StateSaver state(canvas);

	 glPushAttrib(GL_PIXEL_MODE_BIT|GL_COLOR_BUFFER_BIT);
	 {
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		
		obj->doScaling();
		obj->doAlignment();
		
		obj->grRender(canvas);
		
		Assert(obj->hasBB());
		
		itsBitmapCache->grabWorldRect(obj->getRawBB());
		itsBitmapCache->setRasterX(obj->getRawBB().left());
		itsBitmapCache->setRasterY(obj->getRawBB().bottom());
	 }
	 glPopAttrib();
  }
  
  if (GrObj::GROBJ_X11_BITMAP_CACHE == itsMode) {
	 itsBitmapCache->flipVertical();
	 itsBitmapCache->flipContrast();
  }
  
  if (GrObj::GROBJ_GL_BITMAP_CACHE == itsMode) {
	 itsBitmapCache->flipContrast();
  }
  
  postUpdated();

  return true;
}

void GrObj::Impl::Renderer::callList() const {
DOTRACE("GrObj::Impl::Renderer::callList");
  // We must explicitly check that the display list is valid,
  // since it might be invalid if the object was recently
  // constructed, for example.
  if (glIsList(itsDisplayList) == GL_TRUE) {
	 glCallList(itsDisplayList);
	 DebugEvalNL(itsDisplayList);
  }
}

void GrObj::Impl::Renderer::newList() const {
DOTRACE("GrObj::Impl::Renderer::newList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1); 
  if (itsDisplayList == 0) {     
	 throw GrObjError("GrObj::newList: couldn't allocate display list");
  }
}

void GrObj::Impl::Renderer::setMode(GrObj::GrObjRenderMode new_mode,
												GrObj::Impl* obj) {
DOTRACE("GrObj::Impl::Renderer::setMode");
  // If new_mode is the same as the current render mode, then return
  // immediately (and don't send a state change message)
  if (new_mode == itsMode) return; 

  switch (new_mode) {
  case GrObj::GROBJ_DIRECT_RENDER:
  case GrObj::GROBJ_GL_COMPILE:
	 itsMode = new_mode;
	 break;

  case GrObj::GROBJ_GL_BITMAP_CACHE:
  case GrObj::GROBJ_X11_BITMAP_CACHE:
	 // These modes require a bounding box
	 if ( !obj->hasBB() ) return;

	 if (GrObj::GROBJ_GL_BITMAP_CACHE == new_mode) {
		itsBmapRenderer.reset(new GLBmapRenderer());
		itsBitmapCache.reset(new BitmapRep(itsBmapRenderer.get()));
	 }
	 if (GrObj::GROBJ_X11_BITMAP_CACHE == new_mode) {
		itsBmapRenderer.reset(new XBmapRenderer());
		itsBitmapCache.reset(new BitmapRep(itsBmapRenderer.get()));
	 }
	 
	 itsMode = new_mode;
	 break;

  } // end switch
}

void GrObj::Impl::Renderer::render(const GrObj::Impl* obj,
											  Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::render");
  switch (itsMode) {

  case GrObj::GROBJ_DIRECT_RENDER:
	 obj->grRender(canvas);
	 break;

  case GrObj::GROBJ_GL_COMPILE:
	 callList(); 
	 break;

  case GrObj::GROBJ_GL_BITMAP_CACHE:
  case GrObj::GROBJ_X11_BITMAP_CACHE:
	 Assert(itsBitmapCache.get() != 0);
	 itsBitmapCache->grRender(canvas);
	 break;

  } // end switch
}

bool GrObj::Impl::Renderer::update(const GrObj::Impl* obj,
											  Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::update");
  checkForGlError("before GrObj::update");

  bool objectDrawn = false;

  switch (itsMode) {
  case GrObj::GROBJ_GL_COMPILE:
	 recompileIfNeeded(obj, canvas);
	 break;
		
  case GrObj::GROBJ_GL_BITMAP_CACHE:
  case GrObj::GROBJ_X11_BITMAP_CACHE:
	 objectDrawn = recacheBitmapIfNeeded(obj, canvas);
	 break;
  }
  checkForGlError("during GrObj::update");

  return objectDrawn;
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl creator definitions
//
///////////////////////////////////////////////////////////////////////

GrObj::Impl::Impl(GrObj* obj) :
  self(obj),
  itsCategory(-1),
  itsScaler(),
  itsAligner(),
  itsBB(this),
  itsRenderer(),
  itsUnRenderer()
{};

GrObj::Impl::~Impl() {
DOTRACE("GrObj::Impl::~Impl");
}

void GrObj::Impl::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("GrObj::Impl::serialize");

  DebugEvalNL(flag & IO::TYPENAME); 

  if (flag & IO::TYPENAME) { os << "GrObj" << IO::SEP; }

  os << itsCategory << IO::SEP;

  os << itsRenderer.getMode() << IO::SEP;
  os << itsUnRenderer.itsMode << IO::SEP;

  os << itsBB.itsIsVisible << IO::SEP;

  os << itsScaler.getMode() << IO::SEP;
  os << itsScaler.itsWidthFactor << IO::SEP;
  os << itsScaler.itsHeightFactor << IO::SEP;

  os << itsAligner.itsMode << IO::SEP;
  os << itsAligner.itsCenterX << IO::SEP;
  os << itsAligner.itsCenterY << endl;  

  if (os.fail()) throw OutputError("GrObj");

  if (flag & IO::BASES) { /* no bases to serialize */ }
}

void GrObj::Impl::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("GrObj::Impl::deserialize");

  DebugEvalNL(flag & IO::TYPENAME); 

  if (flag & IO::TYPENAME) { IO::readTypename(is, "GrObj"); }

  int temp;

  is >> itsCategory; if (is.fail()) throw InputError("after GrObj::itsCategory");

  is >> temp; itsRenderer.setMode(temp, this);
  if (is.fail()) throw InputError("after GrObj::itsRenderer.itsMode");

  is >> itsUnRenderer.itsMode; if (is.fail()) throw InputError("after GrObj::itsUnRenderer.itsMode");

  is >> temp; if (is.fail()) throw InputError("after GrObj::temp"); itsBB.itsIsVisible = bool(temp);

  is >> temp; itsScaler.setMode(temp, this);
  if (is.fail()) throw InputError("after GrObj::itsScaler.itsMode");

  is >> itsScaler.itsWidthFactor; if (is.fail()) throw InputError("after GrObj::itsScaler.itsWidthFactor");
  is >> itsScaler.itsHeightFactor; if (is.fail()) throw InputError("after GrObj::itsScaler.itsHeightFactor");

  is >> itsAligner.itsMode; if (is.fail()) throw InputError("after GrObj::itsAligner.itsMode");
  is >> itsAligner.itsCenterX; if (is.fail()) throw InputError("after GrObj::itsAligner.itsCenterX");
  is >> itsAligner.itsCenterY; if (is.fail()) throw InputError("after GrObj::itsAligner.itsCenterY");

  invalidateCaches();

  if (is.fail()) throw InputError("GrObj");

  if (flag & IO::BASES) { /* no bases to deserialize */ }
}

int GrObj::Impl::charCount() const {
DOTRACE("GrObj::Impl::charCount");
  int count = 
	 gCharCount("GrObj") + 1
	 + gCharCount(itsCategory) + 1
	 
	 + gCharCount(itsRenderer.getMode()) + 1
	 + gCharCount(itsUnRenderer.itsMode) + 1
	 
	 + gCharCount(itsBB.itsIsVisible) + 1
	 
	 + gCharCount(itsScaler.getMode()) + 1
	 + gCharCount(itsScaler.itsWidthFactor) + 1
	 + gCharCount(itsScaler.itsHeightFactor) + 1
	 
	 + gCharCount(itsAligner.itsMode) + 1
	 + gCharCount(itsAligner.itsCenterX) + 1
	 + gCharCount(itsAligner.itsCenterY) + 1
	 + 5; // fudge factor
  DebugEvalNL(count);
  return count;
}

void GrObj::Impl::readFrom(Reader* reader) {
DOTRACE("GrObj::Impl::readFrom");
  reader->readValue("GrObj::category", itsCategory);

  int temp;

  reader->readValue("GrObj::renderMode", temp);
  itsRenderer.setMode(temp, this);

  reader->readValue("GrObj::unRenderMod", itsUnRenderer.itsMode);

  reader->readValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  reader->readValue("GrObj::scalingMode", temp);
  itsScaler.setMode(temp, this);

  reader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  reader->readValue("GrObj::centerX", itsAligner.itsCenterX);
  reader->readValue("GrObj::centerY", itsAligner.itsCenterY);
}

void GrObj::Impl::writeTo(Writer* writer) const {
DOTRACE("GrObj::Impl::writeTo");
  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsRenderer.getMode());
  writer->writeValue("GrObj::unRenderMod", itsUnRenderer.itsMode);

  writer->writeValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  writer->writeValue("GrObj::scalingMode", itsScaler.getMode());
  writer->writeValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  writer->writeValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  writer->writeValue("GrObj::alignmentMode", itsAligner.itsMode);
  writer->writeValue("GrObj::centerX", itsAligner.itsCenterX);
  writer->writeValue("GrObj::centerY", itsAligner.itsCenterY);
}


///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl accessor definitions
//
///////////////////////////////////////////////////////////////////////

inline double GrObj::Impl::aspectRatio() const {
  if ( !hasBB() ) return 1.0;
  return (itsScaler.itsHeightFactor != 0.0 ? itsScaler.itsWidthFactor/itsScaler.itsHeightFactor : 0.0);
}

inline double GrObj::Impl::finalWidth() const {
  return nativeWidth()*itsScaler.itsWidthFactor;
}

inline double GrObj::Impl::finalHeight() const {
  return nativeHeight()*itsScaler.itsHeightFactor;
}

inline double GrObj::Impl::nativeWidth() const {
  return itsBB.getRaw().width();
}

inline double GrObj::Impl::nativeHeight() const {
  return itsBB.getRaw().height();;
}

inline double GrObj::Impl::nativeCenterX() const {
  return itsBB.getRaw().centerX();
}

inline double GrObj::Impl::nativeCenterY() const {
  return itsBB.getRaw().centerY();
}

bool GrObj::Impl::getBoundingBox(const Canvas& canvas,
											Rect<double>& bbox) const {
DOTRACE("GrObj::Impl::getBoundingBox");

  if ( !itsBB.bbExists() ) return false;

  bbox = itsBB.getFinal(canvas);
  return true;
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl manipulator definitions
//
///////////////////////////////////////////////////////////////////////

inline void GrObj::Impl::setBBVisibility(bool visibility) {
DOTRACE("GrObj::Impl::setBBVisibility");
  itsBB.itsIsVisible = visibility;
}

void GrObj::Impl::setWidth(double new_width) {
DOTRACE("GrObj::Impl::setWidth");
  if (new_width == 0.0 || new_width == finalWidth()) return; 
  if (itsScaler.getMode() == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double new_width_factor = new_width / nativeWidth();
  
  double change_factor = new_width_factor / itsScaler.itsWidthFactor;
	 
  itsScaler.itsWidthFactor = new_width_factor;

  if (itsScaler.getMode() == GrObj::MAINTAIN_ASPECT_SCALING) {
	 itsScaler.itsHeightFactor *= change_factor;
  }
}

void GrObj::Impl::setHeight(double new_height) {
DOTRACE("GrObj::Impl::setHeight");
  if (new_height == 0.0 || new_height == finalHeight()) return; 
  if (itsScaler.getMode() == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double new_height_factor = new_height / nativeHeight();

  double change_factor = new_height_factor / itsScaler.itsHeightFactor;

  itsScaler.itsHeightFactor = new_height_factor;

  if (itsScaler.getMode() == GrObj::MAINTAIN_ASPECT_SCALING) {
	 itsScaler.itsWidthFactor *= change_factor;
  }
}

void GrObj::Impl::setAspectRatio(double new_aspect_ratio) {
DOTRACE("GrObj::Impl::setAspectRatio");
  if (new_aspect_ratio == 0.0 || new_aspect_ratio == aspectRatio()) return; 
  if (itsScaler.getMode() == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double change_factor = new_aspect_ratio / (aspectRatio());

  // By convention, we change only the width to reflect the change in
  // aspect ratio

  itsScaler.itsWidthFactor *= change_factor;
}

void GrObj::Impl::setMaxDimension(double new_max_dimension) {
DOTRACE("GrObj::Impl::setMaxDimension");
  if (itsScaler.getMode() == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double scaling_factor = new_max_dimension / max(finalWidth(), finalHeight());

  itsScaler.itsWidthFactor *= scaling_factor;
  itsScaler.itsHeightFactor *= scaling_factor;
}

void GrObj::Impl::setAlignmentMode(GrObj::AlignmentMode new_mode) {
DOTRACE("GrObj::Impl::setAlignmentMode");
  DebugEval(new_mode); 
  DebugEvalNL(itsAligner.itsMode); 

  if (new_mode == itsAligner.itsMode) return;

  switch (new_mode) {
  case GrObj::NATIVE_ALIGNMENT:
	 itsAligner.itsMode = new_mode;
	 break;

  case GrObj::CENTER_ON_CENTER:
  case GrObj::NW_ON_CENTER:
  case GrObj::NE_ON_CENTER:
  case GrObj::SW_ON_CENTER:
  case GrObj::SE_ON_CENTER:
  case GrObj::ARBITRARY_ON_CENTER:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsAligner.itsMode = new_mode;
	 break;

  } // end switch

  DebugEvalNL(itsAligner.itsMode); 
}

void GrObj::Impl::setUnRenderMode(GrObj::GrObjRenderMode new_mode) {
DOTRACE("GrObj::Impl::setUnRenderMode");
  // If new_mode is the same as the current unrender mode, then return
  // immediately (and don't send a state change message)
  if (new_mode == itsUnRenderer.itsMode) return; 

  switch (new_mode) {
  case GrObj::GROBJ_DIRECT_RENDER:
  case GrObj::GROBJ_SWAP_FORE_BACK:
	 itsUnRenderer.itsMode = new_mode;
	 break;

  case GrObj::GROBJ_CLEAR_BOUNDING_BOX:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsUnRenderer.itsMode = new_mode;
	 break;

  } // end switch
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl action definitions
//
///////////////////////////////////////////////////////////////////////

void GrObj::Impl::update(Canvas& canvas) const {
DOTRACE("GrObj::Impl::update");
  itsRenderer.update(this, canvas);
}

void GrObj::Impl::draw(Canvas& canvas) const {
DOTRACE("GrObj::Impl::draw");
  checkForGlError("before GrObj::draw");

  if ( itsBB.itsIsVisible ) {
	 grDrawBoundingBox(canvas);
  }

  bool objectDrawn = itsRenderer.update(this, canvas);

  if ( !objectDrawn ) {

	 {
		glMatrixMode(GL_MODELVIEW);

		Canvas::StateSaver state(canvas);

	   doScaling();
		doAlignment();

		itsRenderer.render(this, canvas);
	 }
  }

  checkForGlError("during GrObj::draw"); 
}

void GrObj::Impl::undraw(Canvas& canvas) const {
DOTRACE("GrObj::Impl::undraw");
  checkForGlError("before GrObj::undraw");

  switch (itsUnRenderer.itsMode) {
  case GrObj::GROBJ_DIRECT_RENDER:     undrawDirectRender(canvas);     break;
  case GrObj::GROBJ_SWAP_FORE_BACK:    undrawSwapForeBack(canvas);     break;
  case GrObj::GROBJ_CLEAR_BOUNDING_BOX:undrawClearBoundingBox(canvas); break;
  default:                             /* nothing */                  break;
  }

  if ( itsBB.itsIsVisible ) {
	 undrawBoundingBox(canvas);
  }

  checkForGlError("during GrObj::undraw");
}

void GrObj::Impl::grDrawBoundingBox(const Canvas& canvas) const {
DOTRACE("GrObj::Impl::grDrawBoundingBox");
  Rect<double> bbox;
  if ( getBoundingBox(canvas, bbox) ) {
	 DebugPrintNL("drawing bounding box");
	 glPushAttrib(GL_LINE_BIT);
	 {
		glLineWidth(1.0);
		glEnable(GL_LINE_STIPPLE);
 		glLineStipple(1, 0x0F0F);

		glBegin(GL_LINE_LOOP);
		  glVertex2d(bbox.left(), bbox.bottom());
		  glVertex2d(bbox.right(), bbox.bottom());
		  glVertex2d(bbox.right(), bbox.top());
		  glVertex2d(bbox.left(), bbox.top());
		glEnd();
	 }
	 glPopAttrib();
  }
}

void GrObj::Impl::invalidateCaches() {
DOTRACE("GrObj::Impl::invalidateCaches");
  itsRenderer.invalidate();
  itsBB.invalidate();
}

void GrObj::Impl::doAlignment() const {
DOTRACE("GrObj::Impl::doAlignment");
  if (GrObj::NATIVE_ALIGNMENT == itsAligner.itsMode) return;
	 
  Assert(hasBB());
	 
  double width = nativeWidth();
  double height = nativeHeight();
	 
  // These indicate where the center of the object will go
  double centerX=0.0, centerY=0.0;
	 
  switch (itsAligner.itsMode) {
  case GrObj::CENTER_ON_CENTER:
	 centerX = 0.0; centerY = 0.0;
	 break;
  case GrObj::NW_ON_CENTER:
	 centerX = width/2.0; centerY = -height/2.0;
	 break;
  case GrObj::NE_ON_CENTER:
	 centerX = -width/2.0; centerY = -height/2.0;
	 break;
  case GrObj::SW_ON_CENTER:
	 centerX = width/2.0; centerY = height/2.0;
	 break;
  case GrObj::SE_ON_CENTER:
	 centerX = -width/2.0; centerY = height/2.0;
	 break;
  case GrObj::ARBITRARY_ON_CENTER:
	 centerX = itsAligner.itsCenterX; centerY = itsAligner.itsCenterY;
	 break;
  }
	 
  glTranslated(centerX-nativeCenterX(),
					centerY-nativeCenterY(),
					0.0);
}

void GrObj::Impl::undrawDirectRender(Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawDirectRender");
  glMatrixMode(GL_MODELVIEW);

  Canvas::StateSaver state(canvas);

  doScaling();
  doAlignment();
	 
  self->grUnRender(canvas);
}

void GrObj::Impl::undrawSwapForeBack(Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawSwapForeBack");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 canvas.swapForeBack();
	 
	 {
		glMatrixMode(GL_MODELVIEW);

		Canvas::StateSaver state(canvas);

		doScaling();
		doAlignment();
		  
		if ( itsRenderer.getMode() == GrObj::GROBJ_GL_COMPILE ) {
		  itsRenderer.callList();
		}
		else {
		  self->grRender(canvas);
		}
	 }
  }
  glPopAttrib();
}

void GrObj::Impl::undrawClearBoundingBox(Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawClearBoundingBox");
  glMatrixMode(GL_MODELVIEW);

  Rect<double> world_pos;
  if ( getBoundingBox(canvas, world_pos) ) {
	 glPushAttrib(GL_SCISSOR_BIT);
	 {
		glEnable(GL_SCISSOR_TEST);

		Rect<int> screen_pos = canvas.getScreenFromWorld(world_pos);

		// Add an extra one-pixel border around the rect
		screen_pos.widenByStep(1);
		screen_pos.heightenByStep(1);

		glScissor(screen_pos.left(), screen_pos.bottom(),
					 screen_pos.width(), screen_pos.height());

		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
	 }
	 glPopAttrib();
  }
}

void GrObj::Impl::undrawBoundingBox(Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawBoundingBox");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 canvas.swapForeBack();

	 {
		glMatrixMode(GL_MODELVIEW);

		Canvas::StateSaver state(canvas);

		doScaling();
		doAlignment();
		  
		grDrawBoundingBox(canvas);
	 }
  }
  glPopAttrib();
}

///////////////////////////////////////////////////////////////////////
//
// GrObj member definitions
//
///////////////////////////////////////////////////////////////////////


/////////////////////////
// flags and constants //
/////////////////////////

const GrObj::GrObjRenderMode GrObj::GROBJ_DIRECT_RENDER;
const GrObj::GrObjRenderMode GrObj::GROBJ_GL_COMPILE;
const GrObj::GrObjRenderMode GrObj::GROBJ_GL_BITMAP_CACHE;
const GrObj::GrObjRenderMode GrObj::GROBJ_X11_BITMAP_CACHE;
const GrObj::GrObjRenderMode GrObj::GROBJ_SWAP_FORE_BACK;
const GrObj::GrObjRenderMode GrObj::GROBJ_CLEAR_BOUNDING_BOX;


const int GrObj::NATIVE_SCALING;
const int GrObj::MAINTAIN_ASPECT_SCALING;
const int GrObj::FREE_SCALING;


const int GrObj::NATIVE_ALIGNMENT;
const int GrObj::CENTER_ON_CENTER;
const int GrObj::NW_ON_CENTER;
const int GrObj::NE_ON_CENTER;
const int GrObj::SW_ON_CENTER;
const int GrObj::SE_ON_CENTER;
const int GrObj::ARBITRARY_ON_CENTER;

//////////////
// creators //
//////////////

// GrObj default constructor
GrObj::GrObj(GrObjRenderMode render_mode,
				 GrObjRenderMode unrender_mode) :
  itsImpl(new Impl(this))
{
DOTRACE("GrObj::GrObj");
  // The GrObj needs to observe itself in order to update its display
  // list according to state changes.
  attach(this);

  setRenderMode(render_mode);
  setUnRenderMode(unrender_mode);

  // This is necessary because any representations that have been
  // cached during the GrObj constructor will become invalid upon
  // return to the derived class constructor.
  sendStateChangeMsg();
}

// read the object's state from an input stream. The input stream must
// already be open and connected to an appropriate file.
GrObj::GrObj(istream& is, IOFlag flag) :
  itsImpl(new Impl(this))
{
DOTRACE("GrObj::GrObj(istream&)");
  deserialize(is, flag);

  attach(this);

  sendStateChangeMsg();
}

// GrObj destructor
GrObj::~GrObj() {
DOTRACE("GrObj::~GrObj");
  detach(this); 
  delete itsImpl;
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
void GrObj::serialize(ostream& os, IOFlag flag) const {
DOTRACE("GrObj::serialize");
  itsImpl->serialize(os, flag);
}

void GrObj::deserialize(istream& is, IOFlag flag) {
DOTRACE("GrObj::deserialize");

  itsImpl->deserialize(is, flag); 
  sendStateChangeMsg();
}

void GrObj::readFrom(Reader* reader) {
DOTRACE("GrObj::readFrom");

  itsImpl->readFrom(reader);
  sendStateChangeMsg();
}

void GrObj::writeTo(Writer* writer) const {
DOTRACE("GrObj::writeTo");
  itsImpl->writeTo(writer);
}

int GrObj::charCount() const {
DOTRACE("GrObj::charCount");
  return itsImpl->charCount();
}

///////////////
// accessors //
///////////////

bool GrObj::getBBVisibility() const {
DOTRACE("GrObj::getBBVisibility");
  return itsImpl->getBBVisibility();
}

bool GrObj::getBoundingBox(const Canvas& canvas, Rect<double>& bbox) const {
DOTRACE("GrObj::getBoundingBox");

  return itsImpl->getBoundingBox(canvas, bbox);
}

void GrObj::grGetBoundingBox(Rect<double>& /*bounding_box*/,
									  int& /* border_pixels */) const {
DOTRACE("GrObj::grGetBoundingBox");
}

bool GrObj::grHasBoundingBox() const {
DOTRACE("GrObj::grHasBoundingBox");
  return false;
}

GrObj::ScalingMode GrObj::getScalingMode() const {
DOTRACE("GrObj::getScalingMode");
  return itsImpl->getScalingMode();
}

double GrObj::getWidth() const {
DOTRACE("GrObj::getWidth");
  return itsImpl->finalWidth();
}

double GrObj::getHeight() const {
DOTRACE("GrObj::getHeight");
  return itsImpl->finalHeight();
}

double GrObj::getAspectRatio() const {
DOTRACE("GrObj::getAspectRatio");
  return itsImpl->aspectRatio();
}

double GrObj::getMaxDimension() const {
DOTRACE("GrObj::getMaxDimension");
  return itsImpl->getMaxDimension();
}

GrObj::AlignmentMode GrObj::getAlignmentMode() const {
DOTRACE("GrObj::getAlignmentMode");
  return itsImpl->getAlignmentMode();
}

double GrObj::getCenterX() const {
DOTRACE("GrObj::getCenterX");
  return itsImpl->getCenterX();
}

double GrObj::getCenterY() const {
DOTRACE("GrObj::getCenterY");
  return itsImpl->getCenterY();
}

int GrObj::getCategory() const {
DOTRACE("GrObj::getCategory");
  return itsImpl->getCategory();
}

GrObj::GrObjRenderMode GrObj::getRenderMode() const {
  return itsImpl->getRenderMode();
}

GrObj::GrObjRenderMode GrObj::getUnRenderMode() const {
DOTRACE("GrObj::getUnRenderMode");
  return itsImpl->getUnRenderMode();
}

//////////////////
// manipulators //
//////////////////
									
void GrObj::setBBVisibility(bool visibility) {
  itsImpl->setBBVisibility(visibility);
}

void GrObj::setScalingMode(ScalingMode val) {
DOTRACE("GrObj::setScalingMode");

  itsImpl->setScalingMode(val);
  sendStateChangeMsg();
}

void GrObj::setWidth(double val) {
DOTRACE("GrObj::setWidth");

  itsImpl->setWidth(val); 
  sendStateChangeMsg();
}

void GrObj::setHeight(double val) {
DOTRACE("GrObj::setHeight");

  itsImpl->setHeight(val); 
  sendStateChangeMsg();
}

void GrObj::setAspectRatio(double val) {
DOTRACE("GrObj::setAspectRatio");

  itsImpl->setAspectRatio(val); 
  sendStateChangeMsg();
}

void GrObj::setMaxDimension(double val) {
DOTRACE("GrObj::setMaxDimension");

  itsImpl->setMaxDimension(val); 
  sendStateChangeMsg();
}

void GrObj::setAlignmentMode(AlignmentMode val) {
DOTRACE("GrObj::setAlignmentMode");

  itsImpl->setAlignmentMode(val);
  sendStateChangeMsg();
}

void GrObj::setCenterX(double val) {
DOTRACE("GrObj::setCenterX");

  itsImpl->setCenterX(val);
  sendStateChangeMsg();
}

void GrObj::setCenterY(double val) {
DOTRACE("GrObj::setCenterY");

  itsImpl->setCenterY(val);
  sendStateChangeMsg();
}

void GrObj::setCategory(int val) {
DOTRACE("GrObj::setCategory");
  itsImpl->setCategory(val);
}

void GrObj::setRenderMode(GrObjRenderMode mode) {
DOTRACE("GrObj::setRenderMode");

  itsImpl->setRenderMode(mode); 
  sendStateChangeMsg();
}

void GrObj::setUnRenderMode(GrObjRenderMode mode) {
DOTRACE("GrObj::setUnRenderMode");

  itsImpl->setUnRenderMode(mode); 
  sendStateChangeMsg();
}

void GrObj::receiveStateChangeMsg(const Observable* obj) {
DOTRACE("GrObj::receiveStateChangeMsg");
  if (obj == this) {
	 itsImpl->invalidateCaches();
  }
}

void GrObj::receiveDestroyMsg(const Observable*) {
DOTRACE("GrObj::receiveDestroyMsg");
  // Do nothing since the only Observable that 'this' is watching is
  // itself
}


/////////////
// actions //
/////////////

void GrObj::update(Canvas& canvas) const {
  itsImpl->update(canvas);
}

void GrObj::draw(Canvas& canvas) const {
  itsImpl->draw(canvas);
}

void GrObj::undraw(Canvas& canvas) const {
  itsImpl->undraw(canvas);
}

void GrObj::grUnRender(Canvas&) const {
DOTRACE("GrObj::grUnRender");
  // Empty default implementation of this virtual function
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
