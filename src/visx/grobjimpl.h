///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Thu Jul 19 11:42:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJIMPL_H_DEFINED
#define GROBJIMPL_H_DEFINED

#include "grobj.h"
#include "grobjaligner.h"
#include "grobjbbox.h"
#include "grobjrenderer.h"
#include "grobjscaler.h"
#include "point.h"
#include "rect.h"

///////////////////////////////////////////////////////////////////////
//
// GrObjImpl class
//
///////////////////////////////////////////////////////////////////////

class GrObjImpl {
private:
  GrObjImpl(const GrObjImpl&);
  GrObjImpl& operator=(const GrObjImpl&);

  static const IO::VersionId GROBJ_SERIAL_VERSION_ID = 1;

public:
  //////////////
  // creators //
  //////////////

  GrObjImpl(GrObj* obj);
  virtual ~GrObjImpl();

  IO::VersionId serialVersionId() const { return GROBJ_SERIAL_VERSION_ID; }
  void readFrom(IO::Reader* reader);
  void writeTo(IO::Writer* writer) const;


  ///////////////
  // rendering //
  ///////////////
public:

  void draw(GWT::Canvas& canvas) const;

  /////////////////
  // unrendering //
  /////////////////
private:
  class UnRenderer {
  public:
    UnRenderer() :
      itsMode(GrObj::SWAP_FORE_BACK)
      {}

    GrObj::RenderMode itsMode;
  };

  void undrawDirectRender(GWT::Canvas& canvas) const;
  void undrawSwapForeBack(GWT::Canvas& canvas) const;
  void undrawClearBoundingBox(GWT::Canvas& canvas) const;

public:

  GrObj::RenderMode getUnRenderMode() const
    { return itsUnRenderer.itsMode; }

  void setUnRenderMode(GrObj::RenderMode new_mode)
  {
    itsUnRenderer.itsMode = new_mode;
  }

  void undraw(GWT::Canvas& canvas) const;

  ///////////////
  // ? other ? //
  ///////////////
public:

  int category() const { return itsCategory; }
  void setCategory(int val) { itsCategory = val; }

  void invalidateCaches();

  ///////////////////////////////////////////
  // Forwards to GrObj's protected members //
  ///////////////////////////////////////////

  void grRender(GWT::Canvas& canvas, GrObj::DrawMode mode) const
    { self->grRender(canvas, mode); }

  Rect<double> grGetBoundingBox() const
    { return self->grGetBoundingBox(); }

  //////////////////
  // Data members //
  //////////////////
private:
  GrObj* const self;

public:
  int itsCategory;
  GrObjBBox itsBB;
  GrObjScaler itsScaler;
  GrObjAligner itsAligner;
  GrObjRenderer itsRenderer;
  UnRenderer itsUnRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
