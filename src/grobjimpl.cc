///////////////////////////////////////////////////////////////////////
//
// grobjimpl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Mar 23 16:27:57 2000
// written: Mon May 22 12:58:17 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_CC_DEFINED
#define GROBJIMPL_CC_DEFINED

#include "grobjimpl.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"

#include "util/error.h"
#include "util/janitor.h"

#include <iostream.h>           // for serialize
#include <GL/gl.h>
#include <GL/glu.h>

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

class GrObjError : public ErrorWithMsg {
public:
  GrObjError(const char* msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

const unsigned long GrObj::Impl::GROBJ_SERIAL_VERSION_ID;

dynamic_string GrObj::Impl::Renderer::BITMAP_CACHE_DIR(".");

namespace {

  GLenum BITMAP_CACHE_BUFFER = GL_FRONT;

#ifdef LOCAL_DEBUG
  inline void checkForGlError(const char* where) throw (GrObjError) {
	 GLenum status = glGetError();
	 if (status != GL_NO_ERROR) {
		const char* msg =
		  reinterpret_cast<const char*>(gluErrorString(status));
		GrObjError err("GL error: ");
		err.appendMsg(msg);
		err.appendMsg(" ");
		err.appendMsg(where);
		throw err;
	 }
  }
#else
#  define checkForGlError(x) {}
#endif
}

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

void GrObj::Impl::BoundingBox::updateFinal(const GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::BoundingBox::updateFinal");

  if ( !itsFinalBBIsCurrent ) {

	 // Do the object's internal scaling and alignment, and find the
	 // bounding box in screen coordinates
	 
	 Rect<int> screen_pos;

	 {
		glMatrixMode(GL_MODELVIEW);

		GWT::Canvas::StateSaver state(canvas);

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



///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl::Renderer definitions
//
///////////////////////////////////////////////////////////////////////

GrObj::Impl::Renderer::Renderer() :
  itsMode(GrObj::GROBJ_GL_COMPILE),
  itsCacheFilename(""),
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
															 GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recompileIfNeeded");
  if (itsIsCurrent) return;
  
  newList();
  
  glNewList(itsDisplayList, GL_COMPILE);
  obj->grRender(canvas);
  glEndList();
  
  postUpdated();
}

bool GrObj::Impl::Renderer::recacheBitmapIfNeeded(const GrObj::Impl* obj,
																  GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::recacheBitmapIfNeeded");
  if (itsIsCurrent) return false;

  Assert(itsBitmapCache.get() != 0);
  
  obj->undraw(canvas);

  Assert(obj->hasBB());

//   Rect<double> bmapbox;
//   obj->getBoundingBox(canvas, bmapbox);

  {
	 glMatrixMode(GL_MODELVIEW);
	 GWT::Canvas::StateSaver state(canvas);

	 glPushAttrib(GL_COLOR_BUFFER_BIT|GL_PIXEL_MODE_BIT);
	 {
		glDrawBuffer(GL_FRONT);
		
		obj->doScaling();
		obj->doAlignment();
		
		obj->grRender(canvas);

		glReadBuffer(GL_FRONT);
 		Rect<double> bmapbox_init = obj->getRawBB();
 		Rect<int> screen_rect = canvas.getScreenFromWorld(bmapbox_init);
 		screen_rect.widenByStep(2);
 		screen_rect.heightenByStep(2);
 		Rect<double> bmapbox = canvas.getWorldFromScreen(screen_rect);

		DebugEval(bmapbox.left()); DebugEval(bmapbox.top());
		DebugEval(bmapbox.right()); DebugEvalNL(bmapbox.bottom());
		itsBitmapCache->grabWorldRect(bmapbox);
		itsBitmapCache->setRasterX(bmapbox.left());
		itsBitmapCache->setRasterY(bmapbox.bottom());
	 }
	 glPopAttrib();
  }

  DebugEvalNL(itsMode);

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

	 queueBitmapLoad();

	 itsMode = new_mode;
	 break;

  } // end switch
}

void GrObj::Impl::Renderer::render(const GrObj::Impl* obj,
											  GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::Renderer::render");
  DebugEvalNL(itsMode);
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
											  GWT::Canvas& canvas) const {
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
  itsBB(this),
  itsScaler(),
  itsAligner(),
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

  if (os.fail()) throw IO::OutputError("GrObj");

  if (flag & IO::BASES) { /* no bases to serialize */ }
}

void GrObj::Impl::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("GrObj::Impl::deserialize");

  DebugEvalNL(flag & IO::TYPENAME); 

  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, "GrObj"); }

  int temp;

  is >> itsCategory; if (is.fail()) throw IO::InputError("after GrObj::itsCategory");

  is >> temp; itsRenderer.setMode(temp, this);
  if (is.fail()) throw IO::InputError("after GrObj::itsRenderer.itsMode");

  is >> itsUnRenderer.itsMode; if (is.fail()) throw IO::InputError("after GrObj::itsUnRenderer.itsMode");

  is >> temp; if (is.fail()) throw IO::InputError("after GrObj::temp"); itsBB.itsIsVisible = bool(temp);

  is >> temp; itsScaler.setMode(temp, this);
  if (is.fail()) throw IO::InputError("after GrObj::itsScaler.itsMode");

  is >> itsScaler.itsWidthFactor; if (is.fail()) throw IO::InputError("after GrObj::itsScaler.itsWidthFactor");
  is >> itsScaler.itsHeightFactor; if (is.fail()) throw IO::InputError("after GrObj::itsScaler.itsHeightFactor");

  is >> itsAligner.itsMode; if (is.fail()) throw IO::InputError("after GrObj::itsAligner.itsMode");
  is >> itsAligner.itsCenterX; if (is.fail()) throw IO::InputError("after GrObj::itsAligner.itsCenterX");
  is >> itsAligner.itsCenterY; if (is.fail()) throw IO::InputError("after GrObj::itsAligner.itsCenterY");

  invalidateCaches();

  if (is.fail()) throw IO::InputError("GrObj");

  if (flag & IO::BASES) { /* no bases to deserialize */ }
}

int GrObj::Impl::charCount() const {
DOTRACE("GrObj::Impl::charCount");
  int count = 
	 IO::gCharCount("GrObj") + 1
	 + IO::gCharCount(itsCategory) + 1
	 
	 + IO::gCharCount(itsRenderer.getMode()) + 1
	 + IO::gCharCount(itsUnRenderer.itsMode) + 1
	 
	 + IO::gCharCount(itsBB.itsIsVisible) + 1
	 
	 + IO::gCharCount(itsScaler.getMode()) + 1
	 + IO::gCharCount(itsScaler.itsWidthFactor) + 1
	 + IO::gCharCount(itsScaler.itsHeightFactor) + 1
	 
	 + IO::gCharCount(itsAligner.itsMode) + 1
	 + IO::gCharCount(itsAligner.itsCenterX) + 1
	 + IO::gCharCount(itsAligner.itsCenterY) + 1
	 + 5; // fudge factor
  DebugEvalNL(count);
  return count;
}

void GrObj::Impl::readFrom(IO::Reader* reader) {
DOTRACE("GrObj::Impl::readFrom");

  unsigned long svid = reader->readSerialVersionId(); 

  reader->readValue("GrObj::category", itsCategory);

  int temp;
  reader->readValue("GrObj::renderMode", temp);
  itsRenderer.setMode(temp, this);

  if (svid >= 1)
	 {
		dynamic_string temp;
		reader->readValue("GrObj::cacheFilename", temp);
		itsRenderer.setCacheFilename(temp);
	 }

  if (svid == 0)
	 reader->readValue("GrObj::unRenderMod", itsUnRenderer.itsMode);
  else if (svid >= 1)
	 reader->readValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

  reader->readValue("GrObj::bbVisibility", itsBB.itsIsVisible);

  reader->readValue("GrObj::scalingMode", temp);
  itsScaler.setMode(temp, this);

  reader->readValue("GrObj::widthFactor", itsScaler.itsWidthFactor);
  reader->readValue("GrObj::heightFactor", itsScaler.itsHeightFactor);

  reader->readValue("GrObj::alignmentMode", itsAligner.itsMode);
  reader->readValue("GrObj::centerX", itsAligner.itsCenterX);
  reader->readValue("GrObj::centerY", itsAligner.itsCenterY);
}

void GrObj::Impl::writeTo(IO::Writer* writer) const {
DOTRACE("GrObj::Impl::writeTo");
  writer->writeValue("GrObj::category", itsCategory);

  writer->writeValue("GrObj::renderMode", itsRenderer.getMode());

  if (GROBJ_SERIAL_VERSION_ID >= 1)
	 writer->writeValue("GrObj::cacheFilename", itsRenderer.getCacheFilename());

  if (GROBJ_SERIAL_VERSION_ID == 0)
	 writer->writeValue("GrObj::unRenderMod", itsUnRenderer.itsMode);
  else if (GROBJ_SERIAL_VERSION_ID >= 1)
	 writer->writeValue("GrObj::unRenderMode", itsUnRenderer.itsMode);

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

bool GrObj::Impl::getBoundingBox(const GWT::Canvas& canvas,
											Rect<double>& bbox) const {
DOTRACE("GrObj::Impl::getBoundingBox");

  if ( !itsBB.bbExists() ) return false;

  bbox = itsBB.getFinal(canvas);
  DebugEval(bbox.left()); DebugEval(bbox.top());
  DebugEval(bbox.right()); DebugEvalNL(bbox.bottom());
  return true;
}

///////////////////////////////////////////////////////////////////////
//
// GrObj::Impl manipulator definitions
//
///////////////////////////////////////////////////////////////////////

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

void GrObj::Impl::draw(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::draw");
  checkForGlError("before GrObj::draw");

  if ( itsBB.itsIsVisible ) {
	 grDrawBoundingBox(canvas);
  }

  bool objectDrawn = itsRenderer.update(this, canvas);

  if ( !objectDrawn ) {

	 {
		glMatrixMode(GL_MODELVIEW);

		GWT::Canvas::StateSaver state(canvas);

	   doScaling();
		doAlignment();

		itsRenderer.render(this, canvas);
	 }
  }

  checkForGlError("during GrObj::draw"); 
}

void GrObj::Impl::saveBitmapCache(
  GWT::Canvas& canvas, const char* filename
  ) {
DOTRACE("GrObj::Impl::Renderer::saveBitmapCache");

  Util::Janitor<Impl, int> rmj(*this, &Impl::getRenderMode,
										 &Impl::setRenderMode);
  Util::Janitor<Impl, bool> bbj(*this, &Impl::getBBVisibility,
										  &Impl::setBBVisibility);

  setRenderMode(GrObj::GROBJ_X11_BITMAP_CACHE);
  setBBVisibility(false);

  itsRenderer.setCacheFilename("");

  itsRenderer.saveBitmapCache(this, canvas, filename);

  invalidateCaches();
}

void GrObj::Impl::undraw(GWT::Canvas& canvas) const {
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

void GrObj::Impl::grDrawBoundingBox(const GWT::Canvas& canvas) const {
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

void GrObj::Impl::undrawDirectRender(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawDirectRender");
  glMatrixMode(GL_MODELVIEW);

  GWT::Canvas::StateSaver state(canvas);

  doScaling();
  doAlignment();
	 
  self->grUnRender(canvas);
}

void GrObj::Impl::undrawSwapForeBack(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawSwapForeBack");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 canvas.swapForeBack();
	 
	 {
		glMatrixMode(GL_MODELVIEW);

		GWT::Canvas::StateSaver state(canvas);

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

void GrObj::Impl::undrawClearBoundingBox(GWT::Canvas& canvas) const {
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

void GrObj::Impl::undrawBoundingBox(GWT::Canvas& canvas) const {
DOTRACE("GrObj::Impl::undrawBoundingBox");
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  {
	 canvas.swapForeBack();

	 {
		glMatrixMode(GL_MODELVIEW);

		GWT::Canvas::StateSaver state(canvas);

		doScaling();
		doAlignment();
		  
		grDrawBoundingBox(canvas);
	 }
  }
  glPopAttrib();
}

static const char vcid_grobjimpl_cc[] = "$Header$";
#endif // !GROBJIMPL_CC_DEFINED
