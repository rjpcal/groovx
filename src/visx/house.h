///////////////////////////////////////////////////////////////////////
//
// house.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:15 1999
// written: Tue Sep 21 15:04:01 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_H_DEFINED
#define HOUSE_H_DEFINED

#ifndef IOSTL_H_DEFINED
#include "iostl.h"
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifdef PROPERTY
#error PROPERTY macro already defined
#else
#define PROPERTY(type, name) \
  private: IoWrapper<type> its##name; \
  public:  type get##name() const { return its##name(); } \
  public:  void set##name(type val) { its##name() = val; sendStateChangeMsg(); }
#endif

#ifdef BOUNDED_PROPERTY
#error BOUNDED_PROPERTY macro already defined
#else
#define BOUNDED_PROPERTY(type, name, min, max) \
  private: IoWrapper<type> its##name; \
  public:  type get##name() const { return its##name(); } \
  public:  void set##name(type val) { \
				  if (val >= min && val <= max) its##name() = val; \
				  sendStateChangeMsg(); }
#endif

///////////////////////////////////////////////////////////////////////
//
// House class definition
//
///////////////////////////////////////////////////////////////////////

class House : public GrObj {
public:
  House();
  virtual ~House();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  
  virtual int charCount() const;

  virtual int getCategory() const { return -1; }

  virtual void setCategory(int) {  }

  PROPERTY(double, StoryAspectRatio);
  PROPERTY(int, NumStories);

  PROPERTY(int, DoorPosition);
  PROPERTY(double, DoorWidth);  // fraction of avail. space
  PROPERTY(double, DoorHeight); // fraction of one story
  PROPERTY(bool, DoorOrientation); // left or right

  PROPERTY(int, NumWindows);
  PROPERTY(double, WindowWidth);	// fraction of avail. space
  PROPERTY(double, WindowHeight); // fraction of one story
  PROPERTY(int, WindowVertBars);
  PROPERTY(int, WindowHorizBars);

  PROPERTY(int, RoofShape);
  PROPERTY(double, RoofOverhang);
  PROPERTY(double, RoofHeight);
  PROPERTY(int, RoofColor);

  PROPERTY(double, ChimneyXPosition);
  PROPERTY(double, ChimneyYPosition);
  PROPERTY(double, ChimneyWidth);
  PROPERTY(double, ChimneyHeight);

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

#undef PROPERTY

static const char vcid_house_h[] = "$Header$";
#endif // !HOUSE_H_DEFINED
