///////////////////////////////////////////////////////////////////////
//
// gxcylinder.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul  5 12:43:20 2002
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

#ifndef GXCYLINDER_H_DEFINED
#define GXCYLINDER_H_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

//  #######################################################
//  =======================================================

/// GxCylinder is a GxNode subclass for drawing cylinders (also cones).

class GxCylinder : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxCylinder();

public:
  /// Default creator.
  static GxCylinder* make();

  /// Virtual destructor.
  virtual ~GxCylinder() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  ////////////
  // fields //
  ////////////

private:
  double itsBase;
  double itsTop;
  double itsHeight;
  int itsSlices;
  int itsStacks;
  bool itsFilled;

public:
  /// Get GxCylinder's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the cylinder.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the cylinder.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxCylinder(const GxCylinder&);
  GxCylinder& operator=(const GxCylinder&);
};

static const char vcid_gxcylinder_h[] = "$Id$ $URL$";
#endif // !GXCYLINDER_H_DEFINED
