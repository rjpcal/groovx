///////////////////////////////////////////////////////////////////////
//
// grobj.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Fri Jun  1 14:21:34 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJ_CC_DEFINED
#define GROBJ_CC_DEFINED

#include "grobj.h"

#include "grobjimpl.h"

#define DYNAMIC_TRACE_EXPR GrObj::tracer.status()
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

Util::Tracer GrObj::tracer;


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

  DebugEval((void*)this); DebugEvalNL((void*)itsImpl);

  setRenderMode(render_mode);
  setUnRenderMode(unrender_mode);

  // The GrObj needs to observe itself in order to update its display
  // list according to state changes.
  attach(this);

  // This is necessary because any representations that have been
  // cached during the GrObj constructor will become invalid upon
  // return to the derived class constructor.
  sendStateChangeMsg();
}

// GrObj destructor
GrObj::~GrObj() {
DOTRACE("GrObj::~GrObj");
  detach(this); 
  delete itsImpl;
}

IO::VersionId GrObj::serialVersionId() const {
DOTRACE("GrObj::Impl::serialVersionId");
  return itsImpl->serialVersionId(); 
}

void GrObj::readFrom(IO::Reader* reader) {
DOTRACE("GrObj::readFrom");

  itsImpl->readFrom(reader);
  sendStateChangeMsg();
}

void GrObj::writeTo(IO::Writer* writer) const {
DOTRACE("GrObj::writeTo");

  itsImpl->writeTo(writer);
}

///////////////
// accessors //
///////////////

bool GrObj::getBBVisibility() const {
DOTRACE("GrObj::getBBVisibility");
  return itsImpl->getBBVisibility();
}

bool GrObj::getBoundingBox(const GWT::Canvas& canvas, Rect<double>& bbox) const {
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

int GrObj::category() const {
DOTRACE("GrObj::category");
  return itsImpl->category();
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
									
void GrObj::setBitmapCacheDir(const char* dirname) {
DOTRACE("GrObj::setBitmapCacheDir");
  Impl::setBitmapCacheDir(dirname); 
}

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

#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (mode == GROBJ_GL_COMPILE) mode = GROBJ_DIRECT_RENDER; 
#endif

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
  DebugEval((void*)this);
  DebugEval((void*)dynamic_cast<GrObj*>(this));
  DebugEval((void*)dynamic_cast<Observer*>(this));
  DebugEval((void*)dynamic_cast<Observable*>(this));
  DebugEvalNL((void*)obj);
  if (obj == this) {
	 DebugEval((void*)this); DebugEvalNL((void*)itsImpl);
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

void GrObj::saveBitmapCache(GWT::Canvas& canvas, const char* filename) const {
  itsImpl->saveBitmapCache(canvas, filename);
}

void GrObj::restoreBitmapCache() const {
  itsImpl->restoreBitmapCache();
}

void GrObj::update(GWT::Canvas& canvas) const {
  itsImpl->update(canvas);
}

void GrObj::draw(GWT::Canvas& canvas) const {
  itsImpl->draw(canvas);
}

void GrObj::undraw(GWT::Canvas& canvas) const {
  itsImpl->undraw(canvas);
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
