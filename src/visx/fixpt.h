///////////////////////////////////////////////////////////////////////
//
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Fri Oct 20 17:19:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c FixPt is a subclass of \c GrObj for drawing a simple
 * psychophysics fixation cross. \c FixPt has attributes to control
 * the length of the bars, and the pixel-width of the bars.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj, public PropFriend<FixPt> {
protected:
  /// Construct with initial values for the cross length and pixel-width.
  FixPt (double len=0.1, int wid=1);

public:
  /// Default creator.
  static FixPt* make();

  /// Virtual destructor.
  virtual ~FixPt ();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  /// Info about a \c FixPt property.
  typedef PropertyInfo<FixPt> PInfo;

  /// Return the number of \c FixPt properties.
  static unsigned int numPropertyInfos();

  /// Return info on the i'th \c FixPt property.
  static const PInfo& getPropertyInfo(unsigned int i);

  /// Length of crosshairs in GL coordinates.
  CTProperty<FixPt, double> length;

  /// Width of crosshairs in pixels.
  CTProperty<FixPt, int> width;

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas) const;
};

static const char vcid_fixpt_h[] = "$Header$";
#endif // !FIXPT_H_DEFINED
