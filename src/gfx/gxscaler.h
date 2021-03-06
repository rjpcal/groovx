/** @file gfx/gxscaler.h GxBin subclass that rescales its child node */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 13 13:04:27 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_GXSCALER_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXSCALER_H_UTC20050626084024_DEFINED

#include "gfx/gxbin.h"

/// A graphics node that optionally rescales its child object.
class GxScaler : public GxBin
{
public:
  ///////////////////////////////////////////////////////////
  //
  // Scaling modes
  //
  ///////////////////////////////////////////////////////////

  /** The symbolic constants of type \c ScalingMode provide several
      ways to scale an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c getMode() and \c
      setMode(). The default scaling mode is \c NATIVE_SCALING. */
  typedef int Mode;

  /** This is the default scaling mode. No additional scaling is done
      beyond whatever happens in the subclass's \c grRender()
      implementation. */
  static const Mode NATIVE_SCALING           = 1;

  /** In this mode, the native aspect ratio of the object will be
      maintained. Thus, requests to change one of the object's
      dimensions (width or height) will affect the other dimension so
      as to maintain the native aspect ratio. In this mode, \c
      getAspectRatio() should always return 1.0. */
  static const Mode MAINTAIN_ASPECT_SCALING  = 2;

  /** In this mode, the width and height of the object may be set
      independently of each other. */
  static const Mode FREE_SCALING             = 3;


  /// Default constructor.
  GxScaler();

  /// Construct with a given child object.
  GxScaler(nub::ref<GxNode> child);

  /// Factory function.
  static GxScaler* make();

  /// Get the current scaling mode.
  Mode getMode() const { return itsMode; }

  /// Change the current scaling mode.
  void setMode(Mode new_mode);

  /// Set the desired scaled width of the child object.
  void setWidth(double new_width);
  /// Set the desired scaled height of the child object.
  void setHeight(double new_height);
  /// Set the desired maximum scaled dimension of the child object.
  void setMaxDim(double new_max_dimension);

  /// Set the desired scaled aspect ratio of the child object.
  void setAspectRatio(double new_aspect_ratio);

  /// Set the desired width scaling factor.
  void setWidthFactor(double f);
  /// Set the desired height scaling factor.
  void setHeightFactor(double f);

  /// Get the scaled width of the child object.
  double scaledWidth() const;
  /// Get the scaled height of the child object.
  double scaledHeight() const;
  /// Get the maximum scaled dimension of the child object.
  double scaledMaxDim() const;

  /// Get the scaled aspect ratio of the child object.
  double aspectRatio() const;

  /// Get the width scaling factor.
  double widthFactor() const { return itsWidthFactor; }
  /// Get the height scaling factor.
  double heightFactor() const { return itsHeightFactor; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  virtual void draw(Gfx::Canvas& canvas) const override;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

private:
  Mode itsMode;

  double itsWidthFactor;
  double itsHeightFactor;
  double itsAspectRatio;
};


#endif // !GROOVX_GFX_GXSCALER_H_UTC20050626084024_DEFINED
