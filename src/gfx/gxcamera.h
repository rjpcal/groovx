///////////////////////////////////////////////////////////////////////
//
// gxcamera.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 21 15:18:58 2002
// written: Thu Nov 21 16:40:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXCAMERA_H_DEFINED
#define GXCAMERA_H_DEFINED

#include "gx/rect.h"

#include "gfx/gxnode.h"

namespace Gfx
{
  class Canvas;
}

// ####################################################################

/// GxCamera sets up an orthographic or perspective projection.

class GxCamera : public GxNode
{
public:
  void reshape(int w, int h) { itsWidth = w; itsHeight = h; }

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  /// Overridden from GxNode.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const {}

private:
  int itsWidth;
  int itsHeight;
};

class GxPerspectiveCamera : public GxCamera
{
public:
  GxPerspectiveCamera() : GxCamera() {}

  virtual void readFrom(IO::Reader* reader) {}
  virtual void writeTo(IO::Writer* writer) const {}

  void setPerspective(double fovy, double zNear, double zFar);

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  double itsFovY;
  double itsNearZ;
  double itsFarZ;
};

class GxFixedRectCamera : public GxCamera
{
public:
  GxFixedRectCamera() :
    GxCamera(),
    itsRect(Gfx::RectLTRB<double>(-1.0, 1.0, 1.0, -1.0))
  {}

  virtual void readFrom(IO::Reader* reader) {}
  virtual void writeTo(IO::Writer* writer) const {}

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

  virtual void readFrom(IO::Reader* reader) {}
  virtual void writeTo(IO::Writer* writer) const {}

  void setRect(const Gfx::Rect<double>& rect) { itsRect = rect; }

  const Gfx::Rect<double>& getRect() const { return itsRect; }

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  Gfx::Rect<double> itsRect;
};

class GxFixedScaleCamera : public GxCamera
{
public:
  // FIXME pixels per inch should come from the Canvas

  GxFixedScaleCamera(double screen_ppi = 72.0, double unit_angle = 2.05) :
    GxCamera(),
    itsScreenPixelsPerInch(screen_ppi),
    itsPixelsPerUnit(1.0),
    itsViewingDistance(30.0)
  {
    setUnitAngle(unit_angle);
  }

  static GxFixedScaleCamera* make() { return new GxFixedScaleCamera; }

  virtual void readFrom(IO::Reader* reader) {}
  virtual void writeTo(IO::Writer* writer) const {}

  void setPixelsPerUnit(double s);
  void setUnitAngle(double deg);
  void setViewingDistIn(double inches);

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  const double itsScreenPixelsPerInch;
  double itsPixelsPerUnit;
  double itsViewingDistance;    // inches
};

static const char vcid_gxcamera_h[] = "$Header$";
#endif // !GXCAMERA_H_DEFINED
