///////////////////////////////////////////////////////////////////////
//
// gabor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct  6 10:45:58 1999
// written: Wed Sep 25 19:01:57 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_H_DEFINED
#define GABOR_H_DEFINED

#include "gfx/gbcolor.h"

#include "visx/grobj.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Gabor is a subclass of \c GrObj for rendering psychophysical
 * Gabor patches. There are several rendering modes, for rendering in
 * grayscale or in black-and-white.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gabor : public GrObj
{
protected:
  /// Default constructor.
  Gabor();

public:
  /// Default creator.
  static Gabor* make();

  /// Virtual destructor.
  virtual ~Gabor();

  static const FieldMap& classFields();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

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
  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

  virtual void grRender(Gfx::Canvas& canvas) const;
};

static const char vcid_gabor_h[] = "$Header$";
#endif // !GABOR_H_DEFINED
