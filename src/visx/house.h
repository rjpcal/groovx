///////////////////////////////////////////////////////////////////////
//
// house.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:15 1999
// written: Thu Sep 30 11:22:17 1999
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
//
// House class definition
//
///////////////////////////////////////////////////////////////////////

class House : public GrObj, public PropFriend<House> {
public:
  House();
  virtual ~House();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  
  virtual int charCount() const;

  virtual int getCategory() const { return -1; }

  virtual void setCategory(int) {  }

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<House> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<House, double> storyAspectRatio;
  CTProperty<House, int> numStories;

  CTProperty<House, int> doorPosition;
  CTProperty<House, double> doorWidth;  // fraction of avail. space
  CTProperty<House, double> doorHeight; // fraction of one story
  CTProperty<House, bool> doorOrientation; // left or right

  CTProperty<House, int> numWindows;
  CTProperty<House, double> windowWidth;	// fraction of avail. space
  CTProperty<House, double> windowHeight; // fraction of one story
  CTProperty<House, int> windowVertBars;
  CTProperty<House, int> windowHorizBars;

  CTProperty<House, int> roofShape;
  CTProperty<House, double> roofOverhang;
  CTProperty<House, double> roofHeight;
  CTProperty<House, int> roofColor;

  CTProperty<House, double> chimneyXPosition;
  CTProperty<House, double> chimneyYPosition;
  CTProperty<House, double> chimneyWidth;
  CTProperty<House, double> chimneyHeight;

  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

protected:
  virtual void grRender() const; 

private:
  House(const House&);
  House& operator=(const House&);

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;
};

static const char vcid_house_h[] = "$Header$";
#endif // !HOUSE_H_DEFINED
