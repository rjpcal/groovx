///////////////////////////////////////////////////////////////////////
//
// house.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 13 12:43:15 1999
// written: Fri Nov 10 17:03:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_H_DEFINED
#define HOUSE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PROPERTY_H_DEFINED)
#include "io/property.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c House is a subclass of \c GrObj that is able to draw simple
 * line-drawn houses. The appearance of the houses is controlled by
 * many parameters, such as the number of stories in the building, the
 * number of windows per story, the height of the roof, the shape of
 * the roof, etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class House : public GrObj, public PropFriend<House> {
protected:
  /// Default constructor.
  House();

public:
  /// Default creator.
  static House* make();

  /// Virtual destructor.
  virtual ~House();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  /// Info about a \c House property.
  typedef PropertyInfo<House> PInfo;

  /// Return the number of \c House properties.
  static unsigned int numPropertyInfos();

  /// Return info on the i'th \c House property.
  static const PInfo& getPropertyInfo(unsigned int i);

  /// Width/height ratio of each story.
  CTProperty<House, double> storyAspectRatio;

  /// Number of stories in the building.
  CTProperty<House, int> numStories;

  /// Position of the door in range [0, numWindows).
  CTProperty<House, int> doorPosition;

  /// Width of door as fraction of available parcel.
  CTProperty<House, double> doorWidth;

  /// Height of door as fraction of one story.
  CTProperty<House, double> doorHeight;

  /// Whether door handle is on left or right.
  CTProperty<House, bool> doorOrientation;

  /// Number of windows per story.
  CTProperty<House, int> numWindows;

  /// Width of windows as fraction of available parcel.
  CTProperty<House, double> windowWidth;

  /// Height of windows as fraction of one story.
  CTProperty<House, double> windowHeight;

  /// Number of vertical bars in each window.
  CTProperty<House, int> windowVertBars;

  /// Number of horizontal bars in each window.
  CTProperty<House, int> windowHorizBars;

  /// Enumerated mode for the roof shape.
  CTProperty<House, int> roofShape;

  /// Amount of roof overhang, as a fraction of story width.
  CTProperty<House, double> roofOverhang;

  /// Height of roof as fraction of the story height.
  CTProperty<House, double> roofHeight;

  /// Color of the roof: 0 -> black, 1 -> white.
  CTProperty<House, int> roofColor;

  /// X position of the chimney within the roof.
  CTProperty<House, double> chimneyXPosition;

  /// Y position of the chimney within the roof.
  CTProperty<House, double> chimneyYPosition;

  /// Width of the chimney, as fraction of the story width.
  CTProperty<House, double> chimneyWidth;

  /// Height of the chimney, as fraction of the story height.
  CTProperty<House, double> chimneyHeight;

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas) const; 

private:
  House(const House&);
  House& operator=(const House&);
};

static const char vcid_house_h[] = "$Header$";
#endif // !HOUSE_H_DEFINED
