///////////////////////////////////////////////////////////////////////
//
// gxcamera.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 21 15:18:58 2002
// written: Thu Nov 21 18:41:49 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_H_DEFINED
#define GXCAMERA_H_DEFINED

#include "gx/rect.h"

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

namespace Gfx
{
  class Canvas;
}

// ####################################################################

/// GxCamera sets up an orthographic or perspective projection.

class GxCamera : public GxNode
{
public:
  GxCamera() : GxNode(), itsWidth(0), itsHeight(0) {}

  void reshape(int w, int h) { itsWidth = w; itsHeight = h; }

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  /// Overridden from GxNode.
  virtual void getBoundingCube(Gfx::Bbox& /*bbox*/) const {}

private:
  int itsWidth;
  int itsHeight;
};

class GxPerspectiveCamera : public GxCamera, public FieldContainer
{
public:
  GxPerspectiveCamera();

  static GxPerspectiveCamera* make() { return new GxPerspectiveCamera; }

  static const FieldMap& classFields();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  double itsFovY;
  double itsNearZ;
  double itsFarZ;
  GbVec3<double> translation;
};

class GxFixedRectCamera : public GxCamera
{
public:
  GxFixedRectCamera() :
    GxCamera(),
    itsRect(Gfx::RectLTRB<double>(-1.0, 1.0, 1.0, -1.0))
  {}

  virtual void readFrom(IO::Reader* /*reader*/) {}
  virtual void writeTo(IO::Writer* /*writer*/) const {}

  void setRect(const Gfx::Rect<double>& rect) { itsRect = rect; }

  const Gfx::Rect<double>& getRect() const { return itsRect; }

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  Gfx::Rect<double> itsRect;
};

class GxMinRectCamera : public GxCamera
{
public:
  GxMinRectCamera() :
    GxCamera(),
    itsRect(Gfx::RectLTRB<double>(-1.0, 1.0, 1.0, -1.0))
  {}

  virtual void readFrom(IO::Reader* /*reader*/) {}
  virtual void writeTo(IO::Writer* /*writer*/) const {}

  void setRect(const Gfx::Rect<double>& rect) { itsRect = rect; }

  const Gfx::Rect<double>& getRect() const { return itsRect; }

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  Gfx::Rect<double> itsRect;
};

class GxFixedScaleCamera : public GxCamera, public FieldContainer
{
public:
  GxFixedScaleCamera();

  static GxFixedScaleCamera* make() { return new GxFixedScaleCamera; }

  static const FieldMap& classFields();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  double getLogPixelsPerUnit() const;
  void setLogPixelsPerUnit(double s);

  double getPixelsPerUnit() const { return itsPixelsPerUnit; }
  void setPixelsPerUnit(double s);

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  double itsPixelsPerUnit;
};

class GxPsyphyCamera : public GxCamera, public FieldContainer
{
public:
  GxPsyphyCamera();

  static GxPsyphyCamera* make() { return new GxPsyphyCamera; }

  static const FieldMap& classFields();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  double getUnitAngle() const { return itsDegreesPerUnit; }
  double getViewingDistIn() const { return itsViewingDistance; }

  void setUnitAngle(double deg_per_unit);
  void setViewingDistIn(double inches);

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  double itsDegreesPerUnit;
  double itsViewingDistance;    // inches
};

static const char vcid_gxcamera_h[] = "$Header$";
#endif // !GXCAMERA_H_DEFINED
