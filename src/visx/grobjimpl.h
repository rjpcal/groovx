///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Fri Aug 10 17:36:24 2001
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
#include "grobjrenderer.h"
#include "grobjscaler.h"

#include "glcachenode.h"

class GrObjNode : public Gnode {
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : Gnode(), itsObj(obj) {}

  virtual ~GrObjNode() {}

  virtual void gnodeDraw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas, GrObj::DRAW); }

  virtual void gnodeUndraw(Gfx::Canvas& canvas) const
  { itsObj->grRender(canvas, GrObj::UNDRAW); }

  virtual Rect<double> gnodeBoundingBox(Gfx::Canvas& /*canvas*/) const
  { return itsObj->grGetBoundingBox(); }
};

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl class
//
///////////////////////////////////////////////////////////////////////

class GrObjImpl {
private:
  GrObjImpl(const GrObjImpl&);
  GrObjImpl& operator=(const GrObjImpl&);

public:

  GrObjImpl(GrObj* obj);
  virtual ~GrObjImpl();

  IO::VersionId serialVersionId() const;
  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;


  void draw(Gfx::Canvas& canvas) const;

  void undraw(Gfx::Canvas& canvas) const;

  void invalidateCaches();

  //
  // Data members
  //

  int itsCategory;

  shared_ptr<GrObjNode> itsNativeNode;
  shared_ptr<GrObjBBox> itsBB;
  shared_ptr<GLCacheNode> itsGLCache;

  GrObjScaler itsScaler;
  GrObjAligner itsAligner;
  GrObjRenderer itsRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
