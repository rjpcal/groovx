///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Fri Aug 10 13:59:44 2001
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

class GrObjNode : public Gnode {
  borrowed_ptr<GrObj> itsObj;

public:
  GrObjNode(GrObj* obj) : itsObj(obj) {}

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

  void undrawDirectRender(Gfx::Canvas& canvas) const;
  void undrawClearBoundingBox(Gfx::Canvas& canvas) const;

  //
  // Forwards to GrObj's protected members
  //

//    void grRender(Gfx::Canvas& canvas, GrObj::DrawMode mode) const
//      {
//        if (mode == GrObj::DRAW)
//          itsObjNode->gnodeDraw(canvas);
//        else
//          itsObjNode->gnodeUndraw(canvas);
//      }

//    Rect<double> grGetBoundingBox() const
//      {
//        Gfx::Canvas& canvas = Application::theApp().getCanvas();
//        return itsObjNode->gnodeBoundingBox(canvas);
//      }

  //
  // Data members
  //

  shared_ptr<GrObjNode> itsObjNode;

  int itsCategory;
  GrObjBBox itsBB;
  GrObjScaler itsScaler;
  GrObjAligner itsAligner;
  GrObjRenderer itsRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
