/** @file visx/gabor.h graphic object for rendering gabor patches */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Oct  6 10:45:58 1999
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

#ifndef GROOVX_VISX_GABOR_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_GABOR_H_UTC20050626084016_DEFINED

namespace media
{
  class bmap_data;
}

#include "gfx/gbcolor.h"
#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Gabor is a subclass of \c GxShapeKit for rendering psychophysical
 * Gabor patches. There are several rendering modes, for rendering in
 * grayscale or in black-and-white.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gabor : public GxShapeKit
{
protected:
  /// Default constructor.
  Gabor();

public:
  /// Default creator.
  static Gabor* make();

  /// Virtual destructor.
  virtual ~Gabor() noexcept;

  /// Get Gabor's fields.
  static const FieldMap& classFields();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /** The symbolic constants of type \c ColorMode can be used to
      determine exactly how the Gabor function is translated into
      pixel values. */
  typedef int ColorMode;

  /** In this mode, each point in the patch is rendered with a gray
      color whose luminance is proportional to the value of the Gabor
      function. */
  static const ColorMode GRAYSCALE = 1;

  /** In this mode, each point in the patch is rendered with a color
      index proportional to the value of the Gabor function. */
  static const ColorMode COLOR_INDEX = 2;

  /** In this mode, each point in the patch is rendered black with a
      probability proportional to the value of the Gabor function. */
  static const ColorMode BW_DITHER_POINT = 3;

  /** In this mode, the patch is subdivided into rectangles, and each
      rectangle in the patch is rendered black with a probability
      proportional to the value of the Gabor function. */
  static const ColorMode BW_DITHER_RECT = 4;

  /** Sets the contrast to \c pow(10,\a logContrast). \a logContrast
      must be negative, so that contrast falls in [0,1]. */
  void setLogContrast(double logContrast);

  /// Returns \c log10(contrast).
  double getLogContrast() const;

  /// Fill in the media::bmap_data object with the Gabor pixmap.
  void getBmapData(media::bmap_data& bmap) const;

  /// Save the Gabor pixmap as an image file at the given filename.
  void saveImage(const char* filename) const;

private:
  /// The current \c ColorMode used for rendering.
  ColorMode itsColorMode;

  /// Contrast of the sine grating.
  double itsContrast;

  /// The spatial frequency of the sine grating, in cycles per OpenGL unit.
  double itsSpatialFreq;

  /// Phase of the sine grating, in degrees.
  unsigned int itsPhase;

  /** Number of degrees by which itsPhase will drift each time the
      patch is drawn. */
  unsigned int itsDrift;

  /** The actual phase with which the patch is drawn; this may differ
      from itsPhase if the grating is drifting. */
  mutable unsigned int itsDrawnPhase;

  /// The base standard deviation for the Gaussian.
  double itsSigma;

  /// The ratio of standard deviations width/height.
  double itsAspectRatio;

  /// Orientation of the sine grating, in degrees.
  int itsOrientation;

  /** Number of rectangle subdivisions per OpenGL unit (note that this
      applies only when the colorMode is \a BW_DITHER_RECT). */
  int itsResolution;

  /** The size of the points used when \a colorMode is either \a
      GRAYSCALE or \a BW_DITHER_POINT. */
  int itsPointSize;

  /// The tint which should be applied to the foreground color.
  GbColor itsFgTint;

  /// The tint which should be applied to the background color.
  GbColor itsBgTint;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;
};

#endif // !GROOVX_VISX_GABOR_H_UTC20050626084016_DEFINED
