///////////////////////////////////////////////////////////////////////
//
// fixpt.h
// Rob Peters
// created: Jan-99
// written: Thu Oct 21 18:10:10 1999
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
//
// FixPt class declaration
//
///////////////////////////////////////////////////////////////////////

class FixPt : public GrObj, public PropFriend<FixPt> {
public:
  FixPt (double len=0.1, int wid=1);
  FixPt (istream &is, IOFlag flag);
  virtual ~FixPt ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<FixPt> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTProperty<FixPt, double> length;	// length of crosshairs in GL coordinates
  CTProperty<FixPt, int> width; // width of crosshairs in pixels

protected:
  virtual bool grGetBoundingBox(double& left, double& top,
										  double& right, double& bottom,
										  int& border_pixels) const;

  virtual void grRender() const;
};

static const char vcid_fixpt_h[] = "$Header$";
#endif // !FIXPT_H_DEFINED
