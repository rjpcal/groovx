///////////////////////////////////////////////////////////////////////
//
// house.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 13 12:43:15 1999
// written: Thu Jul 19 09:41:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_H_DEFINED
#define HOUSE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
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

class House : public GrObj, public FieldContainer {
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

  /// Width/height ratio of each story.
  TField<double> storyAspectRatio;

  /// Number of stories in the building.
  TField<int> numStories;

  /// Position of the door in range [0, numWindows).
  TField<int> doorPosition;

  /// Width of door as fraction of available parcel.
  TField<double> doorWidth;

  /// Height of door as fraction of one story.
  TField<double> doorHeight;

  /// Whether door handle is on left or right.
  TField<bool> doorOrientation;

  /// Number of windows per story.
  TField<int> numWindows;

  /// Width of windows as fraction of available parcel.
  TField<double> windowWidth;

  /// Height of windows as fraction of one story.
  TField<double> windowHeight;

  /// Number of vertical bars in each window.
  TField<int> windowVertBars;

  /// Number of horizontal bars in each window.
  TField<int> windowHorizBars;

  /// Enumerated mode for the roof shape.
  TField<int> roofShape;

  /// Amount of roof overhang, as a fraction of story width.
  TField<double> roofOverhang;

  /// Height of roof as fraction of the story height.
  TField<double> roofHeight;

  /// Color of the roof: 0 -> black, 1 -> white.
  TField<int> roofColor;

  /// X position of the chimney within the roof.
  TField<double> chimneyXPosition;

  /// Y position of the chimney within the roof.
  TField<double> chimneyYPosition;

  /// Width of the chimney, as fraction of the story width.
  TField<double> chimneyWidth;

  /// Height of the chimney, as fraction of the story height.
  TField<double> chimneyHeight;

  static const FieldMap& classFields();

protected:
  virtual Rect<double> grGetBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas, DrawMode mode) const;

private:
  House(const House&);
  House& operator=(const House&);
};

static const char vcid_house_h[] = "$Header$";
#endif // !HOUSE_H_DEFINED
