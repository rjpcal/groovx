///////////////////////////////////////////////////////////////////////
//
// fish.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:56 1999
// written: Tue Oct  5 12:36:04 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_H_DEFINED
#define FISH_H_DEFINED

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
// Fish class definition
//
///////////////////////////////////////////////////////////////////////

class Fish : public GrObj, public PropFriend<Fish> {
public:
  Fish();
  Fish(const char* splinefile, const char* coordfile, int index);

private:
  void read_splinefile(const char* splinefile);
  void read_coordfile(const char* coordfile, int index);

public:
  virtual ~Fish();

  virtual void serialize(ostream &os, IOFlag flag) const {}
  virtual void deserialize(istream &is, IOFlag flag) {}
  // These functions write the object's state from/to an output/input
  // stream. Both functions are defined, but are no-ops for GrObj.

  virtual int charCount() const { return 0; }

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<Fish> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<Fish, int> category;
  virtual int getCategory() const { return category.getNative(); }
  virtual void setCategory(int val) { category.setNative(val); }

  CTPtrProperty<Fish, double> coord0;
  CTPtrProperty<Fish, double> coord1;
  CTPtrProperty<Fish, double> coord2;
  CTPtrProperty<Fish, double> coord3;

  CTBoundedProperty<Fish, int, 0, 3, 1> currentPart;
  CTBoundedProperty<Fish, int, 0, 3, 1> currentEndPt;

  /////////////
  // actions //
  /////////////

protected:
  virtual void grRender() const;

private:
  struct EndPt;
  struct FishPart;

  FishPart* itsFishParts;
  EndPt* itsEndPts;
  double itsCoords[4];

  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;
};

static const char vcid_fish_h[] = "$Header$";
#endif // !FISH_H_DEFINED
