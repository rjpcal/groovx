///////////////////////////////////////////////////////////////////////
//
// gabor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct  6 10:45:58 1999
// written: Fri Jun  1 14:21:07 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_H_DEFINED
#define GABOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Gabor is a subclass of \c GrObj for rendering psychophysical
 * Gabor patches. There are several rendering modes, for rendering in
 * grayscale or in black-and-white.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gabor : public GrObj, public FieldContainer {
protected:
  /// Default constructor.
  Gabor();

public:
  /// Default creator.
  static Gabor* make();

  /// Virtual destructor.
  virtual ~Gabor();

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

  /// The current \c ColorMode used for rendering.
  TField<ColorMode> colorMode;

  /// Contrast of the sine grating.
  TField<double> contrast;

  /// The spatial frequency of the sine grating, in cycles per OpenGL unit.
  TField<double> spatialFreq;

  /// Phase of the sine grating, in degrees.
  TField<int> phase;

  /// The base standard deviation for the Gaussian.
  TField<double> sigma;

  /// The ratio of standard deviations width/height.
  TField<double> aspectRatio;

  /// Orientation of the sine grating, in degrees.
  TField<int> orientation;

  /** Number of rectangle subdivisions per OpenGL unit (note that this
		applies only when the colorMode is \a BW_DITHER_RECT). */
  TField<int> resolution;

  /** The size of the points used when \a colorMode is either \a
      GRAYSCALE or \a BW_DITHER_POINT. */
  TField<int> pointSize;

  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas, DrawMode mode) const;
};

static const char vcid_gabor_h[] = "$Header$";
#endif // !GABOR_H_DEFINED
