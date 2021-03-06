/** @file gfx/gxpointset.h GxNode subclass for drawing sets of points */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul  5 12:43:20 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

#include "rutz/arrayvalue.h"

//  #######################################################
//  =======================================================

/// GxPointSet is a GxNode subclass for drawing sets pf points.

class GxPointSet : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxPointSet();

public:
  /// Default creator.
  static GxPointSet* make();

  /// Virtual destructor.
  virtual ~GxPointSet() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  ////////////
  // fields //
  ////////////

  /// The array of vertices that will be drawn onscreen.
  rutz::array_value<GbVec3<double> > itsPoints;

private:
  int itsStyle;

public:
  /// Get GxPointSet's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the pointset.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

  /// Draw the pointset.
  virtual void draw(Gfx::Canvas&) const override;

private:
  GxPointSet(const GxPointSet&);
  GxPointSet& operator=(const GxPointSet&);
};

#endif // !GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED
