///////////////////////////////////////////////////////////////////////
//
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Tue Nov  2 10:07:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <GL/glu.h>

#include "gfxattribs.h"
#include "glbitmap.h"
#include "xbitmap.h"
#include "error.h"
#include "rect.h"
#include "reader.h"
#include "writer.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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
///////////////////////////////////////////////////////////////////////
//
// GrObjImpl class
//
///////////////////////////////////////////////////////////////////////

class GrObjImpl {
public:
  //////////////
  // creators //
  //////////////

  GrObjImpl(GrObj* obj);
  ~GrObjImpl();

  virtual void serialize(ostream &os, IO::IOFlag flag) const;
  virtual void deserialize(istream &is, IO::IOFlag flag);
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;


  ///////////////
  // accessors //
  ///////////////

private:
  const Rect<double>& getRawBB() const;

  int getBBPixelBorder() const;

  bool hasBB() const;

  double nativeWidth() const;
  double nativeHeight() const;

  double nativeCenterX() const;
  double nativeCenterY() const;

public:
  double aspectRatio() const;

  double finalWidth() const;
  double finalHeight() const;

  bool getBoundingBox(double& left, double& top,
							 double& right, double& bottom) const;

  int getDisplayList() const { return itsDisplayList; }

  bool isCurrent() const { return itsIsCurrent; }

  bool getBBVisibility() const { return itsBBIsVisible; }

  double getMaxDimension() const
	 { return max(finalWidth(), finalHeight()); }

  GrObj::ScalingMode getScalingMode() const { return itsScalingMode; }

  GrObj::AlignmentMode getAlignmentMode() const { return itsAlignmentMode; }

  double getCenterX() const { return itsCenterX; }
  double getCenterY() const { return itsCenterY; }

  int getCategory() const { return itsCategory; }

  GrObj::GrObjRenderMode getRenderMode() const { return itsRenderMode; }
  GrObj::GrObjRenderMode getUnRenderMode() const { return itsUnRenderMode; }


  //////////////////
  // manipulators //
  //////////////////

  void setBBVisibility(bool visibility);

  void setScalingMode(GrObj::ScalingMode val);

  void setWidth(double val);
  void setHeight(double val);
  void setAspectRatio(double val);
  void setMaxDimension(double val);
  void setAlignmentMode(GrObj::AlignmentMode val);

  void setCenterX(double val) { itsCenterX = val; }
  void setCenterY(double val) { itsCenterY = val; }

  void setCategory(int val) { itsCategory = val; }

  void setRenderMode(GrObj::GrObjRenderMode mode);
  void setUnRenderMode(GrObj::GrObjRenderMode mode);

  void postUpdated() const { itsIsCurrent = true; }


  /////////////
  // actions //
  /////////////

  void update() const;
  void draw() const;
  void undraw() const;
  void grDrawBoundingBox() const;

  void invalidateCaches();

  // This function deletes any previous display list, allocates a new
  // display list, and checks that the allocation actually succeeded.
  void newList() const;

private:
  void updateBB() const;

  // This function updates the cached OpenGL display list.
  void recompile() const;

  // This function updates the cached bitmap.
  void recacheBitmap() const;

  void doAlignment() const;
  void doScaling() const;


  //////////////////
  // Nested types //
  //////////////////

#if 0
  class Scaler {
	 static auto_ptr<Scaler> createScaler(GrObj::SCALING_MODE mode);

	 virtual void doScaling() const = 0;
	 
  };

  class Aligner {
	 static auto_ptr<Aligner> createAligner(GrObj::ALIGNMENT_MODE mode);

	 virtual void doAlignment() const = 0;
  };

  class Renderer {
	 static auto_ptr<Renderer> createRenderer(GrObj::GROBJ_RENDER_MODE mode);

	 virtual void doRender() const = 0;
  };

  class Unrenderer {
	 static auto_ptr<Unrenderer> createUnrenderer(GrObj::GROBJ_UNRENDER_MODE mode);

	 virtual void doUnrender() const = 0;
  };
#endif

  //////////////////
  // Data members //
  //////////////////
private:
  GrObj* self;

  int itsCategory;

  mutable bool itsIsCurrent;    // true if displaylist is current
  mutable int itsDisplayList;   // OpenGL display list that draws the object

  GrObj::GrObjRenderMode itsRenderMode;
  GrObj::GrObjRenderMode itsUnRenderMode;
  
  mutable bool itsRawBBIsCurrent;

  mutable bool itsHasBB;
  mutable Rect<double> itsCachedRawBB;
  mutable int itsCachedBBPixelBorder;

  mutable bool itsFinalBBIsCurrent;
  mutable Rect<double> itsCachedFinalBB;

  bool itsBBIsVisible;

  Bitmap* itsBitmapCache;

  GrObj::ScalingMode itsScalingMode;
  double itsWidthFactor;
  double itsHeightFactor;

  GrObj::AlignmentMode itsAlignmentMode;
  double itsCenterX;
  double itsCenterY;
};

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl creator definitions
//
///////////////////////////////////////////////////////////////////////

GrObjImpl::GrObjImpl(GrObj* obj) :
  self(obj),
  itsCategory(-1),
  itsIsCurrent(false),
  itsDisplayList(-1),
  itsRenderMode(GrObj::GROBJ_GL_COMPILE),
  itsUnRenderMode(GrObj::GROBJ_SWAP_FORE_BACK),
  itsRawBBIsCurrent(false),
  itsHasBB(false),
  itsCachedRawBB(),
  itsCachedBBPixelBorder(0),
  itsFinalBBIsCurrent(false),
  itsCachedFinalBB(),
  itsBBIsVisible(false),
  itsBitmapCache(0),
  itsScalingMode(GrObj::NATIVE_SCALING),
  itsWidthFactor(1.0),
  itsHeightFactor(1.0),
  itsAlignmentMode(GrObj::NATIVE_ALIGNMENT),
  itsCenterX(0.0),
  itsCenterY(0.0)
{};

GrObjImpl::~GrObjImpl() {
DOTRACE("GrObjImpl::~GrObjImpl");
  glDeleteLists(itsDisplayList, 1);
}

void GrObjImpl::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("GrObjImpl::serialize");

  DebugEvalNL(flag & IO::TYPENAME); 

  if (flag & IO::TYPENAME) { os << "GrObj" << IO::SEP; }

  os << itsCategory << IO::SEP;

  os << itsRenderMode << IO::SEP;
  os << itsUnRenderMode << IO::SEP;

  os << itsBBIsVisible << IO::SEP;

  os << itsScalingMode << IO::SEP;
  os << itsWidthFactor << IO::SEP;
  os << itsHeightFactor << IO::SEP;

  os << itsAlignmentMode << IO::SEP;
  os << itsCenterX << IO::SEP;
  os << itsCenterY << endl;  

  if (os.fail()) throw OutputError("GrObj");

  if (flag & IO::BASES) { /* no bases to serialize */ }
}

void GrObjImpl::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("GrObjImpl::deserialize");

  DebugEvalNL(flag & IO::TYPENAME); 

  if (flag & IO::TYPENAME) { IO::readTypename(is, "GrObj"); }

  is >> itsCategory; if (is.fail()) throw InputError("after GrObj::itsCategory");

  is >> itsRenderMode; if (is.fail()) throw InputError("after GrObj::itsRenderMode");
  is >> itsUnRenderMode; if (is.fail()) throw InputError("after GrObj::itsUnRenderMode");

  int temp;
  is >> temp; if (is.fail()) throw InputError("after GrObj::temp"); itsBBIsVisible = bool(temp);

  is >> itsScalingMode; if (is.fail()) throw InputError("after GrObj::itsScalingMode");
  is >> itsWidthFactor; if (is.fail()) throw InputError("after GrObj::itsWidthFactor");
  is >> itsHeightFactor; if (is.fail()) throw InputError("after GrObj::itsHeightFactor");

  is >> itsAlignmentMode; if (is.fail()) throw InputError("after GrObj::itsAlignmentMode");
  is >> itsCenterX; if (is.fail()) throw InputError("after GrObj::itsCenterX");
  is >> itsCenterY; if (is.fail()) throw InputError("after GrObj::itsCenterY");

  invalidateCaches();

  if (is.fail()) throw InputError("GrObj");

  if (flag & IO::BASES) { /* no bases to deserialize */ }
}

int GrObjImpl::charCount() const {
DOTRACE("GrObjImpl::charCount");
  int count = 
	 gCharCount("GrObj") + 1
	 + gCharCount(itsCategory) + 1
	 
	 + gCharCount(itsRenderMode) + 1
	 + gCharCount(itsUnRenderMode) + 1
	 
	 + gCharCount(itsBBIsVisible) + 1
	 
	 + gCharCount(itsScalingMode) + 1
	 + gCharCount(itsWidthFactor) + 1
	 + gCharCount(itsHeightFactor) + 1
	 
	 + gCharCount(itsAlignmentMode) + 1
	 + gCharCount(itsCenterX) + 1
	 + gCharCount(itsCenterY) + 1
	 + 5; // fudge factor
  DebugEvalNL(count);
  return count;
}

void GrObjImpl::readFrom(Reader* reader) {
DOTRACE("GrObjImpl::readFrom");
}

void GrObjImpl::writeTo(Writer* writer) const {
DOTRACE("GrObjImpl::writeTo");
}


///////////////////////////////////////////////////////////////////////
//
// GrObjImpl accessor definitions
//
///////////////////////////////////////////////////////////////////////

inline const Rect<double>& GrObjImpl::getRawBB() const {
  updateBB();
  return itsCachedRawBB;
}

inline int GrObjImpl::getBBPixelBorder() const {
  updateBB();
  return itsCachedBBPixelBorder;
}

inline bool GrObjImpl::hasBB() const {
  updateBB();
  return itsHasBB;
}

inline double GrObjImpl::aspectRatio() const {
  if ( !hasBB() ) return 1.0;
  return (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
}

inline double GrObjImpl::finalWidth() const {
  return nativeWidth()*itsWidthFactor;
}

inline double GrObjImpl::finalHeight() const {
  return nativeHeight()*itsHeightFactor;
}

inline double GrObjImpl::nativeWidth() const {
  updateBB();
  return itsCachedRawBB.width();
}

inline double GrObjImpl::nativeHeight() const {
  updateBB();
  return itsCachedRawBB.height();;
}

inline double GrObjImpl::nativeCenterX() const {
  updateBB();
  return (itsCachedRawBB.r + itsCachedRawBB.l) / 2.0;
}

inline double GrObjImpl::nativeCenterY() const {
  updateBB();
  return (itsCachedRawBB.t + itsCachedRawBB.b) / 2.0;
}

bool GrObjImpl::getBoundingBox(double& left, double& top,
										 double& right, double& bottom) const {
DOTRACE("GrObjImpl::getBoundingBox");
  if ( !hasBB() ) return false;

  // If we need to recompute the final bounding box, do that here...
  if ( !itsFinalBBIsCurrent ) {
	 left = getRawBB().l;
	 top =  getRawBB().t;
	 right = getRawBB().r;
	 bottom = getRawBB().b;
	 
	 int bp = getBBPixelBorder();  // border pixels
	 
	 // Do the object's internal scaling and alignment, and find the
	 // bounding box in screen coordinates
	 
	 int screen_left, screen_top, screen_right, screen_bottom;
	 
	 glMatrixMode(GL_MODELVIEW);
	 glPushMatrix();
	 {
		doScaling();
		doAlignment();
		
		GrObj::getScreenFromWorld(left, bottom, screen_left, screen_bottom);
		GrObj::getScreenFromWorld(right, top, screen_right, screen_top, false);
	 }
	 glPopMatrix();
	 
	 // Now project back to world coordinates, and add a border of 'bp'
	 // pixels around the edges of the image
	 GrObj::getWorldFromScreen(screen_left-bp, screen_bottom-bp, left, bottom);
	 GrObj::getWorldFromScreen(screen_right+bp, screen_top+bp, right, top, false);
	 
	 itsCachedFinalBB.setRectLTRB(left, top, right, bottom);

	 // This next line is commented out to disable the caching scheme
	 // because I don't think it really works, since changes to the
	 // OpenGL state will screw up a cached copy of the box. What we
	 // need is a way to determine whether the OpenGL state has
	 // changed... but this might be impractical.
//  	 itsFinalBBIsCurrent = true;
  }

  // ...otherwise just return the cached copy
  else {
	 left = itsCachedFinalBB.l;
	 top =  itsCachedFinalBB.t;
	 right = itsCachedFinalBB.r;
	 bottom = itsCachedFinalBB.b;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl manipulator definitions
//
///////////////////////////////////////////////////////////////////////

inline void GrObjImpl::setBBVisibility(bool visibility) {
DOTRACE("GrObjImpl::setBBVisibility");
  itsBBIsVisible = visibility;
}

void GrObjImpl::setScalingMode(GrObj::ScalingMode val) {
DOTRACE("GrObjImpl::setScalingMode");
  if (itsScalingMode == val) return;

  switch (val) {
  case GrObj::NATIVE_SCALING:
	 itsScalingMode = val;

	 itsWidthFactor = 1.0;
	 itsHeightFactor = 1.0;
	 break;

  case GrObj::MAINTAIN_ASPECT_SCALING:
  case GrObj::FREE_SCALING:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsScalingMode = val;
	 break;

  } // end switch
}

void GrObjImpl::setWidth(double val) {
DOTRACE("GrObjImpl::setWidth");
  if (val == 0.0 || val == finalWidth()) return; 
  if (itsScalingMode == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double new_width_factor = val / nativeWidth();
  
  double change_factor = new_width_factor / itsWidthFactor;
	 
  itsWidthFactor = new_width_factor;

  if (itsScalingMode == GrObj::MAINTAIN_ASPECT_SCALING) {
	 itsHeightFactor *= change_factor;
  }
}

void GrObjImpl::setHeight(double val) {
DOTRACE("GrObjImpl::setHeight");
  if (val == 0.0 || val == finalHeight()) return; 
  if (itsScalingMode == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double new_height_factor = val / nativeHeight();

  double change_factor = new_height_factor / itsHeightFactor;

  itsHeightFactor = new_height_factor;

  if (itsScalingMode == GrObj::MAINTAIN_ASPECT_SCALING) {
	 itsWidthFactor *= change_factor;
  }
}

void GrObjImpl::setAspectRatio(double val) {
DOTRACE("GrObjImpl::setAspectRatio");
  if (val == 0.0 || val == aspectRatio()) return; 
  if (itsScalingMode == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double change_factor = val / (aspectRatio());

  // By convention, we change only the width to reflect the change in
  // aspect ratio

  itsWidthFactor *= change_factor;
}

void GrObjImpl::setMaxDimension(double val) {
DOTRACE("GrObjImpl::setMaxDimension");
  if (itsScalingMode == GrObj::NATIVE_SCALING) return;
  if ( !hasBB() ) return;

  double scaling_factor = 1.0;

  // If the width is larger...
  if (finalWidth() > finalHeight()) {
	 scaling_factor = val / (finalWidth());
  }
  // ... or if the height is larger
  else {
	 scaling_factor = val / (finalHeight());
  }

  itsWidthFactor *= scaling_factor;
  itsHeightFactor *= scaling_factor;
}

void GrObjImpl::setAlignmentMode(GrObj::AlignmentMode val) {
DOTRACE("GrObjImpl::setAlignmentMode");
  DebugEval(val); 
  DebugEvalNL(itsAlignmentMode); 

  if (val == itsAlignmentMode) return;

  switch (val) {
  case GrObj::NATIVE_ALIGNMENT:
	 itsAlignmentMode = val;
	 break;

  case GrObj::CENTER_ON_CENTER:
  case GrObj::NW_ON_CENTER:
  case GrObj::NE_ON_CENTER:
  case GrObj::SW_ON_CENTER:
  case GrObj::SE_ON_CENTER:
  case GrObj::ARBITRARY_ON_CENTER:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsAlignmentMode = val;
	 break;

  } // end switch

  DebugEvalNL(itsAlignmentMode); 
}

void GrObjImpl::setRenderMode(GrObj::GrObjRenderMode mode) {
DOTRACE("GrObjImpl::setRenderMode");
  // If mode is the same as the current render mode, then return
  // immediately (and don't send a state change message)
  if (mode == itsRenderMode) return; 

  switch (mode) {
  case GrObj::GROBJ_DIRECT_RENDER:
  case GrObj::GROBJ_GL_COMPILE:
	 itsRenderMode = mode;
	 break;

  case GrObj::GROBJ_GL_BITMAP_CACHE:
  case GrObj::GROBJ_X11_BITMAP_CACHE:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsRenderMode = mode;
	 delete itsBitmapCache;
	 
	 if (GrObj::GROBJ_GL_BITMAP_CACHE == mode) {
		itsBitmapCache = new GLBitmap();
	 }
	 if (GrObj::GROBJ_X11_BITMAP_CACHE == mode) {
		itsBitmapCache = new XBitmap();
	 }
	 
	 break;

  } // end switch
}

void GrObjImpl::setUnRenderMode(GrObj::GrObjRenderMode mode) {
DOTRACE("GrObjImpl::setUnRenderMode");
  // If mode is the same as the current unrender mode, then return
  // immediately (and don't send a state change message)
  if (mode == itsUnRenderMode) return; 

  switch (mode) {
  case GrObj::GROBJ_DIRECT_RENDER:
  case GrObj::GROBJ_SWAP_FORE_BACK:
	 itsUnRenderMode = mode;
	 break;

  case GrObj::GROBJ_CLEAR_BOUNDING_BOX:
	 // These modes require a bounding box
	 if ( !hasBB() ) return;

	 itsUnRenderMode = mode;
	 break;

  } // end switch
}

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl action definitions
//
///////////////////////////////////////////////////////////////////////

void GrObjImpl::updateBB() const {
DOTRACE("GrObjImpl::updateBB");
  DebugEval(itsRawBBIsCurrent);
  if (!itsRawBBIsCurrent) {
	 itsHasBB = self->grGetBoundingBox(itsCachedRawBB.l,
												  itsCachedRawBB.t,
												  itsCachedRawBB.r,
												  itsCachedRawBB.b,
												  itsCachedBBPixelBorder);
	 if (!itsHasBB) {
		itsCachedRawBB.setRectLTRB(0.0, 0.0, 0.0, 0.0);
	 }
	 itsRawBBIsCurrent = true;
  }
  DebugEvalNL(itsHasBB);
}

void GrObjImpl::update() const {
DOTRACE("GrObjImpl::update");
  checkForGlError("before GrObj::update");
  if ( !isCurrent() ) {

	 switch (itsRenderMode) {
	 case GrObj::GROBJ_GL_COMPILE:
		recompile();
		break;
		
	 case GrObj::GROBJ_GL_BITMAP_CACHE:
	 case GrObj::GROBJ_X11_BITMAP_CACHE:
		recacheBitmap();
		break;
	 }
  }
  checkForGlError("during GrObj::update");
}

void GrObjImpl::draw() const {
DOTRACE("GrObjImpl::draw");
  checkForGlError("before GrObj::draw");

  if ( itsBBIsVisible ) {
	 grDrawBoundingBox();
  }

  glMatrixMode(GL_MODELVIEW);

  switch (itsRenderMode) {
  case GrObj::GROBJ_DIRECT_RENDER:
	 glPushMatrix();
		doScaling();
		doAlignment();
		
		self->grRender();
	 glPopMatrix();
	 break;
	 
  case GrObj::GROBJ_GL_COMPILE:
	 if ( !isCurrent() ) {
		recompile();
	 }
	 glPushMatrix();
	   doScaling();
		doAlignment();
		glCallList( itsDisplayList );
	 glPopMatrix();
	 break;
	 
  case GrObj::GROBJ_GL_BITMAP_CACHE:
  case GrObj::GROBJ_X11_BITMAP_CACHE:
	 if ( !isCurrent() ) {
		recacheBitmap();
	 }
	 // If recacheBitmap() was called, then we don't need to draw the
	 // bitmap again since the object was rendered to the screen as
	 // part of recacheBitmap().
	 else {
		Assert(itsBitmapCache != 0);
		glPushMatrix();
	     doScaling();
		  doAlignment();
		  itsBitmapCache->draw();
		glPopMatrix();
	 }
	 break;

  } // end switch

  checkForGlError("during GrObj::draw"); 
}

void GrObjImpl::undraw() const {
DOTRACE("GrObjImpl::undraw");
  checkForGlError("before GrObj::undraw");

  glMatrixMode(GL_MODELVIEW);

  if ( itsUnRenderMode == GrObj::GROBJ_DIRECT_RENDER ) {
	 glPushMatrix();
	 {
		doScaling();
		doAlignment();
		
		self->grUnRender();
	 }
	 glPopMatrix();
  }

  else if ( itsUnRenderMode == GrObj::GROBJ_SWAP_FORE_BACK ) {
	 glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
	 {
		GrObj::swapForeBack();
		
		glPushMatrix();
		{
		  doScaling();
		  doAlignment();
		  
		  if ( itsRenderMode == GrObj::GROBJ_GL_COMPILE ) {
			 // Since we don't do a recompile of the display list here,
			 // we must explicitly check that the display list is valid,
			 // since it might be invalid if the object was recently
			 // constructed, for example.
			 if (glIsList(itsDisplayList) == GL_TRUE) {
				glCallList( itsDisplayList );
				DebugEvalNL(itsDisplayList);
			 }
		  }
		  else {
			 self->grRender();
		  }
		}
		glPopMatrix();
	 }
	 glPopAttrib();
  }

  else if ( itsUnRenderMode == GrObj::GROBJ_CLEAR_BOUNDING_BOX ) {
	 double left, top, right, bottom;
	 if (getBoundingBox(left, top, right, bottom)) {
		glPushAttrib(GL_SCISSOR_BIT);
		{
		  glEnable(GL_SCISSOR_TEST);
		  int screen_left, screen_top, screen_right, screen_bottom;
		  GrObj::getScreenFromWorld(left, top, screen_left, screen_top);
		  GrObj::getScreenFromWorld(right, bottom,
											 screen_right, screen_bottom, false);
		  glScissor(screen_left-1,
						screen_bottom-1,
						screen_right-screen_left+2,
						screen_top-screen_bottom+2);
		  glClear(GL_COLOR_BUFFER_BIT);
		  glDisable(GL_SCISSOR_TEST);
		}
		glPopAttrib();
	 }
  }

  if ( itsBBIsVisible ) {
	 glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
	 {
		GrObj::swapForeBack();

		glPushMatrix();
		{
		  doScaling();
		  doAlignment();
		  
		  grDrawBoundingBox();
		}
		glPopMatrix();
	 }
	 glPopAttrib();
  }

  checkForGlError("during GrObj::undraw");
}

void GrObjImpl::grDrawBoundingBox() const {
DOTRACE("GrObjImpl::grDrawBoundingBox");
  double left, top, right, bottom;
  if ( getBoundingBox(left, top, right, bottom) ) {
	 DebugPrintNL("drawing bounding box");
	 glPushAttrib(GL_LINE_BIT);
	 {
		glLineWidth(1.0);
		glEnable(GL_LINE_STIPPLE);
 		glLineStipple(1, 0x0F0F);

		glBegin(GL_LINE_LOOP);
		  glVertex2d(left, bottom);
		  glVertex2d(right, bottom);
		  glVertex2d(right, top);
		  glVertex2d(left, top);
		glEnd();
	 }
	 glPopAttrib();
  }
}

void GrObjImpl::invalidateCaches() {
DOTRACE("GrObjImpl::invalidateCaches");
  itsIsCurrent = false;
  itsRawBBIsCurrent = false;
  itsFinalBBIsCurrent = false;
}

void GrObjImpl::newList() const {
DOTRACE("GrObjImpl::newList");
  glDeleteLists(itsDisplayList, 1);
  itsDisplayList = glGenLists(1); 
  if (itsDisplayList == 0) {     
	 cerr << "GrObj::newList: couldn't allocate display list\n";
	 throw GrObjError();
  }
}

void GrObjImpl::recompile() const {
DOTRACE("GrObjImpl::recompile");
  newList();
  
  glNewList(getDisplayList(), GL_COMPILE);
  self->grRender();
  glEndList();
  
  postUpdated();
}

void GrObjImpl::recacheBitmap() const {
DOTRACE("GrObjImpl::recacheBitmap");
  Assert(itsBitmapCache != 0);
  
  undraw();
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
	 glPushAttrib(GL_PIXEL_MODE_BIT|GL_COLOR_BUFFER_BIT);
	 {
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		
		doScaling();
		doAlignment();
		
		self->grRender();
		
		Assert(hasBB());
		
		itsBitmapCache->grabWorldRect(getRawBB().l, getRawBB().t,
												getRawBB().r, getRawBB().b);
		itsBitmapCache->setRasterX(getRawBB().l);
		itsBitmapCache->setRasterY(getRawBB().b);
	 }
	 glPopAttrib;
  }
  glPopMatrix();
  
  if (GrObj::GROBJ_X11_BITMAP_CACHE == itsRenderMode) {
	 itsBitmapCache->flipVertical();
	 itsBitmapCache->flipContrast();
  }
  
  postUpdated();
}

void GrObjImpl::doAlignment() const {
DOTRACE("GrObjImpl::doAlignment");
  if (GrObj::NATIVE_ALIGNMENT == itsAlignmentMode) return;
	 
  Assert(hasBB());
	 
  double width = nativeWidth();
  double height = nativeHeight();
	 
  // These indicate where the center of the object will go
  double centerX, centerY;
	 
  switch (itsAlignmentMode) {
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
	 centerX = itsCenterX; centerY = itsCenterY;
	 break;
  }
	 
  glTranslated(centerX-nativeCenterX(),
					centerY-nativeCenterY(),
					0.0);
}

void GrObjImpl::doScaling() const {
DOTRACE("GrObjImpl::doScaling");
  if (GrObj::NATIVE_SCALING == itsScalingMode) return;
	 
  switch (itsScalingMode) {
  case GrObj::MAINTAIN_ASPECT_SCALING:
  case GrObj::FREE_SCALING:
	 glScaled(itsWidthFactor, itsHeightFactor, 1.0);
	 break;
  }
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
  itsImpl(new GrObjImpl(this))
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
  itsImpl(new GrObjImpl(this))
{
DOTRACE("GrObj::GrObj(istream&)");
  deserialize(is, flag);
  itsImpl->newList();

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
  sendStateChangeMsg();
}

void GrObj::writeTo(Writer* writer) const {
DOTRACE("GrObj::writeTo");
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

bool GrObj::getBoundingBox(double& left, double& top,
									double& right, double& bottom) const {
DOTRACE("GrObj::getBoundingBox");

  return itsImpl->getBoundingBox(left, top, right, bottom); 
}

bool GrObj::grGetBoundingBox(double& /*left*/, double& /*top*/,
									  double& /*right*/, double& /*bottom*/,
									  int& /* border_pixels */) const {
DOTRACE("GrObj::grGetBoundingBox");
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

namespace {
  // These matrices are shared by getScreenFromWorld and
  // getWorldFromScreen.
  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];
}

void GrObj::getScreenFromWorld(double world_x, double world_y,
										 int& screen_x, int& screen_y,
										 bool recalculate_state) {
DOTRACE("GrObj::getScreenFromWorld");

  if (recalculate_state) { 
	 glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
	 glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
	 glGetIntegerv(GL_VIEWPORT, current_viewport);
  }

  double temp_screen_x, temp_screen_y, dummy_z;

  GLint status =
	 gluProject(world_x, world_y, 0.0,
					current_mv_matrix, current_proj_matrix, current_viewport,
					&temp_screen_x, &temp_screen_y, &dummy_z);

  DebugEval(status);

  if (status == GL_FALSE)
	 throw ErrorWithMsg("GrObj::getScreenFromWorld(): gluProject error");

  screen_x = int(temp_screen_x);
  screen_y = int(temp_screen_y);
}

void GrObj::getWorldFromScreen(int screen_x, int screen_y,
										 double& world_x, double& world_y,
										 bool recalculate_state) {
DOTRACE("GrObj::getWorldFromScreen");
  if (recalculate_state) {
	 glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
	 glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
	 glGetIntegerv(GL_VIEWPORT, current_viewport);
  }

  double dummy_z;

  GLint status =
	 gluUnProject(screen_x, screen_y, 0,
					  current_mv_matrix, current_proj_matrix, current_viewport,
					  &world_x, &world_y, &dummy_z);

  DebugEval(status);

  if (status == GL_FALSE)
	 throw ErrorWithMsg("GrObj::getWorldFromScreen(): gluUnProject error");
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

void GrObj::swapForeBack() {
DOTRACE("GrObj::swapForeBack");
  if ( GfxAttribs::usingRgba() ) {
	 GLdouble foreground[4];
	 GLdouble background[4];
	 glGetDoublev(GL_CURRENT_COLOR, &foreground[0]);
	 glGetDoublev(GL_COLOR_CLEAR_VALUE, &background[0]);
	 glColor4dv(background);
	 glClearColor(foreground[0], foreground[1],
					  foreground[2], foreground[3]);
  }
  else {
	 GLint foreground, background;
	 glGetIntegerv(GL_CURRENT_INDEX, &foreground);
	 glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
	 glIndexi(background);
	 glClearIndex(foreground);
  }
}

void GrObj::update() const {
  itsImpl->update();
}

void GrObj::draw() const {
  itsImpl->draw();
}

void GrObj::undraw() const {
  itsImpl->undraw();
}

void GrObj::grUnRender() const {
DOTRACE("GrObj::grUnRender");
  // Empty default implementation of this virtual function
}

void GrObj::grDrawBoundingBox() const {
  itsImpl->grDrawBoundingBox();
}

int GrObj::grDisplayList() const {
  return itsImpl->getDisplayList();
}

bool GrObj::grIsCurrent() const {
  return itsImpl->isCurrent();
}

void GrObj::grPostUpdated() const {
  itsImpl->postUpdated();
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
