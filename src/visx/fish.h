///////////////////////////////////////////////////////////////////////
//
// fish.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:56 1999
// written: Wed Oct 20 10:05:00 1999
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
  Fish(const char* splinefile=0, const char* coordfile=0, int index=0);

private:
  void restoreToDefault();

  void readSplineFile(const char* splinefile);
  void readCoordFile(const char* coordfile, int index);

public:
  virtual ~Fish();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);

  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  virtual void receiveStateChangeMsg(const Observable* obj);

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
  CTPtrProperty<Fish, int> endPt_Part;
  CTPtrProperty<Fish, int> endPt_Bkpt;

  /////////////
  // actions //
  /////////////

protected:
  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

  virtual void grRender() const;

private:
  void makeIoList(vector<IO *>& vec);
  void makeIoList(vector<const IO *>& vec) const;

  struct EndPt;
  struct FishPart;

  FishPart* itsFishParts;
  EndPt* itsEndPts;
  double itsCoords[4];
};

static const char vcid_fish_h[] = "$Header$";
#endif // !FISH_H_DEFINED
