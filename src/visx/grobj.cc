///////////////////////////////////////////////////////////////////////
//
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Tue Oct 12 15:01:23 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <GL/glu.h>

#include "glbitmap.h"
#include "xbitmap.h"
#include "error.h"
#include "rect.h"

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
  GrObjImpl(GrObj* obj) :
	 self(obj),
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

  ~GrObjImpl() {}

  GrObj* self;

  mutable bool itsIsCurrent;    // true if displaylist is current
  mutable int itsDisplayList;   // OpenGL display list that draws the object

  GrObj::GrObjRenderMode itsRenderMode;
  GrObj::GrObjRenderMode itsUnRenderMode;
  
  bool itsRawBBIsCurrent;

private:
  bool itsHasBB;
  Rect<double> itsCachedRawBB;
  int itsCachedBBPixelBorder;

  void updateBB() {
  DOTRACE("GrObjImpl::updateBB");
    DebugEval(itsRawBBIsCurrent);
	 if (!itsRawBBIsCurrent) {
		itsHasBB = self->grGetBoundingBox(itsCachedRawBB.l,
													 itsCachedRawBB.t,
													 itsCachedRawBB.r,
													 itsCachedRawBB.b,
													 itsCachedBBPixelBorder);
		itsRawBBIsCurrent = true;
	 }
	 DebugEvalNL(itsHasBB);
  }

public:
  const Rect<double>& getRawBB() {
	 updateBB();
	 return itsCachedRawBB;
  }

  int getBBPixelBorder() {
	 updateBB();
	 return itsCachedBBPixelBorder;
  }

  bool hasBB() {
	 updateBB();
	 return itsHasBB;
  }

  bool itsFinalBBIsCurrent;
  Rect<double> itsCachedFinalBB;

  bool itsBBIsVisible;

  Bitmap* itsBitmapCache;

  GrObj::ScalingMode itsScalingMode;
  double itsWidthFactor;
  double itsHeightFactor;

  double aspectRatio() {
	 return (itsHeightFactor != 0.0 ? itsWidthFactor/itsHeightFactor : 0.0);
  }

  double finalWidth() {
	 return nativeWidth()*itsWidthFactor;
  }

  double finalHeight() {
	 return nativeHeight()*itsHeightFactor;
  }

  double nativeWidth() {
	 updateBB();
	 return itsCachedRawBB.r - itsCachedRawBB.l;
  }

  double nativeHeight() {
	 updateBB();
	 return itsCachedRawBB.t - itsCachedRawBB.b;
  }

  double nativeCenterX() {
	 updateBB();
	 return (itsCachedRawBB.r + itsCachedRawBB.l) / 2.0;
  }

  double nativeCenterY() {
	 updateBB();
	 return (itsCachedRawBB.t + itsCachedRawBB.b) / 2.0;
  }

  GrObj::AlignmentMode itsAlignmentMode;
  double itsCenterX;
  double itsCenterY;
};

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
  itsImpl->itsDisplayList = glGenLists(1);

  attach(this);

  sendStateChangeMsg();
}

// GrObj destructor
GrObj::~GrObj() {
DOTRACE("GrObj::~GrObj");
  detach(this); 
  glDeleteLists(itsImpl->itsDisplayList, 1);
  delete itsImpl;
}

// write the object's state to an output stream. The output stream must
// already be open and connected to an appropriate file.
void GrObj::serialize(ostream&, IOFlag) const {
DOTRACE("GrObj::serialize");
}

void GrObj::deserialize(istream&, IOFlag) {
DOTRACE("GrObj::deserialize");
}

int GrObj::charCount() const {
DOTRACE("GrObj::charCount");
  return 0;
}

///////////////
// accessors //
///////////////

bool GrObj::getBBVisibility() const {
DOTRACE("GrObj::getBBVisibility");
  return itsImpl->itsBBIsVisible;
}

bool GrObj::getBoundingBox(double& left, double& top,
									double& right, double& bottom) const {
DOTRACE("GrObj::getBoundingBox");

  if ( !itsImpl->hasBB() ) return false;

  // If we need to recompute the final bounding box, do that here...
  if ( !itsImpl->itsFinalBBIsCurrent ) {
	 left = itsImpl->getRawBB().l;
	 top =  itsImpl->getRawBB().t;
	 right = itsImpl->getRawBB().r;
	 bottom = itsImpl->getRawBB().b;
	 
	 int bp = itsImpl->getBBPixelBorder();  // border pixels
	 
	 // Do the object's internal scaling and alignment, and find the
	 // bounding box in screen coordinates
	 
	 int screen_left, screen_top, screen_right, screen_bottom;
	 
	 glMatrixMode(GL_MODELVIEW);
	 glPushMatrix();
	 {
		grDoScaling();
		grDoAlignment();
		
		getScreenFromWorld(left, bottom, screen_left, screen_bottom);
		getScreenFromWorld(right, top, screen_right, screen_top, false);
	 }
	 glPopMatrix();
	 
	 // Now project back to world coordinates, and add a border of 'bp'
	 // pixels around the edges of the image
	 getWorldFromScreen(screen_left-bp, screen_bottom-bp, left, bottom);
	 getWorldFromScreen(screen_right+bp, screen_top+bp, right, top, false);
	 
	 itsImpl->itsCachedFinalBB.l = left;
	 itsImpl->itsCachedFinalBB.t = top;
	 itsImpl->itsCachedFinalBB.r = right;
	 itsImpl->itsCachedFinalBB.b = bottom;

	 // This next line is commented out to disable the caching scheme
	 // because I don't think it really works, since changes to the
	 // OpenGL state will screw up a cached copy of the box. What we
	 // need is a way to determine whether the OpenGL state has
	 // changed... but this might be impractical.
//  	 itsFinalBBIsCurrent = true;
  }

  // ...otherwise just return the cached copy
  else {
	 left = itsImpl->itsCachedFinalBB.l;
	 top =  itsImpl->itsCachedFinalBB.t;
	 right = itsImpl->itsCachedFinalBB.r;
	 bottom = itsImpl->itsCachedFinalBB.b;
  }

  return true;
}

bool GrObj::grGetBoundingBox(double& /*left*/, double& /*top*/,
									  double& /*right*/, double& /*bottom*/,
									  int& /* border_pixels */) const {
DOTRACE("GrObj::grGetBoundingBox");
  return false;
}

GrObj::ScalingMode GrObj::getScalingMode() const {
DOTRACE("GrObj::getScalingMode");
  return itsImpl->itsScalingMode; 
}

double GrObj::getWidth() const {
DOTRACE("GrObj::getWidth");
  if (!itsImpl->hasBB()) return 0.0;

  return itsImpl->finalWidth();
}

double GrObj::getHeight() const {
DOTRACE("GrObj::getHeight");
  if (!itsImpl->hasBB()) return 0.0;

  return itsImpl->finalHeight();
}

double GrObj::getAspectRatio() const {
DOTRACE("GrObj::getAspectRatio");
  if (!itsImpl->hasBB()) return 0.0;

  return itsImpl->aspectRatio();
}

double GrObj::getMaxDimension() const {
DOTRACE("GrObj::getMaxDimension");
  if (!itsImpl->hasBB()) return 0.0;

  return max(itsImpl->finalWidth(), itsImpl->finalHeight()); 
}

GrObj::AlignmentMode GrObj::getAlignmentMode() const {
DOTRACE("GrObj::getAlignmentMode");
  return itsImpl->itsAlignmentMode; 
}

double GrObj::getCenterX() const {
DOTRACE("GrObj::getCenterX");
  return itsImpl->itsCenterX;
}

double GrObj::getCenterY() const {
DOTRACE("GrObj::getCenterY");
  return itsImpl->itsCenterY;
}

int GrObj::getCategory() const {
DOTRACE("GrObj::getCategory");
  return -1;
}

GrObj::GrObjRenderMode GrObj::getRenderMode() const {
DOTRACE("GrObj::getRenderMode");
  return itsImpl->itsRenderMode; 
}

GrObj::GrObjRenderMode GrObj::getUnRenderMode() const {
DOTRACE("GrObj::getUnRenderMode");
  return itsImpl->itsUnRenderMode; 
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
DOTRACE("GrObj::setBBVisibility");
  itsImpl->itsBBIsVisible = visibility;
}

void GrObj::setScalingMode(ScalingMode val) {
DOTRACE("GrObj::setScalingMode");
  if (itsImpl->itsScalingMode == val) return;

  switch (val) {
  case NATIVE_SCALING:
	 itsImpl->itsScalingMode = val;

	 itsImpl->itsWidthFactor = 1.0;
	 itsImpl->itsHeightFactor = 1.0;
	 break;

  case MAINTAIN_ASPECT_SCALING:
  case FREE_SCALING:
	 // These modes require a bounding box
	 if (!itsImpl->hasBB()) return;

	 itsImpl->itsScalingMode = val;
	 break;

  } // end switch

  sendStateChangeMsg();
}

void GrObj::setWidth(double val) {
DOTRACE("GrObj::setWidth");
  if (val == 0.0 || val == itsImpl->finalWidth()) return; 
  if (itsImpl->itsScalingMode == NATIVE_SCALING) return;
  if (!itsImpl->hasBB()) return;

  double new_width_factor = val / itsImpl->nativeWidth();
  
  double change_factor = new_width_factor / itsImpl->itsWidthFactor;
	 
  itsImpl->itsWidthFactor = new_width_factor;

  if (itsImpl->itsScalingMode == MAINTAIN_ASPECT_SCALING) {
	 itsImpl->itsHeightFactor *= change_factor;
  }

  sendStateChangeMsg();
}

void GrObj::setHeight(double val) {
DOTRACE("GrObj::setHeight");
  if (val == 0.0 || val == itsImpl->finalHeight()) return; 
  if (itsImpl->itsScalingMode == NATIVE_SCALING) return;
  if (!itsImpl->hasBB()) return;

  double new_height_factor = val / itsImpl->nativeHeight();

  double change_factor = new_height_factor / itsImpl->itsHeightFactor;

  itsImpl->itsHeightFactor = new_height_factor;

  if (itsImpl->itsScalingMode == MAINTAIN_ASPECT_SCALING) {
	 itsImpl->itsWidthFactor *= change_factor;
  }

  sendStateChangeMsg();
}

void GrObj::setAspectRatio(double val) {
DOTRACE("GrObj::setAspectRatio");
  if (val == 0.0 || val == itsImpl->aspectRatio()) return; 
  if (itsImpl->itsScalingMode == NATIVE_SCALING) return;
  if (!itsImpl->hasBB()) return;

  double change_factor = val / (itsImpl->aspectRatio());

  // By convention, we change only the width to reflect the change in
  // aspect ratio

  itsImpl->itsWidthFactor *= change_factor;
  
  sendStateChangeMsg();
}

void GrObj::setMaxDimension(double val) {
DOTRACE("GrObj::setMaxDimension");
  if (!itsImpl->hasBB()) return;
  if (itsImpl->itsScalingMode == NATIVE_SCALING) return;

  double scaling_factor = 1.0;

  // If the width is larger...
  if (itsImpl->finalWidth() > itsImpl->finalHeight()) {
	 scaling_factor = val / (itsImpl->finalWidth());
  }
  // ... or if the height is larger
  else {
	 scaling_factor = val / (itsImpl->finalHeight());
  }

  itsImpl->itsWidthFactor *= scaling_factor;
  itsImpl->itsHeightFactor *= scaling_factor;

  sendStateChangeMsg();
}

void GrObj::setAlignmentMode(AlignmentMode val) {
DOTRACE("GrObj::setAlignmentMode");
  DebugEval(val); 
  DebugEvalNL(itsImpl->itsAlignmentMode); 

  if (val == itsImpl->itsAlignmentMode) return;

  switch (val) {
  case NATIVE_ALIGNMENT:
	 itsImpl->itsAlignmentMode = val;
	 break;

  case CENTER_ON_CENTER:
  case NW_ON_CENTER:
  case NE_ON_CENTER:
  case SW_ON_CENTER:
  case SE_ON_CENTER:
  case ARBITRARY_ON_CENTER:
	 // These modes require a bounding box
	 if (!itsImpl->hasBB()) return;

	 itsImpl->itsAlignmentMode = val;
	 break;

  } // end switch

  sendStateChangeMsg();

  DebugEvalNL(itsImpl->itsAlignmentMode); 
}

void GrObj::setCenterX(double val) {
DOTRACE("GrObj::setCenterX");
  itsImpl->itsCenterX = val;

  sendStateChangeMsg();
}

void GrObj::setCenterY(double val) {
DOTRACE("GrObj::setCenterY");
  itsImpl->itsCenterY = val; 

  sendStateChangeMsg();
}

void GrObj::setCategory(int) {
DOTRACE("GrObj::setCategory");
}

void GrObj::setRenderMode(GrObjRenderMode mode) {
DOTRACE("GrObj::setRenderMode");
  // If mode is the same as the current render mode, then return
  // immediately (and don't send a state change message)
  if (mode == itsImpl->itsRenderMode) return; 

  switch (mode) {
  case GROBJ_DIRECT_RENDER:
  case GROBJ_GL_COMPILE:
	 itsImpl->itsRenderMode = mode;
	 sendStateChangeMsg();
	 break;

  case GROBJ_GL_BITMAP_CACHE:
  case GROBJ_X11_BITMAP_CACHE:
	 // These modes require a bounding box
	 if (!itsImpl->hasBB()) return;

	 itsImpl->itsRenderMode = mode;
	 delete itsImpl->itsBitmapCache;
	 
	 if (GROBJ_GL_BITMAP_CACHE == mode) {
		itsImpl->itsBitmapCache = new GLBitmap();
	 }
	 if (GROBJ_X11_BITMAP_CACHE == mode) {
		itsImpl->itsBitmapCache = new XBitmap();
	 }
	 
	 sendStateChangeMsg();
	 break;

  } // end switch
}

void GrObj::setUnRenderMode(GrObjRenderMode mode) {
DOTRACE("GrObj::setUnRenderMode");
  // If mode is the same as the current unrender mode, then return
  // immediately (and don't send a state change message)
  if (mode == itsImpl->itsUnRenderMode) return; 

  switch (mode) {
  case GROBJ_DIRECT_RENDER:
  case GROBJ_SWAP_FORE_BACK:
	 itsImpl->itsUnRenderMode = mode;
	 sendStateChangeMsg();
	 break;

  case GROBJ_CLEAR_BOUNDING_BOX:
	 // These modes require a bounding box
	 if (!itsImpl->hasBB()) return;

	 itsImpl->itsUnRenderMode = mode;
	 sendStateChangeMsg();
	 break;

  } // end switch
}

void GrObj::receiveStateChangeMsg(const Observable* obj) {
DOTRACE("GrObj::receiveStateChangeMsg");
  if (obj == this) {
	 itsImpl->itsIsCurrent = false;
	 itsImpl->itsRawBBIsCurrent = false;
	 itsImpl->itsFinalBBIsCurrent = false;
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

void GrObj::update() const {
DOTRACE("GrObj::update");
  checkForGlError("before GrObj::update");
  if ( !grIsCurrent() ) {

	 switch (itsImpl->itsRenderMode) {
	 case GROBJ_GL_COMPILE:
		grRecompile();
		break;
		
	 case GROBJ_GL_BITMAP_CACHE:
	 case GROBJ_X11_BITMAP_CACHE:
		grRecacheBitmap();
		break;
	 }
  }
  checkForGlError("during GrObj::update");
}

void GrObj::draw() const {
DOTRACE("GrObj::draw");
  checkForGlError("before GrObj::draw");

  if ( itsImpl->itsBBIsVisible ) {
	 grDrawBoundingBox();
  }

  glMatrixMode(GL_MODELVIEW);

  switch (itsImpl->itsRenderMode) {
  case GROBJ_DIRECT_RENDER:
	 glPushMatrix();
		grDoScaling();
		grDoAlignment();
		
		grRender();
	 glPopMatrix();
	 break;
	 
  case GROBJ_GL_COMPILE:
	 if ( !grIsCurrent() ) { grRecompile(); }
	 glPushMatrix();
	   grDoScaling();
		grDoAlignment();
		glCallList( itsImpl->itsDisplayList );
	 glPopMatrix();
	 break;
	 
  case GROBJ_GL_BITMAP_CACHE:
  case GROBJ_X11_BITMAP_CACHE:
	 if ( !grIsCurrent() ) {
		grRecacheBitmap();
	 }
	 // If grRecacheBitmap() was called, then we don't need to draw the
	 // bitmap again since the object was rendered to the screen as
	 // part of grRecacheBitmap().
	 else {
		Assert(itsImpl->itsBitmapCache != 0);
		glPushMatrix();
	     grDoScaling();
		  grDoAlignment();
		  itsImpl->itsBitmapCache->draw();
		glPopMatrix();
	 }
	 break;

  } // end switch

  checkForGlError("during GrObj::draw");
}

void GrObj::undraw() const {
DOTRACE("GrObj::undraw");
  checkForGlError("before GrObj::undraw");

  glMatrixMode(GL_MODELVIEW);

  if ( itsImpl->itsUnRenderMode == GROBJ_DIRECT_RENDER ) {
	 glPushMatrix();
	 {
		grDoScaling();
		grDoAlignment();
		
		grUnRender();
	 }
	 glPopMatrix();
  }

  else if ( itsImpl->itsUnRenderMode == GROBJ_SWAP_FORE_BACK ) {
	 GLint foreground, background;
	 glGetIntegerv(GL_CURRENT_INDEX, &foreground);
	 glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
	 glIndexi(background);
	 glClearIndex(foreground);
	 
	 glPushMatrix();
	 {
		grDoScaling();
		grDoAlignment();
		
		if ( itsImpl->itsRenderMode == GROBJ_GL_COMPILE ) {
		  // Since we don't do a recompile of the display list here, we must
		  // explicitly check that the display list is valid, since it might
		  // be invalid if the object was recently constructed, for example.
		  if (glIsList(itsImpl->itsDisplayList) == GL_TRUE) {
			 glCallList( itsImpl->itsDisplayList ); DebugEvalNL(itsImpl->itsDisplayList);
		  }
		}
		else {
		  grRender();
		}
	 }
	 glPopMatrix();
	 
	 glIndexi(foreground);
	 glClearIndex(background);
  }

  else if ( itsImpl->itsUnRenderMode == GROBJ_CLEAR_BOUNDING_BOX ) {
	 double left, top, right, bottom;
	 if (getBoundingBox(left, top, right, bottom)) {
		glPushAttrib(GL_SCISSOR_BIT);
		{
		  glEnable(GL_SCISSOR_TEST);
		  int screen_left, screen_top, screen_right, screen_bottom;
		  getScreenFromWorld(left, top, screen_left, screen_top);
		  getScreenFromWorld(right, bottom, screen_right, screen_bottom, false);
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

  if ( itsImpl->itsBBIsVisible ) {
	 GLint foreground, background;
	 glGetIntegerv(GL_CURRENT_INDEX, &foreground);
	 glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
	 glIndexi(background);
	 glClearIndex(foreground);
  
	 glPushMatrix();
	 {
		grDoScaling();
		grDoAlignment();
		
		grDrawBoundingBox();
	 }
	 glPopMatrix();

	 glIndexi(foreground);
	 glClearIndex(background);
  }

  checkForGlError("during GrObj::undraw");
}

void GrObj::grUnRender() const {
DOTRACE("GrObj::grUnRender");
}

void GrObj::grDrawBoundingBox() const {
DOTRACE("GrObj::grDrawBoundingBox")
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

int GrObj::grDisplayList() const {
DOTRACE("GrObj::grDisplayList");
  return itsImpl->itsDisplayList; 
}

void GrObj::grRecompile() const {
DOTRACE("GrObj::grRecompile");
  grNewList();

  glNewList(grDisplayList(), GL_COMPILE);
    grRender();
  glEndList();

  grPostUpdated();
}

void GrObj::grRecacheBitmap() const {
DOTRACE("GrObj::grRecacheBitmap");
  Assert(itsImpl->itsBitmapCache != 0);
		
  undraw();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
	 glPushAttrib(GL_PIXEL_MODE_BIT|GL_COLOR_BUFFER_BIT);
	 {
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);

		grDoScaling();
		grDoAlignment();
		
		grRender();
		
		Assert(itsImpl->hasBB());
		
		itsImpl->itsBitmapCache->grabWorldRect(itsImpl->getRawBB().l,
															itsImpl->getRawBB().t,
															itsImpl->getRawBB().r,
															itsImpl->getRawBB().b);
		itsImpl->itsBitmapCache->setRasterX(itsImpl->getRawBB().l);
		itsImpl->itsBitmapCache->setRasterY(itsImpl->getRawBB().b);
	 }
	 glPopAttrib;
  }
  glPopMatrix();

  if (GROBJ_X11_BITMAP_CACHE == itsImpl->itsRenderMode) {
	 itsImpl->itsBitmapCache->flipVertical();
	 itsImpl->itsBitmapCache->flipContrast();
  }
  
  grPostUpdated();
}

bool GrObj::grIsCurrent() const {
DOTRACE("GrObj::grIsCurrent");
  return itsImpl->itsIsCurrent;
}

void GrObj::grPostUpdated() const {
DOTRACE("GrObj::grPostUpdated"); 
  itsImpl->itsIsCurrent = true; 
}

// get rid of old display list, allocate a new display list, and
// check that the allocation actually succeeded
void GrObj::grNewList() const {
DOTRACE("GrObj::grNewList");
  glDeleteLists(itsImpl->itsDisplayList, 1);
  itsImpl->itsDisplayList = glGenLists(1); 
  if (itsImpl->itsDisplayList == 0) {     
	 cerr << "GrObj::grNewList: couldn't allocate display list\n";
	 throw GrObjError();
  }
}

void GrObj::grDoAlignment() const {
DOTRACE("GrObj::grDoAlignment");
  if (itsImpl->itsAlignmentMode == NATIVE_ALIGNMENT) return;
	 
  Assert(itsImpl->hasBB());
	 
  double width = itsImpl->nativeWidth();
  double height = itsImpl->nativeHeight();
	 
  // These indicate where the center of the object will go
  double centerX, centerY;
	 
  switch (itsImpl->itsAlignmentMode) {
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
	 centerX = itsImpl->itsCenterX; centerY = itsImpl->itsCenterY;
	 break;
  }
	 
  glTranslated(centerX-itsImpl->nativeCenterX(),
					centerY-itsImpl->nativeCenterY(),
					0.0);
}
  
void GrObj::grDoScaling() const {
DOTRACE("GrObj::grDoScaling");
  if (itsImpl->itsScalingMode == NATIVE_SCALING) return;
	 
  switch (itsImpl->itsScalingMode) {
  case GrObj::MAINTAIN_ASPECT_SCALING:
  case GrObj::FREE_SCALING:
	 glScaled(itsImpl->itsWidthFactor, itsImpl->itsHeightFactor, 1.0);
	 break;
  }
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
