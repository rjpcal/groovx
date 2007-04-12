/** @file gfx/gxshapekit.h full-featured base class for high-level
    graphic objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Dec  1 08:00:00 1998 (as grobj.h)
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

#ifndef GROOVX_GFX_GXSHAPEKIT_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXSHAPEKIT_H_UTC20050626084023_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

#include "rutz/tracer.h"

class GxShapeKitImpl;

namespace rutz
{
  class fstring;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GxShapeKit is the abstract base class for graphic
 * objects. GxShapeKit*'s may be stored and manipulated in
 * ObjList's. Subclasses derived from GxShapeKit must specify the
 * details of how their objects will be drawn, by overriding the
 * virtual function grRender(), and/or by choosing an appropriate
 * render mode with setRenderMode(). Public clients call draw() to
 * display the object. The caches that mediate the various drawing
 * modes are kept up to date by using the Signal interface. Thus,
 * whenever a manipulator changes a parameter in a derived class, it
 * should also call GxShapeKit::sigNodeChanged.emit() to indicate that
 * an update is needed.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxShapeKit : public GxNode,
                   public FieldContainer
{
public:

  /// This tracer dynamically controls the tracing of GxShapeKit member functions.
  static rutz::tracer tracer;


  //////////////
  // creators //
  //////////////

  /// Default constructor
  GxShapeKit();
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~GxShapeKit() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /// Get GxShapeKit's fields.
  static const FieldMap& classFields();

  ///////////////////////////////////////////////////////////
  //
  // Accessors
  //
  ///////////////////////////////////////////////////////////

  /** Queries whether the bounding box will be visible when the object
      is drawn. */
  bool getBBVisibility() const;

  /** Overridden from GxNode. Extends the cube by the rect returned by
      grGetBoundingBox(), with additional transformations to reflect
      the scaling mode, alignment mode, and pixel border
      values. Subclasses of GxShapeKit should override
      grGetBoundingBox(), not getBoundingCube(). */
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /** Subclasses must override this function to return the bounding
      box in GL coordinates for the object's onscreen image. */
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const = 0;

  /// Returns the current scaling mode.
  int getScalingMode() const;

  /// Returns the object's onscreen width in OpenGL units.
  double getWidth() const;

  /// Returns the object's onscreen height in OpenGL units.
  double getHeight() const;

  /** Returns the object's aspect ratio in OpenGL units. This measures
      how much the object is deformed relative to its native size. For
      both width and height, a scaling factor is computed as the ratio
      of the native dimension to the actual onscreen dimension. The
      aspect ratio is then the ratio of the width scaling factor to
      the height scaling factor. */
  double getAspectRatio() const;

  /// Returns the value of the larger of the object's width and height.
  double getMaxDimension() const;

  /// Returns the current alignment mode.
  int getAlignmentMode() const;

  /// Returns the x value of the location of the object's center.
  double getCenterX() const;

  /// Returns the x value of the location of the object's center.
  double getCenterY() const;

  /** Returns the thickness (as a percentage of object size) of the
      border around the bounding box area. */
  int getPercentBorder() const;

  /// Returns the value of the user-defined category.
  virtual int category() const;

  /// Returns the current rendering mode.
  int getRenderMode() const;


  ///////////////////////////////////////////////////////////
  //
  // Manipulators
  //
  ///////////////////////////////////////////////////////////

  /// Changes whether the bounding box will be visible when the object is drawn.
  void setBBVisibility(bool visibility);

  /** Changes the current scaling mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setScalingMode(int mode);

  /** Changes the current width to \a val, unless the scaling mode is
      \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the height will be adjusted accordingly
      as well. */
  void setWidth(double val);

  /** Changes the current height to \a val, unless the scaling mode is
      \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the width will be adjusted accordingly
      as well. */
  void setHeight(double val);

  /** Changes the aspect ratio to \a val, unless the scaling mode is
      \c NATIVE_SCALING or \c MAINTAIN_ASPECT_SCALING. */
  void setAspectRatio(double val);

  /** Sets the larger of width or height to \a val, unless the scaling
      mode is \c NATIVE_SCALING. If the scaling mode is \c
      MAINTAIN_ASPECT_SCALING, the other dimension will be adjusted
      accordingly as well. */
  void setMaxDimension(double val);

  /** Changes the current alignment mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setAlignmentMode(int mode);

  /** Change the x value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterX(double val);

  /** Change the y value for the location of the object's center to \a
      val. This value is used only if the alignment mode is \c
      ARBITRARY_ON_CENTER. */
  void setCenterY(double val);

  /** Change the thickness (as a percentage of object size) of the
      border that will be added around the bounding box area. */
  void setPercentBorder(int pixels);

  /// Change the value of the user-defined category to \a val.
  virtual void setCategory(int val);

  /** Changes the current rendering mode to \a mode, unless the
      requirements of \a mode (for example, requiring a bounding box)
      are not met, in which case the scaling mode is unchanged. */
  void setRenderMode(int mode);


  ///////////////////////////////////////////////////////////
  //
  // Actions
  //
  ///////////////////////////////////////////////////////////

  /** This function draws the object according to the GrRenderMode
      selected with setRenderMode(). */
  void draw(Gfx::Canvas& canvas) const;

  /** This function must be overridden in derived classes to execute
      the actual drawing commands that render the object. */
  virtual void grRender(Gfx::Canvas& canvas) const = 0;

private:
  double getWidthFactor() const;
  void setWidthFactor(double val);

  double getHeightFactor() const;
  void setHeightFactor(double val);

  GxShapeKit(const GxShapeKit&);
  GxShapeKit& operator=(const GxShapeKit&);

  friend class GxShapeKitImpl;
  GxShapeKitImpl* const rep;    // opaque pointer to implementation
};

static const char __attribute__((used)) vcid_groovx_gfx_gxshapekit_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXSHAPEKIT_H_UTC20050626084023_DEFINED
