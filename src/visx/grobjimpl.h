///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Wed Nov 13 13:08:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "gfx/gxaligner.h"
#include "gfx/gxbin.h"
#include "gfx/gxscaler.h"

#include "gx/box.h"

#include "visx/grobj.h"
#include "visx/grobjbbox.h"

#include "visx/bitmapcachenode.h"
#include "visx/glcachenode.h"

#include "util/volatileobject.h"

class GrObjNode : public GxBin
{
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : GxBin(), itsObj(obj) {}

  virtual ~GrObjNode() {}

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas); }

  virtual void getBoundingCube(Gfx::Box<double>& cube,
                               Gfx::Canvas& canvas) const
  { return cube.unionize(itsObj->grGetBoundingBox(canvas)); }
};

//  ###################################################################
//  ===================================================================

/// Implementation class for GrObj.

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
  Util::Ref<GxAligner> itsAligner;
  Util::Ref<GxScaler> itsScaler;

  Util::Ref<GxNode> itsTopNode;

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
    itsAligner(new GxAligner(itsBitmapCache), Util::PRIVATE),
    itsScaler(new GxScaler(itsAligner), Util::PRIVATE),
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
