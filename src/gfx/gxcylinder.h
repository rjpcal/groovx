/** @file gfx/gxcylinder.h GxNode subclass for drawing cylinders+cones */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 12:43:20 2002
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

#ifndef GROOVX_GFX_GXCYLINDER_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXCYLINDER_H_UTC20050626084024_DEFINED

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

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

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

#endif // !GROOVX_GFX_GXCYLINDER_H_UTC20050626084024_DEFINED
