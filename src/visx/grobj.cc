///////////////////////////////////////////////////////////////////////
//
// grobj.cc
// Rob Peters 
// created: Dec-98
// written: Sat Sep 23 14:22:51 2000
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
GrObj::GrObj(istream& is, IO::IOFlag flag) :
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
void GrObj::serialize(ostream& os, IO::IOFlag flag) const {
DOTRACE("GrObj::serialize");
  itsImpl->serialize(os, flag);
}

void GrObj::deserialize(istream& is, IO::IOFlag flag) {
DOTRACE("GrObj::deserialize");

  itsImpl->deserialize(is, flag); 
  sendStateChangeMsg();
}

unsigned long GrObj::serialVersionId() const {
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

void GrObj::grUnRender(GWT::Canvas&) const {
DOTRACE("GrObj::grUnRender");
  // Empty default implementation of this virtual function
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
