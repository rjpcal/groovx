///////////////////////////////////////////////////////////////////////
//
// grobj.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Dec-98
// written: Fri Aug 10 10:46:50 2001
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
  itsImpl(new GrObjImpl(this))
{
DOTRACE("GrObj::GrObj");

  DebugEval((void*)this); DebugEvalNL((void*)itsImpl);

  itsImpl->itsRenderer.setMode(render_mode);
  itsImpl->itsRenderer.setUnMode(unrender_mode);

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
DOTRACE("GrObj::serialVersionId");
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
  return itsImpl->itsBB.isVisible();
}

void GrObj::getBoundingBox(Gfx::Canvas& canvas, Rect<double>& bbox) const
{
DOTRACE("GrObj::getBoundingBox");

  bbox = itsImpl->itsBB.withBorder(grGetBoundingBox(), canvas);
}

GrObj::ScalingMode GrObj::getScalingMode() const
{
DOTRACE("GrObj::getScalingMode");
  return itsImpl->itsScaler.getMode();
}

double GrObj::getWidth() const {
DOTRACE("GrObj::getWidth");
  return itsImpl->itsScaler.scaledWidth(grGetBoundingBox());
}

double GrObj::getHeight() const {
DOTRACE("GrObj::getHeight");
  return itsImpl->itsScaler.scaledHeight(grGetBoundingBox());
}

double GrObj::getAspectRatio() const {
DOTRACE("GrObj::getAspectRatio");
  return itsImpl->itsScaler.aspectRatio();
}

double GrObj::getMaxDimension() const {
DOTRACE("GrObj::getMaxDimension");
  return itsImpl->itsScaler.scaledMaxDim(grGetBoundingBox());
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

int GrObj::getPixelBorder() const {
DOTRACE("GrObj::getPixelBorder");
  return itsImpl->itsBB.pixelBorder();
}

int GrObj::category() const {
DOTRACE("GrObj::category");
  return itsImpl->itsCategory;
}

GrObj::RenderMode GrObj::getRenderMode() const {
  return itsImpl->itsRenderer.getMode();
}

GrObj::RenderMode GrObj::getUnRenderMode() const {
DOTRACE("GrObj::getUnRenderMode");
  return itsImpl->itsRenderer.getUnMode();
}

//////////////////
// manipulators //
//////////////////

void GrObj::setBitmapCacheDir(const char* dirname) {
DOTRACE("GrObj::setBitmapCacheDir");
  GrObjRenderer::BITMAP_CACHE_DIR = dirname;
}

void GrObj::setBBVisibility(bool visibility)
{
  itsImpl->itsBB.setVisible(visibility);
}

void GrObj::setScalingMode(ScalingMode val)
{
DOTRACE("GrObj::setScalingMode");

  itsImpl->itsScaler.setMode(val);
  sendStateChangeMsg();
}

void GrObj::setWidth(double val) {
DOTRACE("GrObj::setWidth");

  itsImpl->itsScaler.setWidth(val, grGetBoundingBox());
  sendStateChangeMsg();
}

void GrObj::setHeight(double val) {
DOTRACE("GrObj::setHeight");

  itsImpl->itsScaler.setHeight(val, grGetBoundingBox());
  sendStateChangeMsg();
}

void GrObj::setAspectRatio(double val) {
DOTRACE("GrObj::setAspectRatio");

  itsImpl->itsScaler.setAspectRatio(val);
  sendStateChangeMsg();
}

void GrObj::setMaxDimension(double val) {
DOTRACE("GrObj::setMaxDimension");

  itsImpl->itsScaler.setMaxDim(val, grGetBoundingBox());
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

void GrObj::setPixelBorder(int pixels) {
DOTRACE("GrObj::setPixelBorder");
  itsImpl->itsBB.setPixelBorder(pixels);
}

void GrObj::setCategory(int val) {
DOTRACE("GrObj::setCategory");
  itsImpl->itsCategory = val;
}

void GrObj::setRenderMode(GrObj::RenderMode mode) {
DOTRACE("GrObj::setRenderMode");

  itsImpl->itsRenderer.setMode(mode);
  sendStateChangeMsg();
}

void GrObj::setUnRenderMode(GrObj::RenderMode mode) {
DOTRACE("GrObj::setUnRenderMode");

  itsImpl->itsRenderer.setUnMode(mode);
  sendStateChangeMsg();
}

void GrObj::receiveStateChangeMsg(const Util::Observable* obj) {
DOTRACE("GrObj::receiveStateChangeMsg");
  DebugEval((void*)this);
  DebugEval((void*)dynamic_cast<GrObj*>(this));
  DebugEval((void*)dynamic_cast<Util::Observer*>(this));
  DebugEval((void*)dynamic_cast<Util::Observable*>(this));
  DebugEvalNL((void*)obj);
  if (obj == this)
    {
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

void GrObj::saveBitmapCache(Gfx::Canvas& canvas, const char* filename) const {
  itsImpl->itsRenderer.saveBitmapCache(itsImpl, canvas, filename);
}

void GrObj::restoreBitmapCache() const {
  itsImpl->itsRenderer.restoreBitmapCache();
}

void GrObj::update(Gfx::Canvas& canvas) const {
  itsImpl->itsRenderer.update(itsImpl, canvas);
}

void GrObj::draw(Gfx::Canvas& canvas) const {
  itsImpl->draw(canvas);
}

void GrObj::undraw(Gfx::Canvas& canvas) const {
  itsImpl->undraw(canvas);
}

static const char vcid_grobj_cc[] = "$Header$";
#endif // !GROBJ_CC_DEFINED
