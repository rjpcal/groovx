/** @file visx/cloneface.h graphic object for modified Brunswik faces
    with some additional parameters */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Apr 29 09:19:25 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_CLONEFACE_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_CLONEFACE_H_UTC20050626084016_DEFINED

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

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

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

static const char __attribute__((used)) vcid_groovx_visx_cloneface_h_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_CLONEFACE_H_UTC20050626084016_DEFINED
