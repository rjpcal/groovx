///////////////////////////////////////////////////////////////////////
//
// fish.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 29 11:44:56 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_H_DEFINED
#define FISH_H_DEFINED

#include "gfx/gxshapekit.h"

#include "util/tracer.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Fish is a subclass of \c GxShapeKit that can render outline drawings
 * of tropical fish (rendering algorithm designed by Fabrizio
 * Gabbiani).
 *
 **/
///////////////////////////////////////////////////////////////////////

class Fish : public GxShapeKit
{
private:
  Fish(const Fish&);
  Fish& operator=(const Fish&);

  struct Part;
  Part* itsParts;

  void restoreToDefault();

  void readSplineFile(const char* splinefile);
  void readCoordFile(const char* coordfile, int index);

  // Catches changes to currentPart, so that we can reseat the
  // referent of itsCurrentPartBkpt.
  void updatePtrs();

protected:
  /** Constructor can make a Fish by reading a spline file and a
      coordinate file. If either of these filenames are null, the
      constructor will use default values for the spline
      coefficients. */
  Fish(const char* splinefile=0, const char* coordfile=0, int index=0);

public:

  /// Factory function.
  static Fish* make();

  /// Create from files specifying the spline values.
  static Fish* makeFromFiles(const char* splinefile,
                             const char* coordfile, int index);

  /// Virtual destructor.
  virtual ~Fish() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get Fish's fields.
  static const FieldMap& classFields();

  /// This tracer dynamically controls the tracing of \c Fish member functions.
  static rutz::tracer tracer;

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

  /// Controls the breakpoint referred to by the current end point.
  int* itsCurrentPartBkpt;

  /** Controls whether the different parts of the fish will be
      rendered in different colors. */
  bool inColor;

  /// Controls whether the B-spline control points will be shown
  bool showControlPoints;

  /// Selective masks the rendering of the different parts.
  int partsMask;

  /// Controls the degree of the current swimming stroke.
  double swimStroke;

  /////////////
  // actions //
  /////////////

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;
};

static const char vcid_fish_h[] = "$Id$ $URL$";
#endif // !FISH_H_DEFINED
