///////////////////////////////////////////////////////////////////////
//
// maskhatch.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 15:49:58 1999
// written: Sat Nov 11 10:30:08 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_H_DEFINED
#define MASKHATCH_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
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
protected:
  /// Default constructor.
  MaskHatch();

public:
  /// Default creator.
  static MaskHatch* make();

  /// Virtual destructor.
  virtual ~MaskHatch();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////////
  // properties //
  ////////////////
 
  /// Info about a \c MaskHatch property.
  typedef PropertyInfo<MaskHatch> PInfo;

  /// Return the number of \c MaskHatch properties.
  static unsigned int numPropertyInfos();

  /// Return info on the i'th \c MaskHatch property.
  static const PInfo& getPropertyInfo(unsigned int i);

  /// The number of lines that will be rendered in each direction.
  TBoundedProperty<int, 0, 1000, 1> numLines;

  /// The pixel-width of each line.
  TBoundedProperty<int, 0, 1000, 1> lineWidth;

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas) const; 
};

static const char vcid_maskhatch_h[] = "$Header$";
#endif // !MASKHATCH_H_DEFINED
