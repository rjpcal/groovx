///////////////////////////////////////////////////////////////////////
//
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Mon Dec  6 21:34:14 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_H_DEFINED
#define FIXPT_H_DEFINED

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
 * FixPt implements a simple psychophysics fixation cross, with
 * attributes to control the length of the bars, and the pixel-width
 * of the bars.
 *
 * @short Subclass of GrObj for drawing psychophysics fixation crosses.
 **/
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj, public PropFriend<FixPt> {
public:
  /// Default constructor.
  FixPt (double len=0.1, int wid=1);
  /// Stream constructor.
  FixPt (istream &is, IOFlag flag);
  ///
  virtual ~FixPt ();

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

  ////////////////
  // properties //
  ////////////////

  ///
  typedef PropertyInfo<FixPt> PInfo;
  ///
  static const vector<PInfo>& getPropertyInfos();

  /// Length of crosshairs in GL coordinates
  CTProperty<FixPt, double> length;
  /// width of crosshairs in pixels
  CTProperty<FixPt, int> width;

protected:
  ///
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  ///
  virtual void grRender(Canvas& canvas) const;
};

static const char vcid_fixpt_h[] = "$Header$";
#endif // !FIXPT_H_DEFINED
