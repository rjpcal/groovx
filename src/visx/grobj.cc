///////////////////////////////////////////////////////////////////////
//
// grobj.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Wed Jul 18 18:27:52 2001
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

const GrObj::RenderMode GrObj::DIRECT_RENDER;
const GrObj::RenderMode GrObj::GLCOMPILE;
const GrObj::RenderMode GrObj::GL_BITMAP_CACHE;
const GrObj::RenderMode GrObj::X11_BITMAP_CACHE;
const GrObj::RenderMode GrObj::SWAP_FORE_BACK;
const GrObj::RenderMode GrObj::CLEAR_BOUNDING_BOX;


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
GrObj::GrObj(GrObj::RenderMode render_mode,
             GrObj::RenderMode unrender_mode) :
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

void GrObj::getBoundingBox(const GWT::Canvas& canvas, Rect<double>& bbox) const
{
DOTRACE("GrObj::getBoundingBox");

  itsImpl->getBoundingBox(canvas, bbox);
}

GrObj::ScalingMode GrObj::getScalingMode() const
{
DOTRACE("GrObj::getScalingMode");
  return itsImpl->itsScaler.getMode();
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
  return itsImpl->itsScaler.aspectRatio();
}

double GrObj::getMaxDimension() const {
DOTRACE("GrObj::getMaxDimension");
  return itsImpl->getMaxDimension();
}

GrObj::AlignmentMode GrObj::getAlignmentMode() const {
DOTRACE("GrObj::getAlignmentMode");
  return itsImpl->itsAligner.getMode();
}

double GrObj::getCenterX() const {
DOTRACE("GrObj::getCenterX");
  return itsImpl->itsAligner.itsCenter.x();
}

double GrObj::getCenterY() const {
DOTRACE("GrObj::getCenterY");
  return itsImpl->itsAligner.itsCenter.y();
}

int GrObj::category() const {
DOTRACE("GrObj::category");
  return itsImpl->category();
}

GrObj::RenderMode GrObj::getRenderMode() const {
  return itsImpl->getRenderMode();
}

GrObj::RenderMode GrObj::getUnRenderMode() const {
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

void GrObj::setScalingMode(ScalingMode val)
{
DOTRACE("GrObj::setScalingMode");

  itsImpl->itsScaler.setMode(val);
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

  itsImpl->itsScaler.setAspectRatio(val);
  sendStateChangeMsg();
}

void GrObj::setMaxDimension(double val) {
DOTRACE("GrObj::setMaxDimension");

  itsImpl->setMaxDimension(val);
  sendStateChangeMsg();
}

void GrObj::setAlignmentMode(AlignmentMode val) {
DOTRACE("GrObj::setAlignmentMode");

  itsImpl->itsAligner.setMode(val);
  sendStateChangeMsg();
}

void GrObj::setCenterX(double val) {
DOTRACE("GrObj::setCenterX");

  itsImpl->itsAligner.itsCenter.x() = val;
  sendStateChangeMsg();
}

void GrObj::setCenterY(double val) {
DOTRACE("GrObj::setCenterY");

  itsImpl->itsAligner.itsCenter.y() = val;
  sendStateChangeMsg();
}

void GrObj::setCategory(int val) {
DOTRACE("GrObj::setCategory");
  itsImpl->setCategory(val);
}

void GrObj::setRenderMode(GrObj::RenderMode mode) {
DOTRACE("GrObj::setRenderMode");

#ifdef I686
  // display lists don't work at present with i686/linux/mesa
  if (mode == GLCOMPILE) mode = DIRECT_RENDER;
#endif

  itsImpl->setRenderMode(mode);
  sendStateChangeMsg();
}

void GrObj::setUnRenderMode(GrObj::RenderMode mode) {
DOTRACE("GrObj::setUnRenderMode");

  itsImpl->setUnRenderMode(mode);
  sendStateChangeMsg();
}

void GrObj::receiveStateChangeMsg(const Util::Observable* obj) {
DOTRACE("GrObj::receiveStateChangeMsg");
  DebugEval((void*)this);
  DebugEval((void*)dynamic_cast<GrObj*>(this));
  DebugEval((void*)dynamic_cast<Util::Observer*>(this));
  DebugEval((void*)dynamic_cast<Util::Observable*>(this));
  DebugEvalNL((void*)obj);
  if (obj == this) {
    DebugEval((void*)this); DebugEvalNL((void*)itsImpl);
    itsImpl->invalidateCaches();
  }
}

void GrObj::receiveDestroyMsg(const Util::Observable*) {
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
