///////////////////////////////////////////////////////////////////////
//
// grobjimpl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 16:27:54 2000
// written: Thu Jul 19 13:13:08 2001
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


  void draw(GWT::Canvas& canvas) const;

  void undraw(GWT::Canvas& canvas) const;

  void invalidateCaches();

  void undrawDirectRender(GWT::Canvas& canvas) const;
  void undrawClearBoundingBox(GWT::Canvas& canvas) const;

  //
  // Forwards to GrObj's protected members
  //

  void grRender(GWT::Canvas& canvas, GrObj::DrawMode mode) const
    { self->grRender(canvas, mode); }

  Rect<double> grGetBoundingBox() const
    { return self->grGetBoundingBox(); }

  //
  // Data members
  //

  GrObj* const self;

  int itsCategory;
  GrObjBBox itsBB;
  GrObjScaler itsScaler;
  GrObjAligner itsAligner;
  GrObjRenderer itsRenderer;
};

static const char vcid_grobjimpl_h[] = "$Header$";
#endif // !GROBJIMPL_H_DEFINED
