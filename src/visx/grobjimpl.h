///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Wed Sep  5 16:11:07 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "application.h"

#include "gnode.h"

#include "grobj.h"
#include "grobjaligner.h"
#include "grobjbbox.h"
#include "grobjscaler.h"

#include "bitmapcachenode.h"
#include "glcachenode.h"

#include "util/volatileobject.h"

class GrObjNode : public Gnode
{
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : Gnode(), itsObj(obj) {}

  virtual ~GrObjNode() {}

  virtual void gnodeDraw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas); }

  virtual Gfx::Rect<double> gnodeBoundingBox(Gfx::Canvas& /*canvas*/) const
  { return itsObj->grGetBoundingBox(); }
};

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl class
//
///////////////////////////////////////////////////////////////////////

class GrObjImpl : public Util::VolatileObject
{
private:
  GrObjImpl(const GrObjImpl&);
  GrObjImpl& operator=(const GrObjImpl&);

public:

  //
  // Data members
  //

  int itsCategory;

  Util::Ref<GrObjNode> itsNativeNode;
  Util::Ref<GrObjBBox> itsBB;
  Util::Ref<GLCacheNode> itsGLCache;
  Util::Ref<BitmapCacheNode> itsBitmapCache;
  Util::Ref<GrObjAligner> itsAligner;
  Util::Ref<GrObjScaler> itsScaler;

  Util::Ref<Gnode> itsTopNode;

  //
  // Methods
  //

  static GrObjImpl* make(GrObj* obj) { return new GrObjImpl(obj); }

  GrObjImpl(GrObj* obj) :
    itsCategory(-1),
    itsNativeNode(new GrObjNode(obj), Util::PRIVATE),
    itsBB(new GrObjBBox(itsNativeNode), Util::PRIVATE),
    itsGLCache(new GLCacheNode(itsBB), Util::PRIVATE),
    itsBitmapCache(new BitmapCacheNode(itsGLCache), Util::PRIVATE),
    itsAligner(new GrObjAligner(itsBitmapCache), Util::PRIVATE),
    itsScaler(new GrObjScaler(itsAligner), Util::PRIVATE),
    itsTopNode(itsScaler)
  {
    // We connect to sigNodeChanged in order to update any caches
    // according to state changes.
    obj->sigNodeChanged.connect(this, &GrObjImpl::invalidateCaches);
  }

  void invalidateCaches()
  {
    itsGLCache->invalidate();
    itsBitmapCache->invalidate();
  }
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
