///////////////////////////////////////////////////////////////////////
//
// cloneface.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Apr 29 09:19:25 1999
// commit: $Id$
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

#ifndef CLONEFACE_H_DEFINED
#define CLONEFACE_H_DEFINED

#include "visx/face.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c CloneFace is a subclass of \c Face that adds the ability to
 * control additional paramters for Brunswik face rendering. This is
 * achieved by overriding Face's \c getCtrlPnts(), \c getEyeAspect(),
 * and \c getVertOffset() functions to return CloneFace data
 * members. (In the base class \c Face, these functions just return
 * constant data.)
 *
 **/
///////////////////////////////////////////////////////////////////////

class CloneFace : public Face
{
protected:
  /// Default constructor.
  CloneFace();

public:
  /// Default creation function.
  static CloneFace* make();

  /// Virtual destructor.
  virtual ~CloneFace() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Returns an array of Bezier control points for face outline.
  virtual const double* getCtrlPnts() const;

  /// Returns the aspect ratio of eye outline.
  virtual double getEyeAspect() const;

  /** Returns the amount of vertical offset applied to all internal
      face features. */
  virtual double getVertOffset() const;

  /// Get CloneFace's fields.
  static const FieldMap& classFields();

private:
  /// control aspect ratio of eye outline
  double itsEyeAspect;

  /// amount of vertical offset of all features
  double itsVertOffset;

  double itsCtrlPnts[24];       // Bezier control points for face outline
};

static const char vcid_cloneface_h[] = "$Id$ $URL$";
#endif // !CLONEFACE_H_DEFINED
