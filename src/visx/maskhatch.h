///////////////////////////////////////////////////////////////////////
//
// maskhatch.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 15:49:58 1999
// written: Mon Nov 15 15:48:01 1999
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
//
// MaskHatch class definition --
//
///////////////////////////////////////////////////////////////////////

class MaskHatch : public GrObj, public PropFriend<MaskHatch> {
public:
  MaskHatch();
  virtual ~MaskHatch();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);

  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  ////////////////
  // properties //
  ////////////////

  typedef PropertyInfo<MaskHatch> PInfo;
  static const vector<PInfo>& getPropertyInfos();

  CTBoundedProperty<MaskHatch, int, 0, 1000, 1> numLines;
  CTBoundedProperty<MaskHatch, int, 0, 1000, 1> lineWidth;

protected:
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual void grRender() const; 
};

static const char vcid_maskhatch_h[] = "$Header$";
#endif // !MASKHATCH_H_DEFINED
