///////////////////////////////////////////////////////////////////////
//
// maskhatch.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 15:49:58 1999
// written: Fri Feb 18 09:11:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_H_DEFINED
#define MASKHATCH_H_DEFINED

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
 * \c MaskHatch is a subclass of \c GrObj for rendering a visual mask
 * consisting of hatched lines. The pattern will contain vertical,
 * horizontal, and diagonal lines at 45-degrees left, and 45-degrees
 * right.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MaskHatch : public GrObj, public PropFriend<MaskHatch> {
public:
  /// Default constructor.
  MaskHatch();

  /// Virtual destructor.
  virtual ~MaskHatch();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////
 
  /// Info about a \c MaskHatch property.
  typedef PropertyInfo<MaskHatch> PInfo;

  /// Return a collection of info about all \c MaskHatch properties.
  static const vector<PInfo>& getPropertyInfos();

  /// The number of lines that will be rendered in each direction.
  CTBoundedProperty<MaskHatch, int, 0, 1000, 1> numLines;

  /// The pixel-width of each line.
  CTBoundedProperty<MaskHatch, int, 0, 1000, 1> lineWidth;

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(Canvas& canvas) const; 
};

static const char vcid_maskhatch_h[] = "$Header$";
#endif // !MASKHATCH_H_DEFINED
