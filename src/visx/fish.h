///////////////////////////////////////////////////////////////////////
//
// fish.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:56 1999
// written: Tue Aug 21 13:28:41 2001
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

class Fish : public GrObj {
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
      referents of \itsEndPt_Part and itsEndPt_Bkpt, respectively. */
  virtual void receiveSignal();

  ////////////////
  // properties //
  ////////////////

  virtual int category() const
    { return itsFishCategory; }
  virtual void setCategory(int val)
    { itsFishCategory = val; this->sigNodeChanged.emitSignal(); }

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

public:
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

protected:
  virtual Gfx::Rect<double> grGetBoundingBox() const;

  virtual void grRender(Gfx::Canvas& canvas) const;

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
