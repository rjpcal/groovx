/** @file gfx/gxcamera.h view a graphic scene with orthographic or
    perspective projection */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 21 15:18:58 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_GFX_GXCAMERA_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXCAMERA_H_UTC20050626084024_DEFINED

#include "geom/rect.h"

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "field/fields.h"

namespace Gfx
{
  class Canvas;
}

// ########################################################

/// GxCamera sets up an orthographic or perspective projection.

class GxCamera : public GxNode
{
public:
  GxCamera() : GxNode(), itsWidth(0), itsHeight(0) {}

  /// Virtual no-throw destructor.
  virtual ~GxCamera() throw();

  /// Notifies the GxCamera that its window has changed size.
  void reshape(Gfx::Canvas& canvas, int w, int h);

  /// Get the width of the camera's window.
  int width() const { return itsWidth; }
  /// Get the height of the camera's window.
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
  /// Default constructor.
  GxPerspectiveCamera();

  /// Virtual no-throw destructor.
  virtual ~GxPerspectiveCamera() throw();

  /// Factory function.
  static GxPerspectiveCamera* make() { return new GxPerspectiveCamera; }

  /// Get GxPerspectiveCamera's fields.
  static const FieldMap& classFields();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

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
  /// Default constructor.
  GxFixedRectCamera() :
    GxCamera(),
    itsRect(geom::rect<double>::ltrb(-1.0, 1.0, 1.0, -1.0))
  {}

  /// Virtual no-throw destructor.
  virtual ~GxFixedRectCamera() throw();

  virtual void read_from(io::reader& /*reader*/) {}
  virtual void write_to(io::writer& /*writer*/) const {}

  /// Change the viewable rect area.
  void setRect(const geom::rect<double>& rect) { itsRect = rect; }

  /// Get the viewable rect area.
  const geom::rect<double>& getRect() const { return itsRect; }

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  geom::rect<double> itsRect;
};

/// Camera that keeps at least a given rect visible at all times.
class GxMinRectCamera : public GxCamera
{
public:
  /// Default constructor.
  GxMinRectCamera() :
    GxCamera(),
    itsRect(geom::rect<double>::ltrb(-1.0, 1.0, 1.0, -1.0))
  {}

  /// Virtual no-throw destructor.
  virtual ~GxMinRectCamera() throw();

  virtual void read_from(io::reader& /*reader*/) {}
  virtual void write_to(io::writer& /*writer*/) const {}

  /// Change the minimally-visible rect
  void setRect(const geom::rect<double>& rect) { itsRect = rect; }

  /// Get the minimally-visible rect
  const geom::rect<double>& getRect() const { return itsRect; }

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  geom::rect<double> itsRect;
};

/// Camera that maintains a fixed world-to-screen scaling.
class GxFixedScaleCamera : public GxCamera, public FieldContainer
{
public:
  /// Default constructor.
  GxFixedScaleCamera();

  /// Virtual no-throw destructor.
  virtual ~GxFixedScaleCamera() throw();

  /// Factory function.
  static GxFixedScaleCamera* make() { return new GxFixedScaleCamera; }

  /// Get GxFixedScaleCamera's fields.
  static const FieldMap& classFields();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /// Get the log-base-10 of the pixels-per-unit factor.
  double getLogPixelsPerUnit() const;
  /// Set the log-base-10 of the pixels-per-unit factor.
  void setLogPixelsPerUnit(double s);

  /// Get the pixels-per-unit factor.
  double getPixelsPerUnit() const { return itsPixelsPerUnit; }
  /// Set the pixels-per-unit factor.
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
  /// Default constructor.
  GxPsyphyCamera();

  /// Virtual no-throw destructor.
  virtual ~GxPsyphyCamera() throw();

  /// Factory function.
  static GxPsyphyCamera* make() { return new GxPsyphyCamera; }

  /// Get GxPsyphyCamera's fields.
  static const FieldMap& classFields();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /// Get the degrees of visual angle corresponding to one OpenGL unit.
  double getUnitAngle() const { return itsDegreesPerUnit; }
  /// Set the degrees of visual angle corresponding to one OpenGL unit.
  void setUnitAngle(double deg_per_unit);

  /// Get the observer's viewing distance in inches.
  double getViewingDistIn() const { return itsViewingDistance; }
  /// Set the observer's viewing distance in inches.
  void setViewingDistIn(double inches);

  virtual void draw(Gfx::Canvas& canvas) const;

private:
  double itsDegreesPerUnit;
  double itsViewingDistance;    // inches
};

static const char vcid_groovx_gfx_gxcamera_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXCAMERA_H_UTC20050626084024_DEFINED
