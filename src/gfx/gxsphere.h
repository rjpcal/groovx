/** @file gfx/gxsphere.h GxNode subclass for drawing spheres */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 14:18:59 2002
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

#ifndef GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED

#include "gfx/gxnode.h"

#include "field/fields.h"

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
  virtual ~GxSphere() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

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
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the sphere.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxSphere(const GxSphere&);
  GxSphere& operator=(const GxSphere&);
};

static const char vcid_groovx_gfx_gxsphere_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXSPHERE_H_UTC20050626084024_DEFINED
