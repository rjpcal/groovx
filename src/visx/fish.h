///////////////////////////////////////////////////////////////////////
//
// fish.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:56 1999
// written: Fri Jun  1 14:20:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_H_DEFINED
#define FISH_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRACER_H_DEFINED)
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

class Fish : public GrObj, public FieldContainer {
public:
  /// This tracer dynamically controls the tracing of \c Fish member functions.
  static Util::Tracer tracer;

protected:
  /** Constructor can make a Fish by reading a spline file and a
      coordinate file. If either of these filenames are null, the
      constructor will use default values for the spline
      coefficients. */
  Fish(const char* splinefile=0, const char* coordfile=0, int index=0);

public:
  static Fish* make();

  static Fish* makeFromFiles(const char* splinefile,
									  const char* coordfile, int index);

private:
  void restoreToDefault();

  void readSplineFile(const char* splinefile);
  void readCoordFile(const char* coordfile, int index);

public:
  /// Virtual destructor.
  virtual ~Fish();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /** Reimplemented from \c GrObj in order to catch changes to \c
      currentPart and \c currentEndPt, so that we can reseat the
      referents of \endPt_Part and endPt_Bkpt, respectively. */
  virtual void receiveStateChangeMsg(const Observable* obj);

  ////////////////
  // properties //
  ////////////////

  /** The category of the fish. The semantics of \a category are
      defined by the client. */
  TField<int> fishCategory;

  virtual int category() const { return fishCategory(); }
  virtual void setCategory(int val) { fishCategory.setNative(val); }

  /// Controls the shape of the dorsal fin.
  TPtrField<double> dorsalFinCoord;

  /// Controls the shape of the tail fin.
  TPtrField<double> tailFinCoord;

  /// Controls the shape of the lower fin (one fin vs. two fins).
  TPtrField<double> lowerFinCoord;

  /// Controls the shape of the mouth
  TPtrField<double> mouthCoord;

  /// Selects the current part for editing.
  TBoundedField<int> currentPart;

  /// Selects the current end point for editing.
  TBoundedField<int> currentEndPt;

  /// Controls the part referred to by the current end point.
  TPtrField<int> endPt_Part;

  /// Controls the breakpoint referred to by the current end point.
  TPtrField<int> endPt_Bkpt;

  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas, DrawMode mode) const;

private:
  Fish(const Fish&);
  Fish& operator=(const Fish&);

  struct EndPt;
  struct FishPart;

  FishPart* itsFishParts;
  EndPt* itsEndPts;
  double itsCoords[4];
};

static const char vcid_fish_h[] = "$Header$";
#endif // !FISH_H_DEFINED
