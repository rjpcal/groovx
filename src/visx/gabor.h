///////////////////////////////////////////////////////////////////////
//
// gabor.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 10:45:58 1999
// written: Mon Dec  6 21:34:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABOR_H_DEFINED
#define GABOR_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef PROPERTY_H_DEFINED
#include "property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * Gabor implements a psychophysical Gabor patch. There are several
 * rendering modes, for rendering in grayscale or in black-and-white.
 *
 * @short Subclass of GrObj for drawing psychophysical Gabor patches.
 **/
///////////////////////////////////////////////////////////////////////

class Gabor : public GrObj, public PropFriend<Gabor> {
public:
  ///
  Gabor();
  ///
  virtual ~Gabor();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);

  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  /** @name    Color modes   */
  //@{
  ///
  typedef int ColorMode;
  /** Each point in the patch is rendered with a gray color whose
      luminance is proportional to the value of the Gabor function. */
  static const ColorMode GRAYSCALE = 1;
  /** Each point in the patch is rendered black with a probability
      proportional to the value of the Gabor function. */
  static const ColorMode BW_DITHER_POINT = 2;
  /** The patch is subdivided into rectangles, and each rectangle in
      the patch is rendered black with a probability proportional to
      the value of the Gabor function. */
  static const ColorMode BW_DITHER_RECT = 3;
  //@}

  ///
  typedef PropertyInfo<Gabor> PInfo;
  ///
  static const vector<PInfo>& getPropertyInfos();

  ///
  CTProperty<Gabor, ColorMode> colorMode;

  /// Contrast of the sine grating
  CTProperty<Gabor, double> contrast;

  /// The spatial frequency of the sine grating, in cycles per OpenGL unit
  CTProperty<Gabor, double> spatialFreq;

  /// Phase of the sine grating, in degrees
  CTProperty<Gabor, int> phase;

  /// The base standard deviation for the Gaussian
  CTProperty<Gabor, double> sigma;

  /// The ratio of standard deviations width/height
  CTProperty<Gabor, double> aspectRatio;

  /// Orientation of the sine grating, in degrees
  CTProperty<Gabor, int> orientation;

  /** Number of rectangle subdivisions per OpenGL unit (note that this
		applies only when the colorMode is BW_DITHER_RECT */
  CTProperty<Gabor, int> resolution;

  /** The size of the points used when colorMode is either GRAYSCALE
      or BW_DITHER_POINT */
  CTProperty<Gabor, int> pointSize;

protected:
  ///
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  ///
  virtual void grRender(Canvas& canvas) const;
};

static const char vcid_gabor_h[] = "$Header$";
#endif // !GABOR_H_DEFINED
