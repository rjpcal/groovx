///////////////////////////////////////////////////////////////////////
//
// house.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:15 1999
// written: Sun Nov 21 15:54:53 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_H_DEFINED
#define HOUSE_H_DEFINED

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
 * House is a subclass of GrObj that is able to draw simple line-drawn
 * houses that are parameterized in many ways, such as the number of
 * stories in the building, the number of windows per story, the
 * height of the roof, the shape of the roof, etc.
 *
 * @memo A subclass of GrObj for drawing simple line-drawn houses.
 **/
///////////////////////////////////////////////////////////////////////

class House : public GrObj, public PropFriend<House> {
public:
  ///
  House();
  ///
  virtual ~House();

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
  typedef PropertyInfo<House> PInfo;
  ///
  static const vector<PInfo>& getPropertyInfos();

  /// Width/height ratio of each story.
  CTProperty<House, double> storyAspectRatio;

  /// Number of stories in the building.
  CTProperty<House, int> numStories;

  /// Position of the door in range [0, numWindows)
  CTProperty<House, int> doorPosition;

  /// Width of door as fraction of available parcel
  CTProperty<House, double> doorWidth;

  /// Height of door as fraction of one story
  CTProperty<House, double> doorHeight;

  /// Whether door handle is on left or right.
  CTProperty<House, bool> doorOrientation;

  /// Number of windows per story.
  CTProperty<House, int> numWindows;

  /// Width of windows as fraction of available parcel
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
  ///
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;
  ///
  virtual void grRender() const; 

private:
  House(const House&);
  House& operator=(const House&);

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;
};

static const char vcid_house_h[] = "$Header$";
#endif // !HOUSE_H_DEFINED
