///////////////////////////////////////////////////////////////////////
//
// gxcamera.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov 21 15:18:58 2002
// written: Sat Mar 29 13:02:25 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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

/// Camera subclass that generates a perspective projection.
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

/// Camera subclass that keeps a fixed rectangle visible at all times.
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

/// Camera that keeps at least a given rect visible at all times.
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

/// Camera that maintains a fixed world-to-screen scaling.
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

/// Camera that maintains a fixed world-to-screen scaling.
/** The camera is controlled by the observer-to-screen distance and the
    desired scaling of units per degree-of-visual-angle. */
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
