///////////////////////////////////////////////////////////////////////
//
// fish.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:56 1999
// written: Wed Aug 29 13:30:02 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_H_DEFINED
#define FISH_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
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

class Fish : public GrObj
{
private:
  Fish(const Fish&);
  Fish& operator=(const Fish&);

  struct Part;
  Part* itsParts;

  void restoreToDefault();

  void readSplineFile(const char* splinefile);
  void readCoordFile(const char* coordfile, int index);

  // Catches changes to currentPart and currentEndPt, so that we can
  // reseat the referents of itsEndPt_Part and itsEndPt_Bkpt,
  // respectively.
  void updatePtrs();

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

  /// Virtual destructor.
  virtual ~Fish();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  /// This tracer dynamically controls the tracing of \c Fish member functions.
  static Util::Tracer tracer;

  ////////////////
  // properties //
  ////////////////

  virtual int category() const
    { return itsFishCategory; }
  virtual void setCategory(int val)
    { itsFishCategory = val; this->sigNodeChanged.emit(); }

private:
  /** The category of the fish. The semantics of \a category are
      defined by the client. */
  int itsFishCategory;

  /// Controls the shape of the dorsal fin.
  double* itsDorsalFinCoord;

  /// Controls the shape of the tail fin.
  double* itsTailFinCoord;

  /// Controls the shape of the lower fin (one fin vs. two fins).
  double* itsLowerFinCoord;

  /// Controls the shape of the mouth
  double* itsMouthCoord;

  /// Selects the current part for editing.
  int itsCurrentPart;

  /// Selects the current end point for editing.
  int itsCurrentEndPt;

  /// Controls the part referred to by the current end point.
  int* itsEndPt_Part;

  /// Controls the breakpoint referred to by the current end point.
  int* itsEndPt_Bkpt;

  /** Controls whether the different parts of the fish will be
      rendered in different colors. */
  bool inColor;

  /// Controls whether the B-spline control points will be shown
  bool showControlPoints;

  /////////////
  // actions //
  /////////////

protected:
  virtual Gfx::Rect<double> grGetBoundingBox() const;

  virtual void grRender(Gfx::Canvas& canvas) const;
};

static const char vcid_fish_h[] = "$Header$";
#endif // !FISH_H_DEFINED
