/** @file gfx/gxsphere.h GxNode subclass for drawing spheres */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 14:18:59 2002
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

#ifndef GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED

#include "gfx/gxnode.h"

#include "io/fields.h"

//  #######################################################
//  =======================================================

/// GxSphere is a GxNode subclass for drawing spheres.

class GxSphere : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxSphere();

public:
  /// Default creator.
  static GxSphere* make();

  /// Virtual destructor.
  virtual ~GxSphere() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  ////////////
  // fields //
  ////////////

private:
  double itsRadius;
  int itsSlices;
  int itsStacks;
  bool itsFilled;

public:
  /// Get GxSphere's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the sphere.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  /// Draw the sphere.
  virtual void draw(Gfx::Canvas&) const override;

private:
  GxSphere(const GxSphere&);
  GxSphere& operator=(const GxSphere&);
};

#endif // !GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED
