///////////////////////////////////////////////////////////////////////
//
// fish.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:44:56 1999
// written: Thu Feb 17 13:30:05 2000
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

#ifndef TRACER_H_DEFINED
#include "util/tracer.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Fish is a subclass of \c GrObj that can render outline drawings
 * of tropical fish (rendering algorithm designed by Fabrizio
 * Gabbiani).
 *
 **/
///////////////////////////////////////////////////////////////////////

class Fish : public GrObj, public PropFriend<Fish> {
public:
  /// This tracer dynamically controls the tracing of \c Fish member functions.
  static Util::Tracer tracer;


  /** Constructor can make a Fish by reading a spline file and a
      coordinate file. If either of these filenames are null, the
      constructor will use default values for the spline
      coefficients. */
  Fish(const char* splinefile=0, const char* coordfile=0, int index=0);

private:
  void restoreToDefault();

  void readSplineFile(const char* splinefile);
  void readCoordFile(const char* coordfile, int index);

public:
  /// Virtual destructor.
  virtual ~Fish();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  /** Reimplemented from \c GrObj in order to catch changes to \c
      currentPart and \c currentEndPt, so that we can reseat the
      referents of \endPt_Part and endPt_Bkpt, respectively. */
  virtual void receiveStateChangeMsg(const Observable* obj);

  ////////////////
  // properties //
  ////////////////

  /// Info about a \c Fish property.
  typedef PropertyInfo<Fish> PInfo;

  /// Return a collection of info about all \c Fish properties.  
  static const vector<PInfo>& getPropertyInfos();

  /** The category of the fish. The semantics of \a category are
      defined by the client. */
  CTProperty<Fish, int> category;

  virtual int getCategory() const { return category.getNative(); }
  virtual void setCategory(int val) { category.setNative(val); }

  /// Controls the shape of the dorsal fin.
  CTPtrProperty<Fish, double> dorsalFinCoord;

  /// Controls the shape of the tail fin.
  CTPtrProperty<Fish, double> tailFinCoord;

  /// Controls the shape of the lower fin (one fin vs. two fins).
  CTPtrProperty<Fish, double> lowerFinCoord;

  /// Controls the shape of the mouth
  CTPtrProperty<Fish, double> mouthCoord;

  /// Selects the current part for editing.
  CTBoundedProperty<Fish, int, 0, 3, 1> currentPart;

  /// Selects the current end point for editing.
  CTBoundedProperty<Fish, int, 0, 3, 1> currentEndPt;

  /// Controls the part referred to by the current end point.
  CTPtrProperty<Fish, int> endPt_Part;

  /// Controls the breakpoint referred to by the current end point.
  CTPtrProperty<Fish, int> endPt_Bkpt;

  /////////////
  // actions //
  /////////////

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(Canvas& canvas) const;

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
